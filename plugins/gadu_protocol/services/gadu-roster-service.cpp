/*
 * %kadu copyright begin%
 * Copyright 2010, 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2010, 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2010, 2011 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#include "gadu-roster-service.h"

int GaduRosterService::notifyTypeFromContact(const Contact &contact)
{
	if (contact.isAnonymous())
		return 0;

	Buddy buddy = contact.ownerBuddy();
	int result = 0x01; // GG_USER_BUDDY
	if (!buddy.isOfflineTo())
		result |= 0x02; // GG_USER_FRIEND
	if (buddy.isBlocked())
		result |= 0x04; // GG_USER_BLOCKED

	return result;
}

GaduRosterService::GaduRosterService(GaduProtocol *protocol) :
		RosterService(protocol), AlreadySent(false)
{
	connect(BuddyManager::instance(), SIGNAL(buddySubscriptionChanged(Buddy &)),
			this, SLOT(buddySubscriptionChanged(Buddy &)));
	connect(ContactManager::instance(), SIGNAL(contactAttached(Contact, bool)),
			this, SLOT(contactAttached(Contact, bool)));
	connect(ContactManager::instance(), SIGNAL(contactDetached(Contact, Buddy, bool)),
			this, SLOT(contactDetached(Contact, Buddy, bool)));
}

GaduRosterService::~GaduRosterService()
{
	disconnect(BuddyManager::instance(), SIGNAL(buddySubscriptionChanged(Buddy &)),
			this, SLOT(buddySubscriptionChanged(Buddy &)));
	disconnect(ContactManager::instance(), SIGNAL(contactAttached(Contact, bool)),
			this, SLOT(contactAttached(Contact, bool)));
	disconnect(ContactManager::instance(), SIGNAL(contactDetached(Contact, Buddy, bool)),
			this, SLOT(contactDetached(Contact, Buddy, bool)));
}

void GaduRosterService::setUpContactList(const QVector<Contact> &contacts)
{
	QVector<Contact> sendList = contacts;
	sendList.remove(sendList.indexOf(protocol()->account().accountContact()));

	if (sendList.isEmpty())
	{
		gg_notify_ex(static_cast<GaduProtocol *>(protocol())->gaduSession(), 0, 0, 0);
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

	gg_notify_ex(static_cast<GaduProtocol *>(protocol())->gaduSession(), uins.data(), types.data(), count);
	kdebugmf(KDEBUG_NETWORK|KDEBUG_INFO, "Userlist sent\n");

	AlreadySent = true;
}

void GaduRosterService::reset()
{
	AlreadySent = false;
}

void GaduRosterService::buddySubscriptionChanged(Buddy &buddy)
{
	// update offline to and other data
	updateBuddyContacts(buddy);
}

void GaduRosterService::contactAttached(Contact contact, bool reattached)
{
	Q_UNUSED(reattached)

	if (contact.contactAccount() != protocol()->account())
		return;

	// see issue #2159 - we need a way to ignore first status of given contact
	GaduContactDetails *details = static_cast<GaduContactDetails *>(contact.details());
	if (details)
		details->setIgnoreNextStatusChange(true);

	updateContact(contact);
}

void GaduRosterService::contactDetached(Contact contact, Buddy previousBuddy, bool reattaching)
{
	Q_UNUSED(previousBuddy)

	if (reattaching)
		return;

	if (contact.contactAccount() != protocol()->account())
		return;

	updateContact(contact);
}

void GaduRosterService::addContact(const Contact &contact)
{
	Q_UNUSED(contact);
}

void GaduRosterService::removeContact(const Contact &contact)
{
	Q_UNUSED(contact);
}

void GaduRosterService::updateContact(const Contact &contact)
{
	if (!AlreadySent)
		return;

	if (!protocol()->isConnected())
		return;

	gg_session *session = static_cast<GaduProtocol *>(protocol())->gaduSession();
	if (!session)
		return;

	GaduContactDetails *details = GaduProtocolHelper::gaduContactDetails(contact);
	if (!details)
		return;

	int uin = details->uin();
	if (!uin || protocol()->account().id() == QString::number(uin))
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
