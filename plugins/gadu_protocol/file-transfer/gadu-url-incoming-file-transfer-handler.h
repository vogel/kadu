/*
 * %kadu copyright begin%
 * Copyright 2015 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#pragma once

#include "file-transfer/url-incoming-file-transfer-handler.h"

#include <QtCore/QPointer>

class GaduDriveGetTransfer;
class GaduProtocol;

class QNetworkReply;

class GaduUrlIncomingFileTransferHandler : public UrlIncomingFileTransferHandler
{
	Q_OBJECT

public:
	explicit GaduUrlIncomingFileTransferHandler(GaduProtocol *protocol, FileTransfer fileTransfer);
	virtual ~GaduUrlIncomingFileTransferHandler();

	virtual void save(QIODevice *destination);

private:
	QPointer<GaduProtocol> m_protocol;
	QPointer<GaduDriveGetTransfer> m_getTransfer;
	QPointer<QIODevice> m_destination;

	void clenaup();

private slots:
	void downloadProgress(qint64 bytesReceived, qint64 bytesTotal);
	void downloadFinished(QNetworkReply*);

};
