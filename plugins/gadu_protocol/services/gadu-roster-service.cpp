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

#include "contacts/contact-manager.h"
#include "debug.h"

#include "helpers/gadu-protocol-helper.h"
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

GaduRosterService::GaduRosterService(Protocol *protocol) :
		RosterService(protocol), GaduSession(0)
{
	Q_ASSERT(protocol);
}

GaduRosterService::~GaduRosterService()
{
}

void GaduRosterService::setGaduSession(gg_session *gaduSession)
{
	GaduSession = gaduSession;
}

void GaduRosterService::prepareRoster()
{
	Q_ASSERT(StateNonInitialized == state());
	Q_ASSERT(GaduSession);

	setState(StateInitializing);

	QVector<Contact> allContacts = ContactManager::instance()->contacts(account());
	QVector<Contact> sendList;

	foreach (const Contact &contact, allContacts)
		if (!contact.isAnonymous() && contact != account().accountContact())
			sendList.append(contact);

	if (sendList.isEmpty())
	{
		gg_notify_ex(GaduSession, 0, 0, 0);
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

	gg_notify_ex(static_cast<GaduProtocol *>(protocol())->gaduSession(), uins.data(), types.data(), count);
	kdebugmf(KDEBUG_NETWORK|KDEBUG_INFO, "Userlist sent\n");

	setState(StateInitialized);
	emit rosterReady(true);
}

void GaduRosterService::updateFlag(int uin, int newFlags, int oldFlags, int flag) const
{
	if (!GaduSession)
		return;

	if (!(oldFlags & flag) && (newFlags & flag))
		gg_add_notify_ex(GaduSession, uin, flag);
	if ((oldFlags & flag) && !(newFlags & flag))
		gg_remove_notify_ex(GaduSession, uin, flag);
}

void GaduRosterService::sendNewFlags(const Contact &contact, int newFlags) const
{
	GaduContactDetails *details = GaduProtocolHelper::gaduContactDetails(contact);
	if (!details)
		return;

	int uin = details->uin();
	int oldFlags = details->gaduFlags();

	if (newFlags == oldFlags)
		return;

	details->setGaduFlags(newFlags);

	updateFlag(uin, newFlags, oldFlags, 0x01);
	updateFlag(uin, newFlags, oldFlags, 0x02);
	updateFlag(uin, newFlags, oldFlags, 0x04);
}

bool GaduRosterService::addContact(const Contact &contact)
{
	if (!canPerformLocalUpdate() ||
	        account() != contact.contactAccount() ||
	        account().accountContact() == contact)
		return false;

	Q_ASSERT(StateInitialized == state());

	if (!RosterService::addContact(contact))
		return false;

	setState(StateProcessingLocalUpdate);
	sendNewFlags(contact, notifyTypeFromContact(contact));
	setState(StateInitialized);

	return true;
}

bool GaduRosterService::removeContact(const Contact &contact)
{
	if (!canPerformLocalUpdate() ||
	        account() != contact.contactAccount() ||
	        account().accountContact() == contact)
		return false;

	Q_ASSERT(StateInitialized == state());

	if (!RosterService::removeContact(contact))
		return false;

	setState(StateProcessingLocalUpdate);
	sendNewFlags(contact, 0);
	setState(StateInitialized);

	return true;
}

void GaduRosterService::updateContact(const Contact &contact)
{
	if (!canPerformLocalUpdate() ||
	        account() != contact.contactAccount() ||
	        account().accountContact() == contact)
		return;

	Q_ASSERT(StateInitialized == state());

	setState(StateProcessingLocalUpdate);
	sendNewFlags(contact, notifyTypeFromContact(contact));
	setState(StateInitialized);
}
