/*
 * %kadu copyright begin%
 * Copyright 2010, 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2010, 2011, 2012, 2013 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2010, 2011, 2012, 2013, 2014 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#include "contacts/contact-manager.h"
#include "debug.h"

#include "helpers/gadu-protocol-helper.h"
#include "server/gadu-connection.h"
#include "server/gadu-writable-session-token.h"
#include "gadu-contact-details.h"

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

GaduRosterService::GaduRosterService(Account account, QObject *parent) :
		RosterService(account, parent)
{
}

GaduRosterService::~GaduRosterService()
{
}
void GaduRosterService::setConnection(GaduConnection *connection)
{
	Connection = connection;
}

void GaduRosterService::prepareRoster(const QVector<Contact> &contacts)
{
	if (!Connection || !Connection->hasSession())
		return;

	RosterService::prepareRoster(contacts);

	Q_ASSERT(StateNonInitialized == state());

	setState(StateInitializing);

	QVector<Contact> allContacts = ContactManager::instance()->contacts(account());
	QVector<Contact> sendList;

	foreach (const Contact &contact, allContacts)
		if (!contact.isAnonymous() && contact != account().accountContact())
			sendList.append(contact);

	if (sendList.isEmpty())
	{
		auto writableSessionToken = Connection->writableSessionToken();
		gg_notify_ex(writableSessionToken.rawSession(), 0, 0, 0);
		kdebugmf(KDEBUG_NETWORK|KDEBUG_INFO, "Userlist is empty\n");

		setState(StateInitialized);
		emit rosterReady(true);
		return;
	}

	int count = sendList.count();
	QScopedArrayPointer<UinType> uins(new UinType[count]);
	QScopedArrayPointer<char> types(new char[count]);

	int i = 0;

	foreach (const Contact &contact, sendList)
	{
		RosterService::addContact(contact);

		uins[i] = GaduProtocolHelper::uin(contact);
		types[i] = notifyTypeFromContact(contact);

		GaduContactDetails *details = GaduProtocolHelper::gaduContactDetails(contact);
		if (details)
			details->setGaduFlags(types[i]);

		++i;
	}

	auto writableSessionToken = Connection->writableSessionToken();
	gg_notify_ex(writableSessionToken.rawSession(), uins.data(), types.data(), count);
	kdebugmf(KDEBUG_NETWORK|KDEBUG_INFO, "Userlist sent\n");

	setState(StateInitialized);
	emit rosterReady(true);
}

void GaduRosterService::updateFlag(gg_session *session, int uin, int newFlags, int oldFlags, int flag) const
{
	Q_ASSERT(session);

	if (!(oldFlags & flag) && (newFlags & flag))
		gg_add_notify_ex(session, uin, flag);
	if ((oldFlags & flag) && !(newFlags & flag))
		gg_remove_notify_ex(session, uin, flag);
}

void GaduRosterService::sendNewFlags(const Contact &contact, int newFlags) const
{
	// TODO: This is broken... Complete lack of error handling. This method is called
	// from executeTask() which assumes the task is always actually executed and removes
	// it from the list. While there are two returns for error conditions in this
	// method, and also gg_add_notify_ex() or gg_remove_notify_ex() in updateFlag()
	// could fail.

	if (!Connection || !Connection->hasSession())
		return;

	GaduContactDetails *details = GaduProtocolHelper::gaduContactDetails(contact);
	if (!details)
		return;

	int uin = details->uin();
	int oldFlags = details->gaduFlags();

	if (newFlags == oldFlags)
		return;

	details->setGaduFlags(newFlags);

	auto writableSessionToken = Connection->writableSessionToken();
	updateFlag(writableSessionToken.rawSession(), uin, newFlags, oldFlags, 0x01);
	updateFlag(writableSessionToken.rawSession(), uin, newFlags, oldFlags, 0x02);
	updateFlag(writableSessionToken.rawSession(), uin, newFlags, oldFlags, 0x04);
}

void GaduRosterService::executeTask(const RosterTask &task)
{
	Q_ASSERT(StateInitialized == state());

	Contact contact = ContactManager::instance()->byId(account(), task.id(), ActionReturnNull);
	switch (task.type())
	{
		case RosterTaskAdd:
		case RosterTaskUpdate:
			sendNewFlags(contact, notifyTypeFromContact(contact));
			break;
		case RosterTaskDelete:
			sendNewFlags(contact, 0);
			break;
		default:
			break;
	}
}

#include "moc_gadu-roster-service.cpp"
