/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <qsocketnotifier.h>

#include <arpa/inet.h> 

#include "config_file.h"
#include "dcc.h"
#include "debug.h"
#include "ignore.h"
#include "message_box.h"
#include "misc.h"

#include "dcc_socket.h"

DccSocket::DccSocket(struct gg_dcc *dccStruct)
	: Version(Dcc6), Dcc6Struct(dccStruct), Dcc7Struct(0), DccCheckField(dccStruct->check), DccEvent(0), ReadSocketNotifier(0), WriteSocketNotifier(0), ConnectionClosed(false), Handler(0)
{
}

DccSocket::DccSocket(struct gg_dcc7 *dccStruct)
	: Version(Dcc7), Dcc6Struct(0), Dcc7Struct(dccStruct), DccCheckField(dccStruct->check), DccEvent(0), ReadSocketNotifier(0), WriteSocketNotifier(0), ConnectionClosed(false), Handler(0)
{
}

DccSocket::~DccSocket()
{
	closeSocket(true);
	finalizeNotifiers();
	setHandler(0);

	if (Dcc6Struct)
		gg_dcc_free(Dcc6Struct);

	if (Dcc7Struct)
		gg_dcc7_free(Dcc7Struct);
}

void DccSocket::setHandler(DccHandler *handler)
{
	kdebugf();

	if (Handler)
		Handler->removeSocket(this);

	Handler = handler;

	if (Handler)
	{
		Handler->addSocket(this);
		initializeNotifiers();
	}
}

struct gg_event * DccSocket::ggDccEvent() const
{
	return DccEvent;
}

UinType DccSocket::uin()
{
	switch (Version)
	{
		case Dcc6:
			return Dcc6Struct->uin;
		case Dcc7:
			return Dcc7Struct->uin;
		default:
			return -1;
	}
}

UinType DccSocket::peerUin()
{
	switch (Version)
	{
		case Dcc6:
			return Dcc6Struct->peer_uin;
		case Dcc7:
			return Dcc7Struct->peer_uin;
		default:
			return -1;
	}
}

void DccSocket::setType(int type)
{
	if (Version == Dcc6)
		gg_dcc_set_type(Dcc6Struct, type);
}

int DccSocket::type()
{
	switch (Version)
	{
		case Dcc6:
			return Dcc6Struct->type;
		case Dcc7:
			return Dcc7Struct->type;
		default:
			return -1;
	}
}

void DccSocket::initializeNotifiers()
{
	kdebugf();

	int socketFd;
	switch (Version)
	{
		case Dcc6:
			socketFd = Dcc6Struct->fd;
			break;
		case Dcc7:
			socketFd = Dcc7Struct->fd;
			if (socketFd == -1) // wait for accept/reject
			{
				connect(gadu, SIGNAL(dcc7Accepted(struct gg_dcc7 *)), this, SLOT(dcc7Accepted(struct gg_dcc7 *)));
				connect(gadu, SIGNAL(dcc7Rejected(struct gg_dcc7 *)), this, SLOT(dcc7Rejected(struct gg_dcc7 *)));
				return;
			}
			break;
		default:
			return;
	}

	ReadSocketNotifier = new QSocketNotifier(socketFd, QSocketNotifier::Read, this);
	QObject::connect(ReadSocketNotifier, SIGNAL(activated(int)), this, SLOT(socketDataEvent()));
	if (!checkRead())
		ReadSocketNotifier->setEnabled(false);

	WriteSocketNotifier = new QSocketNotifier(socketFd, QSocketNotifier::Write, this);
	QObject::connect(WriteSocketNotifier, SIGNAL(activated(int)), this, SLOT(socketDataEvent()));
	if (!checkWrite())
		WriteSocketNotifier->setEnabled(false);

	kdebugf2();
}

void DccSocket::finalizeNotifiers()
{
	kdebugf();

	if (ReadSocketNotifier)
	{
		delete ReadSocketNotifier;
		ReadSocketNotifier = 0;
	}

	if (WriteSocketNotifier)
	{
		delete WriteSocketNotifier;
		WriteSocketNotifier = 0;
	}

	kdebugf2();
}

void DccSocket::enableNotifiers()
{
	kdebugf();

	if (checkRead())
		ReadSocketNotifier->setEnabled(true);

	if (checkWrite())
		WriteSocketNotifier->setEnabled(true);
}

void DccSocket::disableNotifiers()
{
	ReadSocketNotifier->setEnabled(false);
	WriteSocketNotifier->setEnabled(false);
}

bool DccSocket::checkRead()
{
	return DccCheckField & GG_CHECK_READ;
}

bool DccSocket::checkWrite()
{
	return DccCheckField & GG_CHECK_WRITE;
}

void DccSocket::socketDataEvent()
{
	disableNotifiers();
	watchDcc();
}

void DccSocket::stop()
{
	closeSocket(false);
}

void DccSocket::connectionError()
{
	closeSocket(true);
}

void DccSocket::closeSocket(bool error)
{
	kdebugf();

	if (ConnectionClosed)
		return;
	ConnectionClosed = true;

	disableNotifiers();
	deleteLater();

	if (Handler)
	{
		if (error)
			Handler->connectionError(this);
		else
			Handler->connectionDone(this);

		Handler->removeSocket(this);
	}

	kdebugf2();
}

void DccSocket::watchDcc()
{
	kdebugf();

	switch (Version)
	{
		case Dcc6:
			DccEvent = gg_dcc_watch_fd(Dcc6Struct);
			break;
		case Dcc7:
			DccEvent = gg_dcc7_watch_fd(Dcc7Struct);
			break;
		default:
			return;
	}

	if (!DccEvent)
	{
		kdebugmf(KDEBUG_NETWORK|KDEBUG_INFO, "Connection broken unexpectedly!\n");
		connectionError();
		return;
	}

	switch (DccEvent->type)
	{
		case GG_EVENT_DCC7_CONNECTED:
			// Dcc7Struct->fd is changed, we need new socket notifiers
			finalizeNotifiers();
			initializeNotifiers();
			break;

		case GG_EVENT_DCC7_ERROR:
		case GG_EVENT_DCC_ERROR:
			kdebugmf(KDEBUG_NETWORK|KDEBUG_INFO, "GG_EVENT_DCC_ERROR\n");
			connectionError();
			break;

		// only in version 6
		case GG_EVENT_DCC_CLIENT_ACCEPT:
			kdebugmf(KDEBUG_NETWORK|KDEBUG_INFO, "GG_EVENT_DCC_CLIENT_ACCEPT! uin:%d peer_uin:%d\n",
				Dcc6Struct->uin, Dcc6Struct->peer_uin);

			if (!dcc_manager->acceptClient(Dcc6Struct->uin, Dcc6Struct->peer_uin, Dcc6Struct->remote_addr))
				connectionError();
			break;

		// only in version 6
		case GG_EVENT_DCC_CALLBACK:
			kdebugmf(KDEBUG_NETWORK|KDEBUG_INFO, "GG_EVENT_DCC_CALLBACK! uin:%d peer_uin:%d\n",
				Dcc6Struct->uin, Dcc6Struct->peer_uin);
			gg_dcc_set_type(Dcc6Struct, GG_SESSION_DCC_SEND);
			dcc_manager->callbackReceived(this);
			break;

		case GG_EVENT_DCC7_DONE:
		case GG_EVENT_DCC_DONE:
			kdebugmf(KDEBUG_NETWORK|KDEBUG_INFO, "GG_EVENT_DCC_DONE\n");
			closeSocket(false);
			break;

		default:
			break;
	}

	if (!connectionClosed())
	{
		bool lock = false;

		if (Handler)
			Handler->socketEvent(this, lock);

		if (!lock)
			enableNotifiers();
	}

	if (DccEvent)
	{
		gg_free_event(DccEvent);
		DccEvent = 0;
	}

	kdebugf2();
}

unsigned char * DccSocket::fileName()
{
	switch (Version)
	{
		case Dcc6:
			return Dcc6Struct->file_info.filename;
		case Dcc7:
			return Dcc7Struct->filename;
		default:
			return 0;
	}
}

int DccSocket::fileSize()
{
	switch (Version)
	{
		case Dcc6:
			return Dcc6Struct->file_info.size;
		case Dcc7:
			return Dcc7Struct->size;
		default:
			return -1;
	}
}

int DccSocket::fileOffset()
{
	switch (Version)
	{
		case Dcc6:
			return Dcc6Struct->offset;
		case Dcc7:
			return Dcc7Struct->offset;
		default:
			return -1;
	}
}

void DccSocket::fillFileInfo(const QString &fileName)
{
	switch (Version)
	{
		case Dcc6:
			gg_dcc_fill_file_info2(Dcc6Struct, unicode2cp(fileName), fileName.local8Bit().data());
			return;
		default:
			return;
	}
}

bool DccSocket::setFile(int fd)
{
	if (fd == -1)
		return false;

	switch (Version)
	{
		case Dcc6:
			Dcc6Struct->file_fd = fd;
			return true;
		case Dcc7:
			Dcc7Struct->file_fd = fd;
			return true;
		default:
			return false;
	}
}

void DccSocket::setOffset(long offset)
{
	switch (Version)
	{
		case Dcc6:
			Dcc6Struct->offset = offset;
			break;
		case Dcc7:
			Dcc7Struct->offset = offset;
			break;
		default:
			break;
	}
}

void DccSocket::dcc7Accepted(struct gg_dcc7 *dcc7)
{
	if (dcc7 != Dcc7Struct)
		return;

	kdebugf();

	disconnect(gadu, SIGNAL(dcc7Accepted(struct gg_dcc7 *)), this, SLOT(dcc7Accepted(struct gg_dcc7 *)));
	disconnect(gadu, SIGNAL(dcc7Rejected(struct gg_dcc7 *)), this, SLOT(dcc7Rejected(struct gg_dcc7 *)));

	if (Handler)
		Handler->connectionAccepted(this);

	initializeNotifiers();
}

void DccSocket::dcc7Rejected(struct gg_dcc7 *dcc7)
{
	if (dcc7 != Dcc7Struct)
		return;

	kdebugf();

	disconnect(gadu, SIGNAL(dcc7Accepted(struct gg_dcc7 *)), this, SLOT(dcc7Accepted(struct gg_dcc7 *)));
	disconnect(gadu, SIGNAL(dcc7Rejected(struct gg_dcc7 *)), this, SLOT(dcc7Rejected(struct gg_dcc7 *)));

	if (Handler)
		Handler->connectionRejected(this);
}

void DccSocket::accept()
{
	kdebugf();

	switch (Version)
	{
		case Dcc7:
			// TODO: move it to libgadu
			gg_dcc7_accept(Dcc7Struct, Dcc7Struct->offset);
			break;
		default:
			break;
	}
}

void DccSocket::reject()
{
	kdebugf();

	switch (Version)
	{
		case Dcc7:
			gg_dcc7_reject(Dcc7Struct, GG_DCC7_REJECT_USER);
			break;
		default:
			break;
	}

	stop();
}

void DccSocket::sendVoiceData(char *data, int length)
{
	kdebugf();

	switch (Version)
	{
		case Dcc6:
			gg_dcc_voice_send(Dcc6Struct, data, length);
			break;
		default:
			break;
	}
}
