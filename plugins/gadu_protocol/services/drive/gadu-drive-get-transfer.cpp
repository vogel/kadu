/*
 * %kadu copyright begin%
 * Copyright 2015 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * %kadu copyright end%
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include "gadu-drive-get-transfer.h"

#include <QtCore/QFile>
#include <QtCore/QFileInfo>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkReply>
#include <QtNetwork/QNetworkRequest>

GaduDriveGetTransfer::GaduDriveGetTransfer(QString downloadId, QString remoteFileName, QString localFileName,
	QNetworkAccessManager *networkAccessManager, QObject *parent) :
		QObject{parent}
{
	m_file = new QFile{localFileName, this};
	if (!m_file->open(QFile::WriteOnly | QIODevice::Truncate))
		return;

	auto url = QString{"http://p.gg.pl/p/d/%1/%2"}.arg(downloadId).arg(remoteFileName);

	QNetworkRequest request;
	request.setUrl(QUrl{url});
	request.setRawHeader("Connection", "keep-alive");

	m_reply = networkAccessManager->get(request);
	connect(m_reply, SIGNAL(readyRead()), this, SLOT(readyRead()));
	connect(m_reply, SIGNAL(downloadProgress(qint64,qint64)), this, SIGNAL(downloadProgress(qint64,qint64)));
	connect(m_reply, SIGNAL(finished()), this, SLOT(requestFinished()));
}

GaduDriveGetTransfer::~GaduDriveGetTransfer()
{
	if (m_reply)
		m_reply->deleteLater();
}

bool GaduDriveGetTransfer::fileOpened() const
{
	return m_file && m_file->isOpen();
}

void GaduDriveGetTransfer::readyRead()
{
	m_file->write(m_reply->readAll());
}

void GaduDriveGetTransfer::requestFinished()
{
	deleteLater();
}
