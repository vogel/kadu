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
#include "gadu-protocol.h"

#include "gadu-contact-list-handler.h"

int GaduContactListHandler::notifyTypeFromContact(const Contact &contact)
{
	Buddy buddy = contact.ownerBuddy();

	return buddy.isOfflineTo()
		? GG_USER_OFFLINE
		: buddy.isBlocked()
			? GG_USER_BLOCKED
			: GG_USER_NORMAL;
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
	/*
	 * it looks like gadu-gadu now stores contact list mask (offlineto, blocked, normal)
	 * on server, so need to remove this mask and send a new one for each contact, so
	 * server has up-to-date information about our contact list
	 */

	// send empty list
	//gg_notify_ex(Protocol->gaduSession(), 0, 0, 0);

	// send all items
	//foreach (const Contact &contact, contacts)
	//	addContactEntry(contact);

	if (contacts.isEmpty())
	{
		gg_notify_ex(Protocol->gaduSession(), 0, 0, 0);
		kdebugmf(KDEBUG_NETWORK|KDEBUG_INFO, "Userlist is empty\n");
		return;
	}

	int count = contacts.count();
	QScopedArrayPointer<UinType> uins(new UinType[count]);
	QScopedArrayPointer<char> types(new char[count]);

	int i = 0;

	foreach (const Contact &contact, contacts)
	{
		uins[i] = GaduProtocolHelper::uin(contact);
		types[i] = notifyTypeFromContact(contact);
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
	addContactEntry(contact);
}

void GaduContactListHandler::addContactEntry(UinType uin, int type)
{
	if (!AlreadySent)
		return;

	if (!Protocol->isConnected())
		return;

	if (!uin || Protocol->account().id() == QString::number(uin))
		return;

	gg_session *session = Protocol->gaduSession();
	if (!session)
		return;

	gg_remove_notify_ex(session, uin, GG_USER_NORMAL);
	gg_remove_notify_ex(session, uin, GG_USER_BLOCKED);
	gg_remove_notify_ex(session, uin, GG_USER_OFFLINE);

	gg_add_notify_ex(session, uin, type);
}

void GaduContactListHandler::addContactEntry(Contact contact)
{
	addContactEntry(GaduProtocolHelper::uin(contact), notifyTypeFromContact(contact));
}

void GaduContactListHandler::removeContactEntry(UinType uin)
{
	if (!AlreadySent)
		return;

	if (!uin)
		return;

	if (!Protocol->isConnected())
		return;

	gg_session *session = Protocol->gaduSession();
	if (!session)
		return;

	gg_remove_notify_ex(session, uin, GG_USER_NORMAL);
	gg_remove_notify_ex(session, uin, GG_USER_BLOCKED);
	gg_remove_notify_ex(session, uin, GG_USER_OFFLINE);
}

void GaduContactListHandler::removeContactEntry(Contact contact)
{
	removeContactEntry(GaduProtocolHelper::uin(contact));
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

	addContactEntry(contact);
}

void GaduContactListHandler::contactAboutToBeDetached(Contact contact, bool reattaching)
{
	if (reattaching)
		return;

	if (contact.contactAccount() != Protocol->account())
		return;

	removeContactEntry(contact);
}

void GaduContactListHandler::contactIdChanged(Contact contact, const QString &oldId)
{
	if (contact.contactAccount() != Protocol->account())
		return;

	bool ok;
	UinType oldUin = oldId.toUInt(&ok);
	if (ok)
		removeContactEntry(oldUin);

	addContactEntry(contact);
}
