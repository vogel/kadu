/*
 * %kadu copyright begin%
 * Copyright 2010, 2011 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2009, 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009, 2010, 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2009 Bartłomiej Zimoń (uzi18@o2.pl)
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

#include "accounts/account-manager.h"
#include "avatars/avatar.h"
#include "avatars/avatar-manager.h"
#include "avatars/avatar-shared.h"
#include "buddies/buddy-manager.h"
#include "buddies/buddy-shared.h"
#include "contacts/contact-details.h"
#include "contacts/contact-manager.h"
#include "protocols/protocol.h"

#include "contact-shared.h"

ContactShared * ContactShared::loadStubFromStorage(const QSharedPointer<StoragePoint> &storagePoint)
{
	ContactShared *result = loadFromStorage(storagePoint);
	result->loadStub();

	return result;
}

ContactShared * ContactShared::loadFromStorage(const QSharedPointer<StoragePoint> &storagePoint)
{
	ContactShared *result = new ContactShared();
	result->setStorage(storagePoint);

	return result;
}

ContactShared::ContactShared(QUuid uuid) :
		Shared(uuid),
		ContactAccount(Account::null), ContactAvatar(Avatar::null), OwnerBuddy(Buddy::null),
		Priority(-1), MaximumImageSize(0), Blocking(false), Port(0)
{
}

ContactShared::~ContactShared()
{
	ref.ref();

	triggerAllProtocolsUnregistered();
}

StorableObject * ContactShared::storageParent()
{
	return ContactManager::instance();
}

QString ContactShared::storageNodeName()
{
	return QLatin1String("Contact");
}

void ContactShared::load()
{
	if (!isValidStorage())
		return;

	Shared::load();

	Id = loadValue<QString>("Id");

	Priority = loadValue<int>("Priority", -1);

	ContactAccount = AccountManager::instance()->byUuid(loadValue<QString>("Account"));

	QString buddyUuid = loadValue<QString>("Buddy");
	if (buddyUuid.isNull())
		buddyUuid = loadValue<QString>("Contact");

	setOwnerBuddy(BuddyManager::instance()->byUuid(buddyUuid));

	if (storage()->point().isElement())
	{
		QDomNodeList avatars = storage()->point().elementsByTagName("Avatar");
		if (avatars.count() == 1)
			if (!avatars.at(0).firstChildElement("LastUpdated").isNull())
				storage()->point().removeChild(avatars.at(0));
	}

	ContactAvatar = AvatarManager::instance()->byUuid(loadValue<QString>("Avatar"));

	triggerAllProtocolsRegistered();
}

void ContactShared::aboutToBeRemoved()
{
	// clean up references
	ContactAccount = Account::null;
	OwnerBuddy = Buddy::null;

	AvatarManager::instance()->removeItem(ContactAvatar);
	ContactAvatar = Avatar::null;

	setDetails(0);
}

void ContactShared::store()
{
	if (!isValidStorage())
		return;

	ensureLoaded();

	Shared::store();

	storeValue("Id", Id);
	storeValue("Priority", Priority);
	storeValue("Account", ContactAccount.uuid().toString());

	if (OwnerBuddy.isAnonymous())
		storeValue("Buddy", QString());
	else
		storeValue("Buddy", OwnerBuddy.uuid().toString());

	if (!ContactAvatar.isNull())
		storeValue("Avatar", ContactAvatar.uuid().toString());
	removeValue("Contact");
}

bool ContactShared::shouldStore()
{
	ensureLoaded();

	return UuidStorableObject::shouldStore() && !Id.isEmpty() && !ContactAccount.uuid().isNull();
}

void ContactShared::emitUpdated()
{
	emit updated();
}

void ContactShared::detach(const Buddy &buddy, bool emitSignals)
{
	if (!details() || !buddy)
		return;

	/* NOTE: This guard is needed to delay deleting this object when removing
	 * Contact from Buddy which holds last reference to it and thus wants to
	 * delete it. But we don't want this to happen now because we have to emit
	 * detached() signal first.
	 */
	Contact guard(this);

	if (emitSignals)
		emit aboutToBeDetached();

	OwnerBuddy.removeContact(this);

	if (emitSignals)
		emit detached();
}

void ContactShared::attach(const Buddy &buddy, bool emitReattached)
{
	if (!details())
		return;

	if (!buddy)
		return;

	if (!emitReattached)
		emit aboutToBeAttached();

	OwnerBuddy.addContact(this);

	if (!emitReattached)
		emit attached();
	else
		emit reattached();
}

void ContactShared::setOwnerBuddy(Buddy buddy)
{
	ensureLoaded();

	if (OwnerBuddy == buddy)
		return;

	/* NOTE: This guard is needed to avoid deleting this object when removing
	 * Contact from Buddy which may hold last reference to it and thus wants to
	 * delete it. But we don't want this to happen.
	 */
	Contact guard(this);

	bool hadBuddy = !OwnerBuddy.isNull() && !OwnerBuddy.isAnonymous();

	detach(OwnerBuddy, !buddy);
	OwnerBuddy = buddy;
	attach(OwnerBuddy, hadBuddy);

	// TODO: make it pretty
	// don't allow empty buddy to be set, use at least anonymous one
	if (!OwnerBuddy)
		OwnerBuddy = BuddyManager::instance()->byContact(Contact(this), ActionCreate);

	dataUpdated();
}

void ContactShared::setContactAccount(Account account)
{
	ensureLoaded();

	if (ContactAccount == account)
		return;

	if (ContactAccount && ContactAccount.protocolHandler() && ContactAccount.protocolHandler()->protocolFactory())
		protocolUnregistered(ContactAccount.protocolHandler()->protocolFactory());

	ContactAccount = account;

	if (ContactAccount && ContactAccount.protocolHandler() && ContactAccount.protocolHandler()->protocolFactory())
		protocolRegistered(ContactAccount.protocolHandler()->protocolFactory());

	dataUpdated();
}

void ContactShared::protocolRegistered(ProtocolFactory *protocolFactory)
{
	ensureLoaded();

	if (ContactAccount.protocolName() != protocolFactory->name())
		return;

	if (details())
		return;

	setDetails(protocolFactory->createContactDetails(this));
}

void ContactShared::protocolUnregistered(ProtocolFactory *protocolFactory)
{
	ensureLoaded();

	if (Id.isEmpty())
		return;

	if (ContactAccount.protocolName() != protocolFactory->name())
		return;

	setDetails(0);
}

void ContactShared::detailsAdded()
{
	details()->ensureLoaded();

	dataUpdated();

	ContactManager::instance()->detailsLoaded(this);
}

void ContactShared::afterDetailsAdded()
{
	attach(OwnerBuddy, false);
}

void ContactShared::detailsAboutToBeRemoved()
{
	// do not store contacts that are not in contact manager
	if (ContactManager::instance()->allItems().contains(uuid().toString()))
		details()->store();

	detach(OwnerBuddy, true);
}

void ContactShared::detailsRemoved()
{
	ContactManager::instance()->detailsUnloaded(this);

	dataUpdated();
}

void ContactShared::setId(const QString &id)
{
	ensureLoaded();

	if (Id == id)
		return;

	QString oldId = Id;
	Id = id;

	dataUpdated();
	emit idChanged(oldId);
}
