/*
 * %kadu copyright begin%
 * Copyright 2015 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * %kadu copyright end%
 *
 * Contains code from QtSingleApplication.
 * Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

#include "single-application.h"

#include "long-lived-lock-file.h"

#include <QtCore/QCoreApplication>
#include <QtCore/QDir>
#include <QtCore/QRegExp>
#include <QtNetwork/QLocalServer>
#include <QtNetwork/QLocalSocket>
#include <memory>

#if defined(Q_OS_WIN)
#	include <QtCore/QLibrary>
#	include <QtCore/qt_windows.h>
	typedef BOOL(WINAPI*PProcessIdToSessionId)(DWORD,DWORD*);
	static PProcessIdToSessionId pProcessIdToSessionId = 0;
#endif

#if defined(Q_OS_UNIX)
#	include <sys/types.h>
#	include <time.h>
#	include <unistd.h>
#endif

SingleApplication::SingleApplication(
	const QString &applicationId,
	std::function<void(void)> executeAsFirst,
	std::function<void(SingleApplication &)> executeAsNext,
	std::function<void(const QString &)> onReceiveMessage,
	QObject *parent) :
		QObject{parent},
		m_localServer{nullptr},
		m_onReceiveMessage{onReceiveMessage}
{
	auto finalApplicationId = SingleApplication::defaultApplicationId(applicationId);
	auto prefix = SingleApplication::normalizedPrefix(applicationId.isEmpty(), finalApplicationId);
	m_socketName = SingleApplication::socketName(prefix, finalApplicationId);
	auto lockName = SingleApplication::lockName(m_socketName);

	LongLivedLockFile lockFile{lockName};
	if (lockFile.isLocked())
	{
		startServer();
		if (executeAsFirst)
			executeAsFirst();
	}
	else
	{
		if (executeAsNext)
			executeAsNext(*this);
	}
}

SingleApplication::~SingleApplication()
{
}

QString SingleApplication::defaultApplicationId(const QString &applicationId)
{
	if (!applicationId.isEmpty())
		return applicationId;

	auto result = QCoreApplication::applicationFilePath();
#if defined(Q_OS_WIN)
	result = result.toLower();
#endif

	return result;
}

QString SingleApplication::normalizedPrefix(bool useOnlyLastSection, const QString &applicationId)
{
	auto result = applicationId;
	if (useOnlyLastSection)
		result = result.section(QLatin1Char{'/'}, -1);

	result.remove(QRegExp{"[^a-zA-Z]"});
	result.truncate(24);

	return result;
}

QString SingleApplication::socketName(const QString &prefix, const QString &applicationId)
{
	auto idc = applicationId.toUtf8();
	auto idNum = qChecksum(idc.constData(), static_cast<uint>(idc.size()));
	auto result = QString{QLatin1String{"qtsingleapp-"} + prefix + QLatin1Char{'-'} + QString::number(idNum, 16)};

#if defined(Q_OS_WIN)
	if (!pProcessIdToSessionId)
	{
		QLibrary lib{"kernel32"};
		pProcessIdToSessionId = (PProcessIdToSessionId)lib.resolve("ProcessIdToSessionId");
	}

	if (pProcessIdToSessionId)
	{
		DWORD sessionId = 0;
		pProcessIdToSessionId(GetCurrentProcessId(), &sessionId);
		result += QLatin1Char('-') + QString::number(sessionId, 16);
	}
#else
	result += QLatin1Char{'-'} + QString::number(::getuid(), 16);
#endif

	return result;
}

QString SingleApplication::lockName(const QString &socketName)
{
	return QString{QDir{QDir::tempPath()}.absolutePath() + QLatin1Char{'/'} + socketName + QLatin1String{"-lockfile"}};
}

void SingleApplication::startServer()
{
	m_localServer = new QLocalServer{this};
	auto res = m_localServer->listen(m_socketName);

#if defined(Q_OS_UNIX)
	// ### Workaround
	if (!res && m_localServer->serverError() == QAbstractSocket::AddressInUseError)
	{
		QFile::remove(QDir::cleanPath(QDir::tempPath()) + QLatin1Char{'/'} + m_socketName);
		res = m_localServer->listen(m_socketName);
	}
#endif

	if (!res)
	{
		qWarning("QtSingleCoreApplication: listen on local socket failed, %s", qPrintable(m_localServer->errorString()));
		return;
	}

	QObject::connect(m_localServer, SIGNAL(newConnection()), SLOT(receiveConnection()));
}

void SingleApplication::receiveConnection()
{
	auto socket = std::unique_ptr<QLocalSocket>(m_localServer->nextPendingConnection());
	if (!socket)
		return;

	while (socket->bytesAvailable() < (int)sizeof(quint32))
		socket->waitForReadyRead();

	QDataStream ds{socket.get()};
	auto uMsg = QByteArray{};
	auto remaining = quint32{};

	ds >> remaining;
	uMsg.resize(static_cast<int>(remaining));
	auto got = 0;
	auto uMsgBuf = uMsg.data();
	do {
		got = ds.readRawData(uMsgBuf, static_cast<int>(remaining));
		uMsgBuf += got;
		if (got < 0)
			break;
		if (static_cast<quint32>(got) <= remaining)
			remaining = remaining - static_cast<quint32>(got);
		else
			break;
	} while (static_cast<int>(remaining) && got >= 0 && socket->waitForReadyRead(2000));

	if (got < 0)
	{
		qWarning() << "SingleApplication: Message reception failed" << socket->errorString();
		return;
	}

	auto message = QString::fromUtf8(uMsg);

	if (m_onReceiveMessage)
		m_onReceiveMessage(message);
}

void SingleApplication::sendMessage(const QString &message, int timeout) const
{
	QLocalSocket socket;
	auto connOk = false;
	for (int i = 0; i < 2; i++)
	{
		// Try twice, in case the other instance is just starting up
		socket.connectToServer(m_socketName);
		connOk = socket.waitForConnected(timeout / 2);
		if (connOk || i)
			break;
		int ms = 250;
#if defined(Q_OS_WIN)
		Sleep(DWORD(ms));
#else
		struct timespec ts = { ms / 1000, (ms % 1000) * 1000 * 1000 };
		nanosleep(&ts, NULL);
#endif
	}
	if (!connOk)
		return;

	auto uMsg = QByteArray{message.toUtf8()};
	QDataStream ds{&socket};
	ds.writeBytes(uMsg.constData(), static_cast<uint>(uMsg.size()));
	socket.waitForBytesWritten(timeout);
}

#include "moc_single-application.cpp"
