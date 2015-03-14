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

#include <QtCore/QFileInfo>

#include "file-transfer/file-transfer-status.h"

#include "services/jabber-resource-service.h"
#include "jabber-account-details.h"
#include "jabber-protocol.h"

#include "jabber-outgoing-file-transfer-handler.h"

JabberOutgoingFileTransferHandler::JabberOutgoingFileTransferHandler(QXmppTransferManager *transferManager, FileTransfer transfer) :
		OutgoingFileTransferHandler{transfer},
		m_transferManager{transferManager},
		m_inProgress{false}
{
}

JabberOutgoingFileTransferHandler::~JabberOutgoingFileTransferHandler()
{
	cleanup(transfer().transferStatus());
}

void JabberOutgoingFileTransferHandler::setResourceService(JabberResourceService *resourceService)
{
	m_resourceService = resourceService;
}

void JabberOutgoingFileTransferHandler::cleanup(FileTransferStatus status)
{
	transfer().setTransferStatus(status);
	m_inProgress = false;

	if (m_transferJob)
	{
		if (m_transferJob->state() == QXmppTransferJob::TransferState)
			m_transferJob->abort();
		m_transferJob->deleteLater();
	}

	if (m_source)
	{
		m_source->close();
		m_source->deleteLater();
	}
}

void JabberOutgoingFileTransferHandler::send(QIODevice *source)
{
	if (m_inProgress) // already sending/receiving
		return;

	m_source = source;

	auto account = transfer().peer().contactAccount();
	if (account.isNull())
	{
		transfer().setTransferStatus(FileTransferStatus::NotConnected);
		deleteLater();
		return; // TODO: notify
	}

	JabberProtocol *jabberProtocol = dynamic_cast<JabberProtocol *>(account.protocolHandler());
	if (!jabberProtocol)
	{
		transfer().setTransferStatus(FileTransferStatus::NotConnected);
		deleteLater();
		return;
	}

	auto jid = 	m_resourceService->bestContactJid(transfer().peer());
	auto fileInfo = QXmppTransferFileInfo{};
	fileInfo.setName(transfer().remoteFileName());
	fileInfo.setSize(transfer().fileSize());
	m_transferJob = m_transferManager->sendFile(jid.full(), m_source, fileInfo);

	if (m_transferJob->error() == QXmppTransferJob::Error::NoError)
	{
		connect(m_transferJob, SIGNAL(progress(qint64,qint64)), this, SLOT(progress(qint64,qint64)));
		connect(m_transferJob, SIGNAL(stateChanged(QXmppTransferJob::State)), this, SLOT(stateChanged(QXmppTransferJob::State)));
		connect(m_transferJob, SIGNAL(error(QXmppTransferJob::Error)), this, SLOT(error(QXmppTransferJob::Error)));

		transfer().setTransferStatus(FileTransferStatus::WaitingForAccept);
		m_inProgress = true;
	}
	else
		error(m_transferJob->error());
}

void JabberOutgoingFileTransferHandler::stop()
{
	if (m_transferJob)
	{
		m_transferJob->abort();
		m_transferJob->deleteLater();
	}

	cleanup(FileTransferStatus::NotConnected);
}

void JabberOutgoingFileTransferHandler::progress(qint64 progress, qint64 total)
{
	transfer().setTransferredSize(progress);
	transfer().setFileSize(total);

	emit statusChanged();
}

void JabberOutgoingFileTransferHandler::stateChanged(QXmppTransferJob::State state)
{
	switch (state)
	{
		case QXmppTransferJob::State::OfferState:
			transfer().setTransferStatus(FileTransferStatus::WaitingForAccept);
			break;
		case QXmppTransferJob::State::StartState:
			transfer().setTransferStatus(FileTransferStatus::WaitingForConnection);
			break;
		case QXmppTransferJob::State::TransferState:
			transfer().setTransferStatus(FileTransferStatus::Transfer);
			break;
		case QXmppTransferJob::State::FinishedState:
			transfer().setTransferStatus(FileTransferStatus::Finished);
			break;
	}
}

void JabberOutgoingFileTransferHandler::error(QXmppTransferJob::Error error)
{
	auto errorMessage = QString{};
	switch (error)
	{
		case QXmppTransferJob::Error::AbortError:
			errorMessage = tr("Aborted");
			break;
		case QXmppTransferJob::Error::FileCorruptError:
			errorMessage = tr("File is corrupted");
			break;
		case QXmppTransferJob::Error::ProtocolError:
			errorMessage = tr("Protocol error");
			break;
		default:
			errorMessage = tr("Unknown error");
			break;
	}

	transfer().setError(errorMessage);
	cleanup(FileTransferStatus::NotConnected);
}

#include "moc_jabber-outgoing-file-transfer-handler.cpp"
