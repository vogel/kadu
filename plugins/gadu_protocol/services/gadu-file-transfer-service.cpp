/*
 * %kadu copyright begin%
 * Copyright 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2009 Bartłomiej Zimoń (uzi18@o2.pl)
 * Copyright 2004 Adrian Smarzewski (adrian@kadu.net)
 * Copyright 2007, 2008, 2009, 2010, 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2011 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2004, 2006 Marcin Ślusarz (joi@kadu.net)
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

#include "contacts/contact-manager.h"
#include "misc/misc.h"
#include "debug.h"

#include "dcc/dcc-socket-notifiers.h"
#include "file-transfer/gadu-file-transfer-handler.h"
#include "helpers/gadu-protocol-helper.h"
#include "gadu-account-details.h"
#include "gadu-contact-details.h"
#include "gadu-protocol.h"

#include "gadu-file-transfer-service.h"

GaduFileTransferService::GaduFileTransferService(GaduProtocol *protocol) :
		FileTransferService(protocol), Protocol(protocol)
{
}

GaduFileTransferService::~GaduFileTransferService()
{
}

FileTransferHandler * GaduFileTransferService::createFileTransferHandler(FileTransfer fileTransfer)
{
	GaduFileTransferHandler *handler = new GaduFileTransferHandler(fileTransfer);
	fileTransfer.setHandler(handler);

	return handler;
}

void GaduFileTransferService::connectSocketNotifiers(DccSocketNotifiers *notifiers)
{
	connect(notifiers, SIGNAL(destroyed(QObject *)), this, SLOT(socketNotifiersDestroyed(QObject *)));
}

void GaduFileTransferService::disconnectSocketNotifiers(DccSocketNotifiers *notifiers)
{
	disconnect(notifiers, 0, this, 0);
}

void GaduFileTransferService::socketNotifiersDestroyed(QObject *socketNotifiers)
{
	gg_dcc7 *key = SocketNotifiers.key(static_cast<DccSocketNotifiers *>(socketNotifiers));
	if (key)
		SocketNotifiers.remove(key);
}

bool GaduFileTransferService::connectionAcceptable(UinType uin, UinType peerUin)
{
	GaduAccountDetails *gaduAccountDetails = dynamic_cast<GaduAccountDetails *>(Protocol->account().details());
	if (!gaduAccountDetails)
		return false;

	Contact contact = ContactManager::instance()->byId(Protocol->account(), QString::number(peerUin), ActionReturnNull);
	Buddy buddy = contact.ownerBuddy();
	if (uin != gaduAccountDetails->uin() || buddy.isAnonymous())
	{
		kdebugm(KDEBUG_WARNING, "insane values: uin:%u peer_uin:%u\n", uin, peerUin);
		return false;
	}

	if (buddy.isBlocked())
	{
		kdebugm(KDEBUG_WARNING, "unbidden user: %u\n", peerUin);
		return false;
	}

	return true;
}

void GaduFileTransferService::needIncomingFileTransferAccept(DccSocketNotifiers *socket)
{
	Contact peer = ContactManager::instance()->byId(Protocol->account(), QString::number(socket->peerUin()), ActionCreateAndAdd);

	FileTransfer fileTransfer = FileTransfer::create();
	fileTransfer.setPeer(peer);
	fileTransfer.setTransferType(TypeReceive);
	fileTransfer.setRemoteFileName(socket->remoteFileName());
	fileTransfer.createHandler();

	GaduFileTransferHandler *handler = qobject_cast<GaduFileTransferHandler *>(fileTransfer.handler());
	if (handler)
		handler->setFileTransferNotifiers(socket);

	emit incomingFileTransfer(fileTransfer);
}

void GaduFileTransferService::handleEventDcc7New(struct gg_event *e)
{
	kdebugf();

	struct gg_dcc7 *dcc = e->event.dcc7_new;

	if (!connectionAcceptable(dcc->uin, dcc->peer_uin) || GG_DCC7_TYPE_FILE != dcc->dcc_type)
	{
		// No need to reenable socket notifiers as we close connection here.
		gg_dcc7_reject(dcc, 0);
		gg_dcc7_free(dcc);
		return;
	}

	DccSocketNotifiers *newSocketNotifiers = new DccSocketNotifiers(e->event.dcc7_new, this);
	SocketNotifiers.insert(e->event.dcc7_new, newSocketNotifiers);
	connectSocketNotifiers(newSocketNotifiers);
	newSocketNotifiers->start();

	needIncomingFileTransferAccept(newSocketNotifiers);
}

void GaduFileTransferService::handleEventDcc7Accept(struct gg_event *e)
{
	kdebugf();

	if (SocketNotifiers.contains(e->event.dcc7_accept.dcc7))
		SocketNotifiers.value(e->event.dcc7_accept.dcc7)->handleEventDcc7Accept(e);
}

void GaduFileTransferService::handleEventDcc7Reject(struct gg_event *e)
{
	kdebugf();

	if (SocketNotifiers.contains(e->event.dcc7_reject.dcc7))
		SocketNotifiers.value(e->event.dcc7_reject.dcc7)->handleEventDcc7Reject(e);
}

void GaduFileTransferService::handleEventDcc7Pending(struct gg_event *e)
{
	kdebugf();

	if (SocketNotifiers.contains(e->event.dcc7_pending.dcc7))
		SocketNotifiers.value(e->event.dcc7_pending.dcc7)->handleEventDcc7Pending(e);
}

void GaduFileTransferService::handleEventDcc7Error(struct gg_event *e)
{
	Q_UNUSED(e)

	kdebugf();

	// TODO: write it
}

void GaduFileTransferService::attachSendFileTransferSocket(GaduFileTransferHandler *handler)
{
	Contact contact = handler->transfer().peer();
	if (contact.isNull())
		return;

	GaduContactDetails *details = GaduProtocolHelper::gaduContactDetails(contact);
	if (!details)
		return;

	gg_dcc7 *dcc = gg_dcc7_send_file(Protocol->gaduSession(), details->uin(),
			handler->transfer().localFileName().toUtf8().constData(), 0, 0);

	if (dcc)
	{
		DccSocketNotifiers *fileTransferNotifiers = new DccSocketNotifiers(dcc, this);
		handler->transfer().setTransferStatus(StatusWaitingForAccept);
		handler->setFileTransferNotifiers(fileTransferNotifiers);
		fileTransferNotifiers->start();

		SocketNotifiers.insert(dcc, fileTransferNotifiers);
	}
	else
		handler->socketNotAvailable();
}
