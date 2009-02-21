/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <QtCore/QSocketNotifier>

#include "accounts/account.h"
#include "accounts/account_manager.h"
#include "config_file.h"
#include "dcc-manager.h"
#include "debug.h"
#include "ignore.h"
#include "message_box.h"
#include "misc.h"

#include "dcc-socket.h"

DccSocket::DccSocket(DccManager *manager, struct gg_dcc *dccStruct) :
		Version(Dcc6), Dcc6Struct(dccStruct), Dcc7Struct(0),
		DccCheckField(dccStruct->check), DccEvent(0), destroying(false), 
		ReadSocketNotifier(0), WriteSocketNotifier(0), ConnectionClosed(false),  Timeout(0), Handler(0) 
{
	Manager = manager;
}

DccSocket::DccSocket(DccManager *manager, struct gg_dcc7 *dccStruct) :
	Version(Dcc7), Dcc6Struct(0), Dcc7Struct(dccStruct),
	DccCheckField(dccStruct->check), DccEvent(0), destroying(false),
	ReadSocketNotifier(0), WriteSocketNotifier(0), ConnectionClosed(false), Timeout(0), Handler(0)
{
	Manager = manager;
}

DccSocket::~DccSocket()
{
	cancelTimeout();
	if (Timeout)
	{
		delete Timeout;
		Timeout = 0;
	}

	if (Dcc6Struct)
		gg_dcc_free(Dcc6Struct);

	if (Dcc7Struct)
		gg_dcc7_free(Dcc7Struct);
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

void DccSocket::startTimeout()
{
	kdebugf();

	switch (Version)
	{
		case Dcc6:
			if (Dcc6Struct->timeout <= 0)
				return;
			break;
		case Dcc7:
			if (Dcc7Struct->timeout <= 0)
				return;
			break;
		default:
			break;
	}

	if (!Timeout)
	{
		Timeout = new QTimer(this);
		connect(Timeout, SIGNAL(timeout()), this, SLOT(timeout()));
	}

	switch (Version)
	{
		case Dcc6:
			Timeout->start(Dcc6Struct->timeout * 1000, true);
			break;
		case Dcc7:
			Timeout->start(Dcc7Struct->timeout * 1000, true);
			break;
		default:
			return;
	}
}

void DccSocket::cancelTimeout()
{
	kdebugf();

	if (Timeout)
		Timeout->stop();
}

void DccSocket::timeout()
{
	kdebugf();
/*
	switch (Version)
	{
		case Dcc6:
			closeSocket(true);
			break;
		case Dcc7:
			if (Dcc7Struct->soft_timeout)
				watchDcc();
			else
				closeSocket(true);
			break;
		default:
			return;
	}*/
}

void DccSocket::enableNotifiers()
{
}

void DccSocket::disableNotifiers()
{
}

void DccSocket::socketDataEvent()
{
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

	kdebugf2();
}

QString DccSocket::fileName()
{
	switch (Version)
	{
		case Dcc6:
			return cp2unicode(QByteArray((const char *)Dcc6Struct->file_info.filename));
		case Dcc7:
			return cp2unicode(QByteArray((const char *)Dcc7Struct->filename));
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
			gg_dcc_fill_file_info2(Dcc6Struct, unicode2cp(fileName), qPrintable(fileName));
			return;
		default:
			return;
	}
}

bool DccSocket::setFile(int fd)
{
	kdebugf();

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
