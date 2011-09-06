/*
 * %kadu copyright begin%
 * Copyright 2010 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2010, 2011 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2010, 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include <QtCore/QScopedArrayPointer>

#include <libgadu.h>

#include "buddies/buddy-manager.h"
#include "contacts/contact-manager.h"
#include "protocols/protocol.h"
#include "debug.h"

#include "helpers/gadu-protocol-helper.h"
#include "gadu-contact-details.h"
#include "gadu-protocol.h"

#include "gadu-contact-list-handler.h"

int GaduContactListHandler::notifyTypeFromContact(const Contact &contact)
{
	Buddy buddy = contact.ownerBuddy();

	if (buddy.isAnonymous())
		return 0;

	int result = 0x01; // GG_USER_BUDDY
	if (!buddy.isOfflineTo())
		result |= 0x02; // GG_USER_FRIEND
	if (buddy.isBlocked())
		result |= 0x04; // GG_USER_BLOCKED

	return result;
}

GaduContactListHandler::GaduContactListHandler(GaduProtocol *protocol) :
		QObject(protocol), Protocol(protocol), AlreadySent(false)
{
	connect(BuddyManager::instance(), SIGNAL(buddySubscriptionChanged(Buddy &)),
			this, SLOT(buddySubscriptionChanged(Buddy &)));
	connect(ContactManager::instance(), SIGNAL(contactAttached(Contact, bool)),
			this, SLOT(contactAttached(Contact, bool)));
	connect(ContactManager::instance(), SIGNAL(contactAboutToBeDetached(Contact, bool)),
			this, SLOT(contactAboutToBeDetached(Contact, bool)));
	connect(ContactManager::instance(), SIGNAL(contactIdChanged(Contact, const QString &)),
			this, SLOT(contactIdChanged(Contact, const QString &)));
}

GaduContactListHandler::~GaduContactListHandler()
{
	disconnect(BuddyManager::instance(), SIGNAL(buddySubscriptionChanged(Buddy &)),
			this, SLOT(buddySubscriptionChanged(Buddy &)));
	disconnect(ContactManager::instance(), SIGNAL(contactAttached(Contact, bool)),
			this, SLOT(contactAttached(Contact, bool)));
	disconnect(ContactManager::instance(), SIGNAL(contactAboutToBeDetached(Contact, bool)),
			this, SLOT(contactAboutToBeDetached(Contact, bool)));
}

void GaduContactListHandler::setUpContactList(const QList<Contact> &contacts)
{
	QList<Contact> sendList = contacts;
	sendList.removeAll(Protocol->account().accountContact());

	if (sendList.isEmpty())
	{
		gg_notify_ex(Protocol->gaduSession(), 0, 0, 0);
		kdebugmf(KDEBUG_NETWORK|KDEBUG_INFO, "Userlist is empty\n");

		AlreadySent = true;
		return;
	}

	int count = sendList.count();
	QScopedArrayPointer<UinType> uins(new UinType[count]);
	QScopedArrayPointer<char> types(new char[count]);

	int i = 0;

	foreach (const Contact &contact, sendList)
	{
		uins[i] = GaduProtocolHelper::uin(contact);
		types[i] = notifyTypeFromContact(contact);

		GaduContactDetails *details = GaduProtocolHelper::gaduContactDetails(contact);
		if (details)
			details->setGaduFlags(types[i]);

		++i;
	}

	gg_notify_ex(Protocol->gaduSession(), uins.data(), types.data(), count);
	kdebugmf(KDEBUG_NETWORK|KDEBUG_INFO, "Userlist sent\n");

	AlreadySent = true;
}

void GaduContactListHandler::reset()
{
	AlreadySent = false;
}

void GaduContactListHandler::updateContactEntry(Contact contact)
{
	if (!AlreadySent)
		return;

	if (!Protocol->isConnected())
		return;

	gg_session *session = Protocol->gaduSession();
	if (!session)
		return;

	GaduContactDetails *details = GaduProtocolHelper::gaduContactDetails(contact);
	if (!details)
		return;

	int uin = details->uin();
	if (!uin || Protocol->account().id() == QString::number(uin))
		return;

	int newFlags = notifyTypeFromContact(contact);
	int oldFlags = details->gaduFlags();
	details->setGaduFlags(newFlags);

	// add new flags
	if (!(oldFlags & 0x01) && (newFlags & 0x01))
		gg_add_notify_ex(session, uin, 0x01);
	if (!(oldFlags & 0x02) && (newFlags & 0x02))
		gg_add_notify_ex(session, uin, 0x02);
	if (!(oldFlags & 0x04) && (newFlags & 0x04))
		gg_add_notify_ex(session, uin, 0x04);

	// remove old flags
	if ((oldFlags & 0x01) && !(newFlags & 0x01))
		gg_remove_notify_ex(session, uin, 0x01);
	if ((oldFlags & 0x02) && !(newFlags & 0x02))
		gg_remove_notify_ex(session, uin, 0x02);
	if ((oldFlags & 0x04) && !(newFlags & 0x04))
		gg_remove_notify_ex(session, uin, 0x04);
}

void GaduContactListHandler::buddySubscriptionChanged(Buddy &buddy)
{
	// update offline to and other data
	foreach (const Contact &contact, buddy.contacts(Protocol->account()))
		updateContactEntry(contact);
}

void GaduContactListHandler::contactAttached(Contact contact, bool reattached)
{
	Q_UNUSED(reattached)

	if (contact.contactAccount() != Protocol->account())
		return;

	updateContactEntry(contact);
}

void GaduContactListHandler::contactAboutToBeDetached(Contact contact, bool reattaching)
{
	if (reattaching)
		return;

	if (contact.contactAccount() != Protocol->account())
		return;

	updateContactEntry(contact);
}

void GaduContactListHandler::contactIdChanged(Contact contact, const QString &oldId)
{
	if (contact.contactAccount() != Protocol->account())
		return;

	if (!AlreadySent)
		return;

	if (!Protocol->isConnected())
		return;

	gg_session *session = Protocol->gaduSession();
	if (!session)
		return;

	bool ok;
	UinType oldUin = oldId.toUInt(&ok);
	if (session && ok)
	{
		// remove all flags
		gg_remove_notify_ex(session, oldUin, 0x04);
		gg_remove_notify_ex(session, oldUin, 0x02);
		gg_remove_notify_ex(session, oldUin, 0x01);
	}

	updateContactEntry(contact);
}
