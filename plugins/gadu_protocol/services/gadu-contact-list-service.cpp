/*
 * %kadu copyright begin%
 * Copyright 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2009 Bartłomiej Zimoń (uzi18@o2.pl)
 * Copyright 2009, 2010, 2011, 2012 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2010, 2011, 2012, 2013 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#include <libgadu.h>

#include <QtCore/QByteArray>

#include "buddies/buddy-manager.h"
#include "contacts/contact-manager.h"
#include "contacts/contact.h"
#include "core/core.h"
#include "misc/misc.h"
#include "protocols/services/roster/roster-entry.h"
#include "protocols/services/roster/roster-notifier.h"
#include "debug.h"

#include "helpers/gadu-list-helper.h"
#include "server/gadu-connection.h"
#include "server/gadu-writable-session-token.h"
#include "services/gadu-contact-list-state-machine.h"
#include "gadu-account-details.h"

#include "gadu-contact-list-service.h"

GaduContactListService::GaduContactListService(const Account &account, Protocol *protocol) :
		ContactListService(account, protocol), StateMachine(new GaduContactListStateMachine(this, protocol))
{
	connect(StateMachine, SIGNAL(performGet()), SLOT(importContactList()));
	connect(StateMachine, SIGNAL(performPut()), SLOT(exportContactList()));

	connect(ContactManager::instance(), SIGNAL(dirtyContactAdded(Contact)), SLOT(dirtyContactAdded(Contact)));

	StateMachine->start();

	if (!ContactManager::instance()->dirtyContacts(account).isEmpty())
		emit stateMachineLocalDirty();
}

GaduContactListService::~GaduContactListService()
{
}

void GaduContactListService::setConnection(GaduConnection *connection)
{
	Connection = connection;
}

void GaduContactListService::setRosterNotifier(RosterNotifier *rosterNotifier)
{
	MyRosterNotifier = rosterNotifier;
}

void GaduContactListService::putFinished(bool ok)
{
	if (ok)
	{
		emit stateMachinePutFinished();
		if (MyRosterNotifier)
			MyRosterNotifier.data()->notifyExportSucceeded(account());
	}
	else
	{
		emit stateMachinePutFailed();
		if (MyRosterNotifier)
			MyRosterNotifier.data()->notifyExportFailed(account());
	}
}

void GaduContactListService::getFinished(bool ok)
{
	if (ok)
	{
		emit stateMachineGetFinished();
		if (MyRosterNotifier)
			MyRosterNotifier.data()->notifyImportSucceeded(account());
	}
	else
	{
		emit stateMachineGetFailed();
		if (MyRosterNotifier)
			MyRosterNotifier.data()->notifyImportFailed(account());
	}
}

void GaduContactListService::handleEventUserlist100GetReply(struct gg_event *e)
{
	if (!StateMachine->isPerformingGet())
	{
		kdebugmf(KDEBUG_NETWORK|KDEBUG_INFO, "got unexpected userlist 100 get reply, ignoring\n");
		return;
	}

	GaduAccountDetails *accountDetails = dynamic_cast<GaduAccountDetails *>(account().details());
	if (!accountDetails)
	{
		getFinished(false);
		return;
	}

	if (e->event.userlist100_reply.format_type != GG_USERLIST100_FORMAT_TYPE_GG70)
	{
		kdebugmf(KDEBUG_NETWORK|KDEBUG_INFO, "got userlist 100 reply with unwanted format type (%d)\n", (int)e->event.userlist100_reply.format_type);
		getFinished(false);
		return;
	}

	const char *content = e->event.userlist100_reply.reply;
	if (!content)
	{
		kdebugmf(KDEBUG_NETWORK|KDEBUG_INFO, "got userlist 100 reply without any content\n");
		getFinished(false);
		return;
	}

	kdebugmf(KDEBUG_NETWORK|KDEBUG_INFO, "userlist 100 reply:\n%s\n", content);

	if (accountDetails->userlistVersion() != (int)e->event.userlist100_reply.version)
	{
		QByteArray content2(content);
		BuddyList buddies = GaduListHelper::byteArrayToBuddyList(account(), content2);
		getFinished(true);

		setBuddiesList(buddies, true);
		accountDetails->setUserlistVersion(e->event.userlist100_reply.version);
		accountDetails->setInitialRosterImport(false);

		// cleanup references, so buddy and contact instances can be removed
		// this is really a hack, we need to call aboutToBeRemoved someway for non-manager contacts and buddies too
		// or just only store managed only, i dont know yet
		foreach (Buddy buddy, buddies)
		{
			foreach (Contact contact, buddy.contacts())
				contact.data()->aboutToBeRemoved();
			buddy.data()->aboutToBeRemoved();
		}
	}
	else
	{
		kdebugmf(KDEBUG_NETWORK|KDEBUG_INFO, "ignoring userlist 100 reply as we already know that version\n");

		getFinished(true);
	}
}

void GaduContactListService::handleEventUserlist100PutReply(struct gg_event *e)
{
	if (!StateMachine->isPerformingPut())
	{
		kdebugmf(KDEBUG_NETWORK|KDEBUG_INFO, "got unexpected userlist 100 put reply, ignoring\n");
		return;
	}

	if (e->event.userlist100_reply.type == GG_USERLIST100_REPLY_ACK)
	{
		GaduAccountDetails *accountDetails = dynamic_cast<GaduAccountDetails *>(account().details());
		if (accountDetails)
		{
			accountDetails->setUserlistVersion(e->event.userlist100_reply.version);

			foreach (const Contact &contact, ContactManager::instance()->dirtyContacts(account()))
				contact.rosterEntry()->setState(RosterEntrySynchronized);

			putFinished(true);
			return;
		}
	}

	putFinished(false);
}

void GaduContactListService::handleEventUserlist100Reply(struct gg_event *e)
{
	switch (e->event.userlist100_reply.type)
	{
		case GG_USERLIST100_REPLY_LIST:
			handleEventUserlist100GetReply(e);
			break;
		case GG_USERLIST100_REPLY_ACK:
		case GG_USERLIST100_REPLY_REJECT:
			handleEventUserlist100PutReply(e);
			break;
		default:
			kdebugmf(KDEBUG_NETWORK|KDEBUG_INFO, "got unknown userlist100 reply type (%d)\n", e->event.userlist100_reply.type);
	}
}

void GaduContactListService::handleEventUserlist100Version(gg_event *e)
{
	kdebugmf(KDEBUG_NETWORK|KDEBUG_INFO, "new version of userlist available: %d\n", e->event.userlist100_version.version);

	GaduAccountDetails *accountDetails = dynamic_cast<GaduAccountDetails *>(account().details());
	if (accountDetails && accountDetails->userlistVersion() != (int)e->event.userlist100_version.version)
		emit stateMachineRemoteDirty();
}

void GaduContactListService::dirtyContactAdded(Contact contact)
{
	if (contact.contactAccount() == account())
		QMetaObject::invokeMethod(this, "stateMachineLocalDirty", Qt::QueuedConnection);
}

bool GaduContactListService::haveToAskForAddingContacts() const
{
	GaduAccountDetails *accountDetails = dynamic_cast<GaduAccountDetails *>(account().details());
	if (!accountDetails) // assert?
		return true;

	// if already synchronized, never ask
	if (-1 != accountDetails->userlistVersion())
		return false;

	// if not yet synchronized but also not migrating from 0.9.x, i.e., it's a clean install, do not ask as well
	if (accountDetails->initialRosterImport())
		return false;

	// here is the case for migrating from 0.9.x - ask then
	return true;
}

void GaduContactListService::importContactList()
{
	if (!Connection || !Connection.data()->hasSession())
		return;

	emit stateMachineGetStarted();

	auto writableSessionToken = Connection.data()->writableSessionToken();
	int ret = gg_userlist100_request(writableSessionToken.rawSession(), GG_USERLIST100_GET, 0, GG_USERLIST100_FORMAT_TYPE_GG70, 0);
	if (-1 == ret)
		getFinished(false);
}

void GaduContactListService::exportContactList()
{
	exportContactList(BuddyManager::instance()->buddies(account()));
}

void GaduContactListService::exportContactList(const BuddyList &buddies)
{
	if (!Connection || !Connection.data()->hasSession())
		return;

	emit stateMachinePutStarted();

	QByteArray contacts = GaduListHelper::buddyListToByteArray(account(), buddies);

	kdebugmf(KDEBUG_NETWORK|KDEBUG_INFO, "\n%s\n", contacts.constData());

	GaduAccountDetails *accountDetails = dynamic_cast<GaduAccountDetails *>(account().details());
	if (!accountDetails)
	{
		putFinished(false);
		return;
	}

	auto writableSessionToken = Connection.data()->writableSessionToken();
	int ret = gg_userlist100_request(writableSessionToken.rawSession(),
			GG_USERLIST100_PUT, accountDetails->userlistVersion(), GG_USERLIST100_FORMAT_TYPE_GG70, contacts.constData());
	if (-1 == ret)
		putFinished(false);
}

void GaduContactListService::copySupportedBuddyInformation(const Buddy &destination, const Buddy &source)
{
	GaduListHelper::setSupportedBuddyInformation(destination, source);
}

QList<Buddy> GaduContactListService::loadBuddyList(QTextStream &dataStream)
{
	return GaduListHelper::streamToBuddyList(account(), dataStream);
}

QByteArray GaduContactListService::storeBuddyList(const BuddyList &buddies)
{
	return GaduListHelper::buddyListToByteArray(account(), buddies);
}

#include "moc_gadu-contact-list-service.cpp"
