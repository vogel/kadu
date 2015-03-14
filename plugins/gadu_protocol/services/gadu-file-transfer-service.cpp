/*
 * %kadu copyright begin%
 * Copyright 2011, 2012, 2013 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2011, 2013, 2014, 2015 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "gadu-file-transfer-service.h"
#include "gadu-imtoken-service.h"

#include "file-transfer/gadu-outgoing-file-transfer-handler.h"
#include "file-transfer/gadu-url-incoming-file-transfer-handler.h"
#include "helpers/gadu-protocol-helper.h"
#include "gadu-account-details.h"
#include "gadu-protocol.h"

#include "contacts/contact-manager.h"
#include "core/core.h"
#include "file-transfer/file-transfer-direction.h"
#include "file-transfer/file-transfer-manager.h"
#include "file-transfer/file-transfer-status.h"
#include "file-transfer/file-transfer-type.h"
#include "file-transfer/gui/file-transfer-can-send-result.h"
#include "misc/misc.h"

#include <QtCore/QUrl>


GaduFileTransferService::GaduFileTransferService(GaduProtocol *protocol) :
		FileTransferService(protocol), Protocol(protocol)
{
	connect(Protocol, SIGNAL(connected(Account)), this, SIGNAL(canSendChanged()));
	connect(Protocol, SIGNAL(disconnected(Account)), this, SIGNAL(canSendChanged()));
}

GaduFileTransferService::~GaduFileTransferService()
{
}

void GaduFileTransferService::setGaduIMTokenService(GaduIMTokenService *imTokenService)
{
	m_imTokenService = imTokenService;

	connect(m_imTokenService, SIGNAL(imTokenChanged(QByteArray)), this, SIGNAL(canSendChanged()));
}

FileTransferHandler * GaduFileTransferService::createFileTransferHandler(FileTransfer fileTransfer)
{
	if (fileTransfer.transferDirection() == FileTransferDirection::Incoming)
		return new GaduUrlIncomingFileTransferHandler{Protocol, fileTransfer};
	else
		return new GaduOutgoingFileTransferHandler{Protocol, fileTransfer};
}

FileTransferCanSendResult GaduFileTransferService::canSend(Contact contact)
{
	if (Core::instance()->myself() == contact.ownerBuddy())
		return {false, {}};

	if (!Protocol->isConnected())
		return {false, tr("Connect before sending files.")};

	if (!Protocol->secureConnection())
		return {false, tr("Enable SSL in account configuration and reconnect before sending files.")};

	if (m_imTokenService->imToken().isEmpty())
		return {false, tr("Unable to login to GG Drive. Reconnect before sending files.")};

	return {true, {}};
}

void GaduFileTransferService::fileTransferReceived(Contact peer, QString downloadId, QString fileName)
{
	auto transfer = FileTransfer::create();
	transfer.setPeer(peer);
	transfer.setTransferDirection(FileTransferDirection::Incoming);
	transfer.setTransferType(FileTransferType::Url);
	transfer.setTransferStatus(FileTransferStatus::ReadyToDownload);
	transfer.setRemoteFileName(QUrl::fromPercentEncoding(fileName.toUtf8()));
	transfer.setFileSize(0); // we don't know file size yet
	transfer.addProperty("gg:downloadId", downloadId, CustomProperties::Storable);
	transfer.addProperty("gg:remoteFileName", fileName, CustomProperties::Storable);

	emit incomingFileTransfer(transfer);
}

#include "moc_gadu-file-transfer-service.cpp"
