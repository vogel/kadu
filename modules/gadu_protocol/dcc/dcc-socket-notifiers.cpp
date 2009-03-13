/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <QtCore/QFile>

#include "debug.h"
#include "misc.h"

#include "file-transfer/gadu-file-transfer.h"
#include "socket-notifiers/gadu-protocol-socket-notifiers.h"

#include "dcc-socket-notifiers.h"

void DccSocketNotifiers::watchFor(struct gg_dcc *socket)
{
	kdebugmf(KDEBUG_NETWORK | KDEBUG_INFO, "%p\n", socket);

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
}

void DccSocketNotifiers::watchFor(struct gg_dcc7 *socket)
{
	kdebugmf(KDEBUG_NETWORK | KDEBUG_INFO, "%p\n", socket);

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
	kdebugmf(KDEBUG_NETWORK|KDEBUG_INFO, "%d\n", e->event.dcc_error);

	finished(false);
}

void DccSocketNotifiers::handleEventDccDone(struct gg_event *e)
{
	kdebugf();

	finished(true);
}

void DccSocketNotifiers::handleEventDccClientAccept(struct gg_event *e)
{
	kdebugmf(KDEBUG_NETWORK|KDEBUG_INFO, "uin:%d peer_uin:%d\n", Socket->uin, Socket->peer_uin);

	// TODO: make async TODO: 0.6.6
	if (!Manager->acceptConnection(Socket->uin, Socket->peer_uin, Socket->remote_addr))
	{
		done(false);
		return;
	}
}

void DccSocketNotifiers::handleEventDccCallback(struct gg_event *e)
{
	kdebugmf(KDEBUG_NETWORK|KDEBUG_INFO, "uin:%d peer_uin:%d\n", Socket->uin, Socket->peer_uin);

	GaduFileTransfer *gft = Manager->findFileTransfer(this);
	if (gft)
	{
		gg_dcc_set_type(Socket, GG_SESSION_DCC_SEND); // TODO: 0.6.6 for voice it wont look that way
		gft->setFileTransferNotifiers(this);
		return;
	}
	else
		finished(false);
}

void DccSocketNotifiers::handleEventDccNeedFileInfo(struct gg_event *e)
{
	kdebugf();

	if (Version == Dcc6 && FileTransfer)
	{
		gg_dcc_fill_file_info2(Socket, unicode2cp(FileTransfer->localFileName()), qPrintable(FileTransfer->localFileName()));
		watchFor(Socket);
	}
	else
		finished(false);
}

void DccSocketNotifiers::handleEventDccNeedFileAck(struct gg_event *e)
{
	kdebugf();

	lock();
	Manager->needIncomingFileTransferAccept(this);
}

void DccSocketNotifiers::handleEventDccNeedVoiceAck(struct gg_event *e)
{
	kdebugf();
}

void DccSocketNotifiers::handleEventDccVoiceData(struct gg_event *e)
{
	kdebugf();
}

void DccSocketNotifiers::handleEventDcc7Accept(struct gg_event *e)
{
	kdebugf();

	accepted();
}

void DccSocketNotifiers::handleEventDcc7Reject(struct gg_event *e)
{
	kdebugf();

	rejected();
}

void DccSocketNotifiers::handleEventDcc7Connected(struct gg_event *e)
{
	kdebugf();

	watchFor(Socket7); // socket may change
}

void DccSocketNotifiers::handleEventDcc7Error(struct gg_event *e)
{
	kdebugf();

	finished(false);
}

void DccSocketNotifiers::handleEventDcc7Done(struct gg_event *e)
{
	kdebugf();

	finished(true);
}

void DccSocketNotifiers::handleEventDcc7Pending(struct gg_event *e)
{
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
			if (FileTransfer)
				FileTransfer->updateFileInfo();
			break;

		case Dcc7:
			e = gg_dcc7_watch_fd(Socket7);
			if (FileTransfer)
				FileTransfer->updateFileInfo();
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
				? Socket->timeout
				: 0;
		case Dcc7:
			return Socket7
				? Socket7->timeout
				: 0;
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
	if (FileTransfer)
		FileTransfer->changeFileTransferStatus(FileTransfer::StatusTransfer);
	watchFor(Socket7);
}

void DccSocketNotifiers::rejected()
{
	if (FileTransfer)
		FileTransfer->changeFileTransferStatus(FileTransfer::StatusRejected);
	deleteLater();
}

void DccSocketNotifiers::finished(bool ok)
{
	watchFor((struct gg_dcc *)0);
	deleteLater();
	emit done(ok);

	if (FileTransfer)
		FileTransfer->finished(ok);
}

UinType DccSocketNotifiers::peerUin()
{
	switch (Version)
	{
		case Dcc6:
			return Socket->peer_uin;
		case Dcc7:
			return Socket7->peer_uin;
	}

	return 0;
}

unsigned long DccSocketNotifiers::fileSize()
{
	switch (Version)
	{
		case Dcc6:
			return gg_fix32(Socket->file_info.size);
		case Dcc7:
			return gg_fix32( Socket7->size);
	}

	return 0;
}

unsigned long DccSocketNotifiers::transferredFileSize()
{
	switch (Version)
	{
		case Dcc6:
			return gg_fix32(Socket->offset);
		case Dcc7:
			return gg_fix32(Socket7->offset);
	}

	return 0;
}

QString DccSocketNotifiers::remoteFileName()
{
	switch (Version)
	{
		case Dcc6:
			return cp2unicode(QByteArray((const char *)Socket->file_info.filename));
		case Dcc7:
			return cp2unicode(QByteArray((const char *)Socket7->filename));
	}

	return QString::null;
}

void DccSocketNotifiers::setGaduFileTransfer(GaduFileTransfer *fileTransfer)
{
	FileTransfer = fileTransfer;
}

bool DccSocketNotifiers::acceptFileTransfer(const QFile &file)
{
	kdebugf();

	switch (Version)
	{
		case Dcc6:
			Socket->file_fd = dup(file.handle());
			Socket->offset = file.size();

			unlock();
			break;

		case Dcc7:
			Socket7->file_fd = dup(file.handle());
			Socket7->offset = file.size();
			gg_dcc7_accept(Socket7, Socket7->offset);
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
