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

#include "jabber-stream-incoming-file-transfer-handler.h"

#include "jabber-protocol.h"

#include "file-transfer/file-transfer-status.h"

#include <QtCore/QFileInfo>
#include <qxmpp/QXmppTransferManager.h>

JabberStreamIncomingFileTransferHandler::JabberStreamIncomingFileTransferHandler(FileTransfer transfer) :
		StreamIncomingFileTransferHandler{transfer},
		m_inProgress{false}
{
}

JabberStreamIncomingFileTransferHandler::~JabberStreamIncomingFileTransferHandler()
{
	cleanup(transfer().transferStatus());
}

void JabberStreamIncomingFileTransferHandler::setTransferJob(QXmppTransferJob *transferJob)
{
	if (m_transferJob)
		disconnect(m_transferJob, 0, this, 0);
	m_transferJob = transferJob;
	if (m_transferJob)
	{
		connect(m_transferJob, SIGNAL(progress(qint64,qint64)), this, SLOT(progress(qint64,qint64)));
		connect(m_transferJob, SIGNAL(stateChanged(QXmppTransferJob::State)), this, SLOT(stateChanged(QXmppTransferJob::State)));
		connect(m_transferJob, SIGNAL(error(QXmppTransferJob::Error)), this, SLOT(error(QXmppTransferJob::Error)));
	}
}

void JabberStreamIncomingFileTransferHandler::progress(qint64 progress, qint64 total)
{
	transfer().setTransferredSize(progress);
	transfer().setFileSize(total);

	emit statusChanged();
}

void JabberStreamIncomingFileTransferHandler::cleanup(FileTransferStatus status)
{
	transfer().setTransferStatus(status);
	m_inProgress = false;

	if (m_transferJob)
	{
		if (m_transferJob->state() == QXmppTransferJob::TransferState)
			m_transferJob->abort();
		m_transferJob->deleteLater();
	}

	if (m_destination)
	{
		m_destination->close();
		m_destination->deleteLater();
	}
}

void JabberStreamIncomingFileTransferHandler::accept(QIODevice *destination)
{
	m_destination = destination;

	transfer().setTransferStatus(FileTransferStatus::Transfer);
	transfer().setTransferredSize(0);
	transfer().setFileSize(m_transferJob->fileSize());

	m_transferJob->accept(m_destination);

}

void JabberStreamIncomingFileTransferHandler::reject()
{
	cleanup(FileTransferStatus::Rejected);
}

void JabberStreamIncomingFileTransferHandler::stateChanged(QXmppTransferJob::State state)
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

void JabberStreamIncomingFileTransferHandler::error(QXmppTransferJob::Error error)
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

#include "moc_jabber-stream-incoming-file-transfer-handler.cpp"
