/*
 * %kadu copyright begin%
 * Copyright 2007 Dawid Stawiarski (neeo@kadu.net)
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2004, 2005, 2006 Marcin Ślusarz (joi@kadu.net)
 * Copyright 2004 Adrian Smarzewski (adrian@kadu.net)
 * Copyright 2007, 2008, 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2008, 2009 Michał Podsiadlik (michal@kadu.net)
 * Copyright 2005 Roman Krzystyniak (Ron_K@tlen.pl)
 * Copyright 2009 Bartłomiej Zimoń (uzi18@o2.pl)
 * Copyright 2008, 2010 Piotr Galiszewski (piotrgaliszewski@gmail.com)
 * Copyright 2005 Paweł Płuciennik (pawel_p@kadu.net)
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

#include <QtCore/QtGlobal>

#ifdef Q_OS_WIN
#include <winsock2.h>
#else
#include <arpa/inet.h>
#endif

#include "accounts/account.h"
#include "buddies/buddy.h"
#include "buddies/buddy-manager.h"
#include "configuration/configuration-file.h"
#include "contacts/contact.h"
#include "contacts/contact-manager.h"
#include "file-transfer/file-transfer-manager.h"
#include "gui/windows/message-dialog.h"

#include "debug.h"

#include "dcc/dcc-manager.h"
#include "dcc/dcc-socket-notifiers.h"
#include "file-transfer/gadu-file-transfer-handler.h"
#include "services/gadu-file-transfer-service.h"
#include "socket-notifiers/gadu-protocol-socket-notifiers.h"
#include "gadu-account-details.h"
#include "gadu-contact-details.h"

#include "gadu-protocol.h"


DccManager::DccManager(GaduProtocol *protocol) :
		QObject(protocol), Protocol(protocol)
{
	kdebugf();

	setUpDcc();

	kdebugf2();
}

DccManager::~DccManager()
{
	kdebugf();

	closeDcc();

	kdebugf2();
}

void DccManager::setUpDcc()
{
	kdebugf();

	WaitingFileTransfers.clear();
	DccEnabled = true;

	kdebugf2();
}

void DccManager::configurationUpdated()
{
// 		GaduAccount *account = qobject_cast<GaduAccount *>(Protocol->account());
// 		if (!account)
// 				return;
// 		account->loadConfiguration(xml_config_file);



	// kadu->reconnect() ??
}

bool DccManager::dccEnabled() const
{
	return DccEnabled;
}

// void DccManager::timeout()
// {
	// TODO: change into notification
	// 	MessageDialog::msg(tr("Direct connection timeout!\nThe receiver doesn't support direct connections or\nboth machines are behind routers with NAT."), true, "dialog-warning");
// }

void DccManager::closeDcc()
{
	kdebugf();

	DccEnabled = false;

	kdebugf2();
}

void DccManager::connectSocketNotifiers(DccSocketNotifiers *notifiers)
{
	connect(notifiers, SIGNAL(destroyed(QObject *)),
			this, SLOT(socketNotifiersDestroyed(QObject *)));
// 	TODO: signal never emitted
// 	connect(notifiers, SIGNAL(incomingConnection(struct gg_dcc *)),
// 			this, SLOT(dccIncomingConnection(struct gg_dcc *)));
}

void DccManager::disconnectSocketNotifiers(DccSocketNotifiers *notifiers)
{
	disconnect(notifiers, SIGNAL(destroyed(QObject *)),
			this, SLOT(socketNotifiersDestroyed(QObject *)));
// 	disconnect(notifiers, SIGNAL(incomingConnection(struct gg_dcc *)),
// 			this, SLOT(dccIncomingConnection(struct gg_dcc *)));
}

void DccManager::socketNotifiersDestroyed(QObject *socketNotifiers)
{
	SocketNotifiers.removeAll(static_cast<DccSocketNotifiers *>(socketNotifiers));
}

bool DccManager::acceptConnection(UinType uin, UinType peerUin, unsigned int peerAddr)
{
	GaduAccountDetails *gaduAccountDetails = dynamic_cast<GaduAccountDetails *>(Protocol->account().details());
	if (!gaduAccountDetails)
		return false;

	Contact contact = ContactManager::instance()->byId(Protocol->account(), QString::number(peerUin));
	if (contact.isNull())
		return false;

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

	QHostAddress remoteAddress(ntohl(peerAddr));

	if (remoteAddress == contact.address())
		return true;

	kdebugm(KDEBUG_WARNING, "possible spoofing attempt from %s (uin:%u)\n", qPrintable(remoteAddress.toString()), peerUin);

	return MessageDialog::ask(QString(), tr("Kadu"), tr("%1 is asking for direct connection but his/her\n"
				"IP address (%2) differs from what GG server returned\n"
				"as his/her IP address (%3). It may be spoofing\n"
				"or he/she has port forwarding. Continue connection?").arg(buddy.display(),
					remoteAddress.toString(),
					contact.address().toString()));
}

void DccManager::needIncomingFileTransferAccept(DccSocketNotifiers *socket)
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

	Protocol->CurrentFileTransferService->newIncomingFileTransfer(fileTransfer);
}

GaduFileTransferHandler * DccManager::findFileTransferHandler(DccSocketNotifiers *notifiers)
{
	foreach (GaduFileTransferHandler *handler, WaitingFileTransfers)
	{
		UinType uin = Protocol->uin(handler->transfer().peer());
		if (uin == notifiers->peerUin())
		{
			disconnectSocketNotifiers(notifiers);
			SocketNotifiers.removeAll(notifiers);
			return handler;
		}
	}

	return 0;
}

void DccManager::handleEventDcc7New(struct gg_event *e)
{
	kdebugf();

	struct gg_dcc7 *dcc = e->event.dcc7_new;
/*
	if (!acceptConnection(dcc->uin, dcc->peer_uin, dcc->remote_addr))
	{
		gg_dcc7_reject(dcc, 0);
		gg_dcc7_free(dcc);
		return;
	}
*/
	switch (dcc->dcc_type)
	{
		case GG_DCC7_TYPE_FILE:
		{
// 			TODO: ZARAZ
			DccSocketNotifiers *newSocketNotifiers = new DccSocketNotifiers(Protocol, this);
			SocketNotifiers << newSocketNotifiers;
			connectSocketNotifiers(newSocketNotifiers);
			newSocketNotifiers->watchFor(e->event.dcc7_new);
			needIncomingFileTransferAccept(newSocketNotifiers);
// 			file_transfer_manager->dcc7IncomingFileTransfer(new DccSocket(dcc));
			break;
		}

		default:
			gg_dcc7_reject(dcc, GG_DCC7_REJECT_USER);
			gg_dcc7_free(dcc);
			break;
	}

	kdebugf2();
}

void DccManager::handleEventDcc7Accept(struct gg_event *e)
{
	kdebugf();

	foreach (DccSocketNotifiers *socketNotifiers, SocketNotifiers)
	{
		if (socketNotifiers->hasSocket(e->event.dcc7_accept.dcc7))
		{
			socketNotifiers->handleEventDcc7Accept(e);
			return;
		}
	}
}

void DccManager::handleEventDcc7Reject(struct gg_event *e)
{
	kdebugf();

	foreach (DccSocketNotifiers *socketNotifiers, SocketNotifiers)
	{
		if (socketNotifiers->hasSocket(e->event.dcc7_accept.dcc7))
		{
			socketNotifiers->handleEventDcc7Reject(e);
			return;
		}
	}
}

void DccManager::handleEventDcc7Pending(struct gg_event *e)
{
	kdebugf();

	foreach (DccSocketNotifiers *socketNotifiers, SocketNotifiers)
	{
		socketNotifiers->handleEventDcc7Pending(e);
		return;
	}
}

void DccManager::handleEventDcc7Error(struct gg_event *e)
{
	Q_UNUSED(e)

	kdebugf();

// 	foreach (DccSocketNotifiers *socketNotifiers, SocketNotifiers)
// 	{
// 		if (socketNotifiers->Socket7 == e->event.dcc7_error)
// 		{
// 			socketNotifiers->handleEventDcc7Error(e);
// 			return;
// 		}
// 	}
}

void DccManager::fileTransferHandlerDestroyed(QObject *object)
{
	WaitingFileTransfers.removeAll(static_cast<GaduFileTransferHandler *>(object));
}

void DccManager::attachSendFileTransferSocket(GaduFileTransferHandler *handler)
{
	Contact contact = handler->transfer().peer();
	if (contact.isNull())
		return;

	GaduContactDetails *details = Protocol->gaduContactDetails(contact);
	if (!details)
		return;

	gg_dcc7 *dcc = gg_dcc7_send_file(Protocol->gaduSession(), details->uin(),
			handler->transfer().localFileName().toUtf8().constData(),
			handler->transfer().localFileName().toUtf8().constData(), 0);

	if (dcc)
	{
		DccSocketNotifiers *fileTransferNotifiers = new DccSocketNotifiers(Protocol, this);
		handler->setFileTransferNotifiers(fileTransferNotifiers);
		handler->transfer().setTransferStatus(StatusWaitingForAccept);
		fileTransferNotifiers->watchFor(dcc);

		SocketNotifiers << fileTransferNotifiers;
	}
	else
		handler->socketNotAvailable();
}
