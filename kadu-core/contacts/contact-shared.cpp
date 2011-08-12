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

ContactShared::ContactShared(const QUuid &uuid) :
		Shared(uuid),
		Priority(-1), MaximumImageSize(0), Blocking(false), Dirty(true), Port(0)
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
	
	Dirty = loadValue<bool>("Dirty", true);

	ContactAccount = AccountManager::instance()->byUuid(loadValue<QString>("Account"));

	QString buddyUuid = loadValue<QString>("Buddy");
	if (buddyUuid.isNull())
		buddyUuid = loadValue<QString>("Contact");
	doSetOwnerBuddy(BuddyManager::instance()->byUuid(buddyUuid), false);

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
	storeValue("Dirty", Dirty);
	storeValue("Account", ContactAccount.uuid().toString());
	storeValue("Buddy", !OwnerBuddy.isAnonymous()
			? OwnerBuddy.uuid().toString()
			: QString());

	if (ContactAvatar)
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

void ContactShared::detach(bool reattaching, bool emitSignals)
{
	if (!details())
		return;

	if (!OwnerBuddy)
		return;

	/* NOTE: This guard is needed to delay deleting this object when removing
	 * Contact from Buddy which holds last reference to it and thus wants to
	 * delete it. But we don't want this to happen now because we have to emit
	 * detached() signal first.
	 */
	Contact guard(this);

	if (emitSignals)
		emit aboutToBeDetached(reattaching);

	OwnerBuddy.removeContact(this);

	if (emitSignals)
		emit detached(OwnerBuddy);
}

void ContactShared::attach(bool reattaching, bool emitSignals)
{
	if (!details())
		return;

	if (!OwnerBuddy)
		return;

	if (emitSignals)
		emit aboutToBeAttached(OwnerBuddy);

	OwnerBuddy.addContact(this);

	if (emitSignals)
		emit attached(reattaching);
}

void ContactShared::doSetOwnerBuddy(const Buddy &buddy, bool emitSignals)
{
	/* NOTE: This guard is needed to avoid deleting this object when removing
	 * Contact from Buddy which may hold last reference to it and thus wants to
	 * delete it. But we don't want this to happen.
	 */
	Contact guard(this);

	bool oldIsNotAnonymous = !OwnerBuddy.isAnonymous();
	bool newIsNotAnonymous = !buddy.isAnonymous();
	bool reattaching = oldIsNotAnonymous && newIsNotAnonymous;

	detach(reattaching, emitSignals && oldIsNotAnonymous);

	OwnerBuddy = buddy;

	// TODO: make it pretty
	// don't allow empty buddy to be set, use at least anonymous one
	if (!OwnerBuddy)
		OwnerBuddy = BuddyManager::instance()->byContact(this, ActionCreate);

	attach(reattaching, emitSignals && newIsNotAnonymous);
}

void ContactShared::setOwnerBuddy(const Buddy &buddy)
{
	ensureLoaded();

	if (OwnerBuddy == buddy)
		return;

	/* NOTE: This guard is needed to avoid deleting this object when removing
	 * Contact from Buddy which may hold last reference to it and thus wants to
	 * delete it. But we don't want this to happen.
	 */
	Contact guard(this);

	doSetOwnerBuddy(buddy, true);
	setDirty(true);

	dataUpdated();
}

void ContactShared::setContactAccount(const Account &account)
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

	if (details())
		return;

	if (!ContactAccount || ContactAccount.protocolName() != protocolFactory->name())
		return;

	setDetails(protocolFactory->createContactDetails(this));
}

void ContactShared::protocolUnregistered(ProtocolFactory *protocolFactory)
{
	Q_UNUSED(protocolFactory)
// 	protocol unregistered means auto-deleting all contact details, so we cannot set them to zero here
}

void ContactShared::detailsAdded()
{
	details()->ensureLoaded();

	dataUpdated();

	ContactManager::instance()->detailsLoaded(this);
}

void ContactShared::afterDetailsAdded()
{
	attach(false, true);
}

void ContactShared::detailsAboutToBeRemoved()
{
	// do not store contacts that are not in contact manager
	if (ContactManager::instance()->allItems().contains(uuid()))
		details()->store();

	detach(false, true);
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

	setDirty(true);
	dataUpdated();
	emit idChanged(oldId);
}

void ContactShared::setDirty(bool dirty)
{
	ensureLoaded();

	if (Dirty == dirty)
		return;

	Dirty = dirty;
	dataUpdated();
	emit dirtinessChanged();
}
