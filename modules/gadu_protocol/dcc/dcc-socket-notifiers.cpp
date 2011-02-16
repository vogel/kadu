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

DccSocketNotifiers::DccSocketNotifiers(struct gg_dcc7 *socket, QObject *parent) :
		GaduSocketNotifiers(parent), FileTransferHandler(0), Socket7(socket)
{
}

DccSocketNotifiers::~DccSocketNotifiers()
{
	gg_dcc7_free(Socket7);
	Socket7 = 0;
}

void DccSocketNotifiers::start()
{
	watchFor();
}

void DccSocketNotifiers::watchFor()
{
	if (-1 != Socket7->fd)
		GaduSocketNotifiers::watchFor(Socket7->fd);
	else
		GaduSocketNotifiers::watchFor(0);
}

bool DccSocketNotifiers::checkRead()
{
	return Socket7->check & GG_CHECK_READ;
}

bool DccSocketNotifiers::checkWrite()
{
	return Socket7->check & GG_CHECK_WRITE;
}

void DccSocketNotifiers::handleEventDcc7Accept(struct gg_event *e)
{
	Q_UNUSED(e)

	if (FileTransferHandler)
		FileTransferHandler->transfer().setTransferStatus(StatusTransfer);
	watchFor();
}

void DccSocketNotifiers::handleEventDcc7Reject(struct gg_event *e)
{
	Q_UNUSED(e)

	if (FileTransferHandler)
		FileTransferHandler->transfer().setTransferStatus(StatusRejected);
	deleteLater();
}

void DccSocketNotifiers::handleEventDcc7Connected(struct gg_event *e)
{
	Q_UNUSED(e)

	kdebugf();

	watchFor(); // socket may change
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

	watchFor(); // socket may change
}

void DccSocketNotifiers::socketEvent()
{
	kdebugf();

	struct gg_event *e = gg_dcc7_watch_fd(Socket7);
	if (FileTransferHandler)
		FileTransferHandler->updateFileInfo();

	if (!e)
	{
		kdebugmf(KDEBUG_NETWORK | KDEBUG_INFO, "Connection broken unexpectedly!\n");
		finished(false);
		return;
	}

	kdebugmf(KDEBUG_NETWORK | KDEBUG_INFO, "dcc event: %d\n", e->type);

	switch (e->type)
	{
		// DCC6 is not supported anymore
		case GG_EVENT_DCC_NEW:
		case GG_EVENT_DCC_ERROR:
		case GG_EVENT_DCC_DONE:
		case GG_EVENT_DCC_CLIENT_ACCEPT:
		case GG_EVENT_DCC_CALLBACK:
		case GG_EVENT_DCC_NEED_FILE_INFO:
		case GG_EVENT_DCC_NEED_FILE_ACK:
		case GG_EVENT_DCC_NEED_VOICE_ACK:
		case GG_EVENT_DCC_VOICE_DATA:
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
	return Socket7
			? Socket7->timeout * 1000
			: 0;
}

bool DccSocketNotifiers::handleSoftTimeout()
{
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

void DccSocketNotifiers::finished(bool ok)
{
	GaduSocketNotifiers::watchFor(0);
	deleteLater();
	emit done(ok);

	if (FileTransferHandler)
		FileTransferHandler->finished(ok);
}

UinType DccSocketNotifiers::peerUin()
{
	return Socket7
			? Socket7->peer_uin
			: 0;
}

unsigned long DccSocketNotifiers::fileSize()
{
	return Socket7
			? Socket7->size
			: 0;
}

unsigned long DccSocketNotifiers::transferredFileSize()
{
	return Socket7
			? Socket7->offset
			: 0;
}

QString DccSocketNotifiers::remoteFileName()
{
	return Socket7
			? QString::fromUtf8((const char *)Socket7->filename)
			: QString();
}

void DccSocketNotifiers::setGaduFileTransferHandler(GaduFileTransferHandler *fileTransferHandler)
{
	FileTransferHandler = fileTransferHandler;
}

bool DccSocketNotifiers::acceptFileTransfer(const QFile &file)
{
	kdebugf();

	Socket7->file_fd = dup(file.handle());
	Socket7->offset = file.size();
	gg_dcc7_accept(Socket7, Socket7->offset);
	if (FileTransferHandler)
		FileTransferHandler->transfer().setTransferStatus(StatusTransfer);
	watchFor(); // descriptor may be changed

	return true;
}

void DccSocketNotifiers::rejectFileTransfer()
{
	kdebugf();

	gg_dcc7_reject(Socket7, GG_DCC7_REJECT_USER);
}
