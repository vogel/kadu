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
		done(false);
}

void DccSocketNotifiers::handleEventDccNeedFileInfo(struct gg_event *e)
{
	kdebugf();

	if (Version == Dcc6 && FileTransfer)
		gg_dcc_fill_file_info2(Socket, unicode2cp(FileTransfer->localFileName()), qPrintable(FileTransfer->localFileName()));
	else
		finished(false);
}

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

void DccSocketNotifiers::handleEventDccNeedFileAck(struct gg_event *e)
{
	kdebugf();

// TODO: 0.6.6
	lock();
	Manager->needIncomingFileTransferAccept(this);
/*
	int handle = creat("/home/vogel/gg-download", 0640);

	printf("handle: %d\n", handle);
	printf("version: %d\n", Version);

	switch (Version)
	{
		case Dcc6:
			Socket->file_fd = handle;
			Socket->offset = 0;
			break;
		case Dcc7:
			Socket7->file_fd = handle;
			Socket7->offset = 0;
			break;
	}
*/
}

void DccSocketNotifiers::handleEventDccNeedVoiceAck(struct gg_event *e)
{
	kdebugf();
}

void DccSocketNotifiers::handleEventDccVoiceData(struct gg_event *e)
{
	kdebugf();
}

void DccSocketNotifiers::socketEvent()
{
	kdebugf();

	struct gg_event *e;

	switch (Version)
	{
		case Dcc6:
			e = gg_dcc_watch_fd(Socket);
			break;
		case Dcc7:
			e = gg_dcc7_watch_fd(Socket7);
			break;
		default:
			kdebugmf(KDEBUG_NETWORK|KDEBUG_INFO, "DCC version not set!\n");
			done(false);
			return;
	}

	if (!e)
	{
		kdebugmf(KDEBUG_NETWORK|KDEBUG_INFO, "Connection broken unexpectedly!\n");
		done(false);
		return;
	}

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

void DccSocketNotifiers::finished(bool ok)
{
	watchFor((struct gg_dcc *)0);
	deleteLater();
	emit done(ok);
}

// 		case GG_EVENT_DCC7_CONNECTED:
			// Dcc7Struct->fd changed, we need new socket notifiers
// 			printf("dcc7 connected\n");
// 			watchFor(Socket7);
// 			break;

// 		case GG_EVENT_DCC7_ERROR:
// 			kdebugmf(KDEBUG_NETWORK|KDEBUG_INFO, "GG_EVENT_DCC_ERROR\n");
// 			finished(false);
// 			return;

// 		case GG_EVENT_DCC7_DONE:
// 			printf("event done\n");
// 			kdebugmf(KDEBUG_NETWORK|KDEBUG_INFO, "GG_EVENT_DCC_DONE\n");
// 			finished(true);
// 			break;

// 		case GG_EVENT_DCC_NEED_VOICE_ACK:
// 			kdebugmf(KDEBUG_NETWORK|KDEBUG_INFO, "GG_EVENT_DCC_NEED_VOICE_ACK! uin:%d peer_uin:%d\n",
// 				socket->uin(), socket->peerUin());
// 			if (askAcceptVoiceChat(socket))
// 			{
// 				VoiceChatDialog *voiceChatDialog = new VoiceChatDialog();
// 				socket->setHandler(voiceChatDialog);
// 			}
// 			else
// 			{
// 				socket->reject();
// 				delete socket;
// 			}
// 			break;

// 		case GG_EVENT_DCC_ACK:
// 			kdebugmf(KDEBUG_INFO, "GG_EVENT_DCC_ACK\n");
// 			if (socket->type() == GG_SESSION_DCC_VOICE)
// 			{
// 				VoiceChatDialog *voiceChatDialog = new VoiceChatDialog();
// 				socket->setHandler(voiceChatDialog);
// 			}
// 			break;

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
			return Socket->file_info.size;
		case Dcc7:
			return Socket7->size;
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
			gg_dcc7_accept(Socket7, Socket7->offset);
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
