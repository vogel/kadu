/****************************************************************************
**
** Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of a Qt Solutions component.
**
** Commercial Usage
** Licensees holding valid Qt Commercial licenses may use this file in
** accordance with the Qt Solutions Commercial License Agreement provided
** with the Software or, alternatively, in accordance with the terms
** contained in a written agreement between you and Nokia.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain
** additional rights. These rights are described in the Nokia Qt LGPL
** Exception version 1.1, included in the file LGPL_EXCEPTION.txt in this
** package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** Please note Third Party Software included with Qt Solutions may impose
** additional restrictions and it is the user's responsibility to ensure
** that they have met the licensing requirements of the GPL, LGPL, or Qt
** Solutions Commercial license and the relevant license of the Third
** Party Software they are using.
**
** If you are unsure which license is appropriate for your use, please
** contact Nokia at qt-info@nokia.com.
**
****************************************************************************/

#include "qtlocalpeer.h"

#include "os/qtsingleapplication/long-lived-lock-file.h"

#include <QtCore/QCoreApplication>
#include <QtCore/QDir>
#include <QtCore/QLockFile>
#include <QtCore/QTime>
#include <QtNetwork/QAbstractSocket>
#include <QtNetwork/QLocalServer>
#include <QtNetwork/QLocalSocket>

#if defined(Q_OS_WIN)
#include <QtCore/QLibrary>
#include <QtCore/qt_windows.h>
	typedef BOOL(WINAPI*PProcessIdToSessionId)(DWORD,DWORD*);
	static PProcessIdToSessionId pProcessIdToSessionId = 0;
#endif

#if defined(Q_OS_UNIX)
#include <sys/types.h>
#include <time.h>
#include <unistd.h>
#endif

KADUAPI const char * QtLocalPeer::ack = "ack";

QtLocalPeer::QtLocalPeer(QString appId, QObject *parent) :
		QObject{parent},
		m_lockFile{nullptr}
{
	auto prefix = appId;
	if (appId.isEmpty())
	{
		appId = QCoreApplication::applicationFilePath();
#if defined(Q_OS_WIN)
		appId = appId.toLower();
#endif
		prefix = appId.section(QLatin1Char('/'), -1);
	}
	prefix.remove(QRegExp("[^a-zA-Z]"));
	prefix.truncate(24);

	auto idc = appId.toUtf8();
	auto idNum = qChecksum(idc.constData(), static_cast<uint>(idc.size()));
	m_socketName = QLatin1String{"qtsingleapp-"} + prefix +
		QLatin1Char('-') + QString::number(idNum, 16);

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
		m_socketName += QLatin1Char('-') + QString::number(sessionId, 16);
	}
#else
	m_socketName += QLatin1Char('-') + QString::number(::getuid(), 16);
#endif

	auto lockName = QString{QDir{QDir::tempPath()}.absolutePath()
		+ QLatin1Char{'/'} + m_socketName
		+ QLatin1String{"-lockfile"}};
	m_lockFile = std::unique_ptr<LongLivedLockFile>{new LongLivedLockFile{lockName}};
}

QtLocalPeer::~QtLocalPeer()
{
}

bool QtLocalPeer::isClient() const
{
	return !m_lockFile->isLocked();
}

bool QtLocalPeer::startServer()
{
	m_server = new QLocalServer{this};
	auto res = m_server->listen(m_socketName);

#if defined(Q_OS_UNIX)
	// ### Workaround
	if (!res && m_server->serverError() == QAbstractSocket::AddressInUseError)
	{
		QFile::remove(QDir::cleanPath(QDir::tempPath()) + QLatin1Char{'/'} + m_socketName);
		res = m_server->listen(m_socketName);
	}
#endif

	if (!res)
	{
		qWarning("QtSingleCoreApplication: listen on local socket failed, %s", qPrintable(m_server->errorString()));
		return false;
	}

	QObject::connect(m_server, SIGNAL(newConnection()), SLOT(receiveConnection()));
	return true;
}

bool QtLocalPeer::sendMessage(const QString &message, int timeout)
{
	if (!isClient())
		return false;

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
		return false;

	auto uMsg = QByteArray{message.toUtf8()};
	QDataStream ds{&socket};
	ds.writeBytes(uMsg.constData(), static_cast<uint>(uMsg.size()));
	bool res = socket.waitForBytesWritten(timeout);
	res &= socket.waitForReadyRead(timeout);   // wait for ack
	res &= (socket.read(qstrlen(ack)) == ack);
	return res;
}

void QtLocalPeer::receiveConnection()
{
	auto *socket = m_server->nextPendingConnection();
	if (!socket)
		return;

	while (socket->bytesAvailable() < (int)sizeof(quint32))
		socket->waitForReadyRead();

	QDataStream ds{socket};
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
		qWarning() << "QtLocalPeer: Message reception failed" << socket->errorString();
		delete socket;
		return;
	}

	auto message = QString::fromUtf8(uMsg);
	socket->write(ack, qstrlen(ack));
	socket->waitForBytesWritten(1000);
	delete socket;

	emit messageReceived(message); //### (might take a long time to return)
}
