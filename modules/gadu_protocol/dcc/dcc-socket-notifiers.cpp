/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "debug.h"

#include "socket-notifiers/gadu-protocol-socket-notifiers.h"

#include "connection-acceptor.h"
#include "dcc-socket-notifiers.h"

void DccSocketNotifiers::watchFor(struct gg_dcc *socket)
{
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
			return;
	}

	if (!e)
	{
		kdebugmf(KDEBUG_NETWORK|KDEBUG_INFO, "Connection broken unexpectedly!\n");
		done(false);
		return;
	}

	handleEvent(e);

	if (e)
		gg_free_event(e);

	kdebugf2();
}

void DccSocketNotifiers::finished(bool ok)
{
	watchFor((struct gg_dcc *)0);
	deleteLater();
	emit done(ok);
}

void DccSocketNotifiers::handleEvent(struct gg_event *e)
{
	switch (e->type)
	{
		case GG_EVENT_DCC7_CONNECTED:
			// Dcc7Struct->fd changed, we need new socket notifiers
			watchFor(Socket7);
			break;

		case GG_EVENT_DCC7_ERROR:
		case GG_EVENT_DCC_ERROR:
			kdebugmf(KDEBUG_NETWORK|KDEBUG_INFO, "GG_EVENT_DCC_ERROR\n");
			finished(false);
			return;

		// only in version 6
		case GG_EVENT_DCC_CLIENT_ACCEPT:
			kdebugmf(KDEBUG_NETWORK|KDEBUG_INFO, "GG_EVENT_DCC_CLIENT_ACCEPT! uin:%d peer_uin:%d\n",
				Socket->uin, Socket->peer_uin);

			// TODO: make async
			if (!Acceptor || !Acceptor->acceptConnection(Socket->uin, Socket->peer_uin, Socket->remote_addr))
			{
				done(false);
				return;
			}
			break;

		// only in version 6
		case GG_EVENT_DCC_CALLBACK:
			kdebugmf(KDEBUG_NETWORK|KDEBUG_INFO, "GG_EVENT_DCC_CALLBACK! uin:%d peer_uin:%d\n",
				Socket->uin, Socket->peer_uin);
			gg_dcc_set_type(Socket, GG_SESSION_DCC_SEND); // for voice it wont look that way
			emit callbackReceived(this);
			break;

		case GG_EVENT_DCC7_DONE:
		case GG_EVENT_DCC_DONE:
			kdebugmf(KDEBUG_NETWORK|KDEBUG_INFO, "GG_EVENT_DCC_DONE\n");
			finished(true);
			break;

		case GG_EVENT_DCC_NEED_FILE_INFO:
// 			Socket->fillFileInfo(FileName); // TODO: 0.6.6 implement
			break;

		case GG_EVENT_DCC_NEED_VOICE_ACK:
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
			break;

		case GG_EVENT_DCC_ACK:
// 			kdebugmf(KDEBUG_INFO, "GG_EVENT_DCC_ACK\n");
// 			if (socket->type() == GG_SESSION_DCC_VOICE)
// 			{
// 				VoiceChatDialog *voiceChatDialog = new VoiceChatDialog();
// 				socket->setHandler(voiceChatDialog);
// 			}
			break;

		case GG_EVENT_DCC_NEED_FILE_ACK:
// 			kdebugmf(KDEBUG_NETWORK|KDEBUG_INFO, "GG_EVENT_DCC_NEED_FILE_ACK! uin:%d peer_uin:%d\n",
// 				socket->uin(), socket->peerUin());
//			lock();
// 			needFileAccept(socket);
// 			lock = true;
			done(true); // TODO: 0.6.6, implement
			break;

		case GG_EVENT_DCC_NEW:
		{
			kdebugmf(KDEBUG_NETWORK | KDEBUG_INFO, "GG_EVENT_DCC_NEW\n");
			emit incomingConnection(e->event.dcc_new);
			break;
		}

		default:
			break;
	}
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
