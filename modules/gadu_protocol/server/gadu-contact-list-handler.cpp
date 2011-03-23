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

#include "protocols/protocol.h"
#include "debug.h"

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

GaduContactListHandler::GaduContactListHandler(GaduProtocol *protocol, QObject *parent) :
		QObject(parent), Protocol(protocol), AlreadySent(false)
{
}

GaduContactListHandler::~GaduContactListHandler()
{
}

void GaduContactListHandler::setUpContactList(const QList<Contact> &contacts)
{
	if (::Protocol::NetworkConnected != Protocol->state())
		return;

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
		uins[i] = Protocol->uin(contact);
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

	if (::Protocol::NetworkConnected != Protocol->state())
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
	addContactEntry(Protocol->uin(contact), notifyTypeFromContact(contact));
}

void GaduContactListHandler::removeContactEntry(UinType uin)
{
	if (!AlreadySent)
		return;

	if (!uin)
		return;

	if (::Protocol::NetworkConnected != Protocol->state())
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
	removeContactEntry(Protocol->uin(contact));
}
