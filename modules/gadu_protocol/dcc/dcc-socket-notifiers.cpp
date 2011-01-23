/*
 * %kadu copyright begin%
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2009 Michał Podsiadlik (michal@kadu.net)
 * Copyright 2009 Piotr Galiszewski (piotrgaliszewski@gmail.com)
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

#include <QtCore/QFile>

#include "debug.h"
#include "misc/misc.h"

#include "file-transfer/gadu-file-transfer-handler.h"
#include "socket-notifiers/gadu-protocol-socket-notifiers.h"

#include "dcc-socket-notifiers.h"

#ifdef Q_OS_WIN
#define dup(x) x
#endif

DccSocketNotifiers::~DccSocketNotifiers()
{
	if (Socket)
	{
		Socket->destroy(Socket);
		Socket = 0;
	}
}

void DccSocketNotifiers::watchFor(struct gg_dcc *socket)
{
	kdebugmf(KDEBUG_NETWORK | KDEBUG_INFO, "%p\n", socket);

	struct gg_dcc *oldSocket = Socket;

	Version = Dcc6;
	Socket = socket;
	Socket7 = 0;

	if (Socket)
	{
		DccCheckField = &Socket->check;
		GaduSocketNotifiers::watchFor(Socket->fd);
	}
	else
		GaduSocketNotifiers::watchFor(0);

	if (oldSocket && oldSocket != Socket)
		oldSocket->destroy(oldSocket);
}

void DccSocketNotifiers::watchFor(struct gg_dcc7 *socket)
{
	kdebugmf(KDEBUG_NETWORK | KDEBUG_INFO, "%p\n", socket);

	struct gg_dcc *oldSocket = Socket;

	Version = Dcc7;
	Socket = 0;
	Socket7 = socket;

	if (Socket7)
	{
		DccCheckField = &Socket7->check;

		if (-1 == Socket7->fd) // wait for accept/reject
		{
			connect(Protocol->socketNotifiers(), SIGNAL(dcc7Accepted(struct gg_dcc7 *)),
					this, SLOT(dcc7Accepted(struct gg_dcc7 *)));
			connect(Protocol->socketNotifiers(), SIGNAL(dcc7Rejected(struct gg_dcc7 *)),
					this, SLOT(dcc7Rejected(struct gg_dcc7 *)));
			return;
		}

		GaduSocketNotifiers::watchFor(Socket7->fd);
	}
	else
		GaduSocketNotifiers::watchFor(0);

	if (oldSocket)
		oldSocket->destroy(oldSocket);
}

void DccSocketNotifiers::dcc7Accepted(struct gg_dcc7 *socket)
{
	if (Socket7 != socket)
		return;

	disconnect(Protocol->socketNotifiers(), SIGNAL(dcc7Accepted(struct gg_dcc7 *)), this, SLOT(dcc7Accepted(struct gg_dcc7 *)));
	disconnect(Protocol->socketNotifiers(), SIGNAL(dcc7Rejected(struct gg_dcc7 *)), this, SLOT(dcc7Rejected(struct gg_dcc7 *)));

	watchFor(Socket7);
}

void DccSocketNotifiers::dcc7Rejected(struct gg_dcc7 *socket)
{
	if (Socket7 != socket)
		return;

	disconnect(Protocol->socketNotifiers(), SIGNAL(dcc7Accepted(struct gg_dcc7 *)), this, SLOT(dcc7Accepted(struct gg_dcc7 *)));
	disconnect(Protocol->socketNotifiers(), SIGNAL(dcc7Rejected(struct gg_dcc7 *)), this, SLOT(dcc7Rejected(struct gg_dcc7 *)));

// TODO: 0.6.6 emit finished
}

bool DccSocketNotifiers::checkRead()
{
	return DccCheckField && (*DccCheckField & GG_CHECK_READ);
}

bool DccSocketNotifiers::checkWrite()
{
	return DccCheckField && (*DccCheckField & GG_CHECK_WRITE);
}

void DccSocketNotifiers::handleEventDccError(struct gg_event *e)
{
	Q_UNUSED(e)

	kdebugmf(KDEBUG_NETWORK|KDEBUG_INFO, "%d\n", e->event.dcc_error);

	finished(false);
}

void DccSocketNotifiers::handleEventDccDone(struct gg_event *e)
{
	Q_UNUSED(e)

	kdebugf();

	finished(true);
}

void DccSocketNotifiers::handleEventDccClientAccept(struct gg_event *e)
{
	Q_UNUSED(e)

	kdebugmf(KDEBUG_NETWORK|KDEBUG_INFO, "uin:%u peer_uin:%u\n", Socket->uin, Socket->peer_uin);

	// TODO: make async TODO: 0.6.6
	if (!Manager->acceptConnection(Socket->uin, Socket->peer_uin, Socket->remote_addr))
	{
		done(false);
		return;
	}
}

void DccSocketNotifiers::handleEventDccCallback(struct gg_event *e)
{
	Q_UNUSED(e)

	kdebugmf(KDEBUG_NETWORK|KDEBUG_INFO, "uin:%u peer_uin:%u\n", Socket->uin, Socket->peer_uin);

	GaduFileTransferHandler *handler = Manager->findFileTransferHandler(this);
	if (handler)
	{
		gg_dcc_set_type(Socket, GG_SESSION_DCC_SEND); // TODO: 0.6.6 for voice it wont look that way
		handler->setFileTransferNotifiers(this);
		return;
	}
	else
		finished(false);
}

void DccSocketNotifiers::handleEventDccNeedFileInfo(struct gg_event *e)
{
	Q_UNUSED(e)

	kdebugf();

	if (Version == Dcc6 && FileTransferHandler)
	{
		FileTransfer transfer = FileTransferHandler->transfer();
		gg_dcc_fill_file_info2(Socket, transfer.localFileName().toUtf8().constData(), qPrintable(transfer.localFileName()));
		watchFor(Socket);
	}
	else
		finished(false);
}

void DccSocketNotifiers::handleEventDccNeedFileAck(struct gg_event *e)
{
	Q_UNUSED(e)

	kdebugf();

	lock();
	Manager->needIncomingFileTransferAccept(this);
}

void DccSocketNotifiers::handleEventDccNeedVoiceAck(struct gg_event *e)
{
	Q_UNUSED(e)

	kdebugf();
}

void DccSocketNotifiers::handleEventDccVoiceData(struct gg_event *e)
{
	Q_UNUSED(e)

	kdebugf();
}

void DccSocketNotifiers::handleEventDcc7Accept(struct gg_event *e)
{
	Q_UNUSED(e)

	kdebugf();

	accepted();
}

void DccSocketNotifiers::handleEventDcc7Reject(struct gg_event *e)
{
	Q_UNUSED(e)

	kdebugf();

	rejected();
}

void DccSocketNotifiers::handleEventDcc7Connected(struct gg_event *e)
{
	Q_UNUSED(e)

	kdebugf();

	watchFor(Socket7); // socket may change
}

void DccSocketNotifiers::handleEventDcc7Error(struct gg_event *e)
{
	Q_UNUSED(e)

	kdebugf();

	finished(false);
}

void DccSocketNotifiers::handleEventDcc7Done(struct gg_event *e)
{
	Q_UNUSED(e)

	kdebugf();

	finished(true);
}

void DccSocketNotifiers::handleEventDcc7Pending(struct gg_event *e)
{
	Q_UNUSED(e)

	kdebugf();

	watchFor(Socket7); // socket may change
}

void DccSocketNotifiers::socketEvent()
{
	kdebugf();

	struct gg_event *e;

	switch (Version)
	{
		case Dcc6:
			e = gg_dcc_watch_fd(Socket);
			if (FileTransferHandler)
				FileTransferHandler->updateFileInfo();
			break;

		case Dcc7:
			e = gg_dcc7_watch_fd(Socket7);
			if (FileTransferHandler)
				FileTransferHandler->updateFileInfo();
			break;

		default:
			kdebugmf(KDEBUG_NETWORK | KDEBUG_INFO, "DCC version not set!\n");
			finished(false);
			return;
	}

	if (!e)
	{
		kdebugmf(KDEBUG_NETWORK | KDEBUG_INFO, "Connection broken unexpectedly!\n");
		finished(false);
		return;
	}

	kdebugmf(KDEBUG_NETWORK | KDEBUG_INFO, "dcc event: %d\n", e->type);

	switch (e->type)
	{
		case GG_EVENT_DCC_NEW:
			Manager->handleEventDccNew(e);
			break;

		case GG_EVENT_DCC_ERROR:
			handleEventDccError(e);
			break;

		case GG_EVENT_DCC_DONE:
			handleEventDccDone(e);
			break;

		case GG_EVENT_DCC_CLIENT_ACCEPT:
			handleEventDccClientAccept(e);
			break;

		case GG_EVENT_DCC_CALLBACK:
			handleEventDccCallback(e);
			break;

		case GG_EVENT_DCC_NEED_FILE_INFO:
			handleEventDccNeedFileInfo(e);
			break;

		case GG_EVENT_DCC_NEED_FILE_ACK:
			handleEventDccNeedFileAck(e);
			break;

		case GG_EVENT_DCC_NEED_VOICE_ACK:
			handleEventDccNeedVoiceAck(e);
			break;

		case GG_EVENT_DCC_VOICE_DATA:
			handleEventDccVoiceData(e);
			break;

		case GG_EVENT_DCC7_ACCEPT:
			handleEventDcc7Accept(e);
			break;

		case GG_EVENT_DCC7_CONNECTED:
			handleEventDcc7Connected(e);
			break;

		case GG_EVENT_DCC7_ERROR:
			handleEventDcc7Error(e);
			break;

		case GG_EVENT_DCC7_DONE:
			handleEventDcc7Done(e);
			break;

		case GG_EVENT_DCC7_PENDING:
			handleEventDcc7Pending(e);
			break;
	}

	if (e)
		gg_free_event(e);

	kdebugf2();
}

int DccSocketNotifiers::timeout()
{
	switch (Version)
	{
		case Dcc6:
			return Socket
				? Socket->timeout * 1000
				: 0;
		case Dcc7:
			return Socket7
				? Socket7->timeout * 1000
				: 0;
		case DccUnknown:
			return 0;
	}

	return 0;
}

bool DccSocketNotifiers::handleSoftTimeout()
{
	if (Dcc7 != Version)
		return false;

	if (!Socket7 || !Socket7->soft_timeout)
		return false;

	Socket7->timeout = 0;

	disable();
	socketEvent();
	enable();

	return true;
}

void DccSocketNotifiers::connectionTimeout()
{
	finished(false);
}

void DccSocketNotifiers::accepted()
{
	if (FileTransferHandler)
		FileTransferHandler->transfer().setTransferStatus(StatusTransfer);
	watchFor(Socket7);
}

void DccSocketNotifiers::rejected()
{
	if (FileTransferHandler)
		FileTransferHandler->transfer().setTransferStatus(StatusRejected);
	deleteLater();
}

void DccSocketNotifiers::finished(bool ok)
{
	if (Version == Dcc7)
		watchFor((struct gg_dcc7 *)0);
	else
		watchFor((struct gg_dcc *)0);
	deleteLater();
	emit done(ok);

	if (FileTransferHandler)
		FileTransferHandler->finished(ok);
}

UinType DccSocketNotifiers::peerUin()
{
	switch (Version)
	{
		case Dcc6:
			return Socket->peer_uin;
		case Dcc7:
			return Socket7->peer_uin;
		case DccUnknown:
			return 0;
	}

	return 0;
}

unsigned long DccSocketNotifiers::fileSize()
{
	switch (Version)
	{
		case Dcc6:
			return Socket->file_info.size;
		case Dcc7:
			return Socket7->size;
		case DccUnknown:
			return 0;
	}

	return 0;
}

unsigned long DccSocketNotifiers::transferredFileSize()
{
	switch (Version)
	{
		case Dcc6:
			return Socket->offset;
		case Dcc7:
			return Socket7->offset;
		case DccUnknown:
			return 0;
	}

	return 0;
}

QString DccSocketNotifiers::remoteFileName()
{
	switch (Version)
	{
		case Dcc6:
			return QString::fromUtf8((const char *)Socket->file_info.filename);
		case Dcc7:
			return QString::fromUtf8((const char *)Socket7->filename);
		case DccUnknown:
			return QString();
	}

	return QString();
}

void DccSocketNotifiers::setGaduFileTransferHandler(GaduFileTransferHandler *fileTransferHandler)
{
	FileTransferHandler = fileTransferHandler;
}

bool DccSocketNotifiers::acceptFileTransfer(const QFile &file)
{
	kdebugf();

	switch (Version)
	{
		case Dcc6:
			Socket->file_fd = dup(file.handle());
			Socket->offset = file.size();
			if (FileTransferHandler)
				FileTransferHandler->transfer().setTransferStatus(StatusTransfer);

			unlock();
			break;

		case Dcc7:
			Socket7->file_fd = dup(file.handle());
			Socket7->offset = file.size();
			gg_dcc7_accept(Socket7, Socket7->offset);
			if (FileTransferHandler)
				FileTransferHandler->transfer().setTransferStatus(StatusTransfer);
			watchFor(Socket7); // descriptor may be changed
			break;

		default:
			return false;
	}

	return true;
}

void DccSocketNotifiers::rejectFileTransfer()
{
	kdebugf();

	if (Dcc7 == Version)
		gg_dcc7_reject(Socket7, Socket7->offset);
	else
		finished(true);
}
