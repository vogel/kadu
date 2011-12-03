/*
 * %kadu copyright begin%
 * Copyright 2009, 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009 Bartłomiej Zimoń (uzi18@o2.pl)
 * Copyright 2009, 2009, 2010, 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "accounts/account-manager.h"
#include "accounts/account.h"
#include "avatars/avatar-manager.h"
#include "avatars/avatar.h"
#include "buddies/buddy-manager.h"
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
		Priority(-1), MaximumImageSize(0), UnreadMessagesCount(0),
		Blocking(false), Dirty(true), Port(0)
{
	ContactAccount = new Account();
	ContactAvatar = new Avatar();
	OwnerBuddy = new Buddy();
}

ContactShared::~ContactShared()
{
	ref.ref();

	triggerAllProtocolsUnregistered();

	delete OwnerBuddy;
	delete ContactAvatar;
	delete ContactAccount;
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

	*ContactAccount = AccountManager::instance()->byUuid(loadValue<QString>("Account"));

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

	setContactAvatar(AvatarManager::instance()->byUuid(loadValue<QString>("Avatar")));

	triggerAllProtocolsRegistered();
}

void ContactShared::aboutToBeRemoved()
{
	// clean up references
	*ContactAccount = Account::null;
	*OwnerBuddy = Buddy::null;

	AvatarManager::instance()->removeItem(*ContactAvatar);
	setContactAvatar(Avatar::null);

	// do not store contacts that are not in contact manager
	if (ContactManager::instance()->allItems().contains(uuid()))
		details()->ensureStored();

	detach(false, false, true);
	removeDetails();
	ContactManager::instance()->unregisterItem(this);

	dataUpdated();
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
	storeValue("Account", ContactAccount->uuid().toString());
	storeValue("Buddy", !OwnerBuddy->isAnonymous()
			? OwnerBuddy->uuid().toString()
			: QString());

	if (*ContactAvatar)
		storeValue("Avatar", ContactAvatar->uuid().toString());

	removeValue("Contact");
}

bool ContactShared::shouldStore()
{
	ensureLoaded();

	return UuidStorableObject::shouldStore() && !Id.isEmpty() && !ContactAccount->uuid().isNull();
}

void ContactShared::emitUpdated()
{
	emit updated();
}

void ContactShared::detach(bool resetBuddy, bool reattaching, bool emitSignals)
{
	if (!*OwnerBuddy)
		return;

	if (!hasDetails())
	{
		*OwnerBuddy = Buddy::null;
		return;
	}

	/* NOTE: This guard is needed to delay deleting this object when removing
	 * Contact from Buddy which holds last reference to it and thus wants to
	 * delete it. But we don't want this to happen now because we have to emit
	 * detached() signal first.
	 */
	Contact guard(this);

	Buddy oldBuddy = *OwnerBuddy;

	emitSignals = emitSignals && !oldBuddy.isAnonymous();

	if (emitSignals)
		emit aboutToBeDetached();

	oldBuddy.removeContact(this);

	// TODO This is far from ideal. In the moment OwnerBuddy emitted contactRemoved()
	// this contact still had owner buddy set. Hopefully nothing depends on correct behavior here.
	if (resetBuddy)
		*OwnerBuddy = Buddy::null;

	if (emitSignals)
		emit detached(oldBuddy, reattaching);
}

void ContactShared::attach(const Buddy &buddy, bool reattaching, bool emitSignals)
{
	if (!hasDetails() || !buddy)
	{
		*OwnerBuddy = buddy;
		return;
	}

	emitSignals = emitSignals && !buddy.isAnonymous();

	if (emitSignals)
		emit aboutToBeAttached(buddy);

	*OwnerBuddy = buddy;
	OwnerBuddy->addContact(this);

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

	bool reattaching = !OwnerBuddy->isAnonymous() && !buddy.isAnonymous();
	detach(true, reattaching, emitSignals);
	attach(buddy, reattaching, emitSignals);
}

void ContactShared::setOwnerBuddy(const Buddy &buddy)
{
	ensureLoaded();

	if (*OwnerBuddy == buddy)
		return;

	/* NOTE: This guard is needed to avoid deleting this object when removing
	 * Contact from Buddy which may hold last reference to it and thus wants to
	 * delete it. But we don't want this to happen.
	 */
	Contact guard(this);

	// don't allow empty buddy to be set, use at least anonymous one
	Buddy targetBuddy = buddy.isNull() ? Buddy::create() : buddy;
	doSetOwnerBuddy(targetBuddy, true);
	setDirty(true);
	dataUpdated();
}

void ContactShared::removeOwnerBuddy()
{
	doSetOwnerBuddy(Buddy::null, true);
}

void ContactShared::setContactAccount(const Account &account)
{
	ensureLoaded();

	if (*ContactAccount == account)
		return;

	if (*ContactAccount && ContactAccount->protocolHandler() && ContactAccount->protocolHandler()->protocolFactory())
		protocolUnregistered(ContactAccount->protocolHandler()->protocolFactory());

	*ContactAccount = account;

	if (*ContactAccount && ContactAccount->protocolHandler() && ContactAccount->protocolHandler()->protocolFactory())
		protocolRegistered(ContactAccount->protocolHandler()->protocolFactory());

	dataUpdated();
}

void ContactShared::protocolRegistered(ProtocolFactory *protocolFactory)
{
	ensureLoaded();

	if (hasDetails())
		return;

	if (!*ContactAccount || ContactAccount->protocolName() != protocolFactory->name())
		return;

	setDetails(protocolFactory->createContactDetails(this));
	details()->ensureLoaded();

	dataUpdated();

	ContactManager::instance()->registerItem(this);
	attach(*OwnerBuddy, false, true);
}

void ContactShared::protocolUnregistered(ProtocolFactory *protocolFactory)
{
	ensureLoaded();

	if (!hasDetails())
		return;

	if (!*ContactAccount || ContactAccount->protocolName() != protocolFactory->name())
		return;

	// do not store contacts that are not in contact manager
	if (ContactManager::instance()->allItems().contains(uuid()))
		details()->ensureStored();

	detach(false, false, true);
	removeDetails();
	ContactManager::instance()->unregisterItem(this);

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

/**
 * Sets state if this contact to \p dirty. All contacts are dirty by default.
 *
 * Dirty contacts with anonymous owner buddies are considered dirty removed and will
 * never be added to roster as long as this state lasts and will in effect be removed
 * from remote roster. Dirty contacts with not anonymous owner buddies are considered
 * dirty added and will always be added to roster, even if remote roster marked
 * them as removed.
 *
 * When adding contacts with anononymous owner buddies to the manager, always make sure
 * to mark them not dirty, otherwise they will be considered dirty removed and will
 * not be added to roster if remote roster says so, which is probably not what one expects.
 */
void ContactShared::setDirty(bool dirty)
{
	ensureLoaded();

	if (Dirty == dirty)
		return;

	Dirty = dirty;
	dataUpdated();
	emit dirtinessChanged();
}

void ContactShared::avatarUpdated()
{
	dataUpdated();
}

void ContactShared::setContactAvatar(const Avatar &contactAvatar)
{
	if (*ContactAvatar == contactAvatar)
		return;

	if (*ContactAvatar)
		disconnect(*ContactAvatar, SIGNAL(updated()), this, SLOT(avatarUpdated()));

	*ContactAvatar = contactAvatar;
	dataUpdated();

	if (*ContactAvatar)
		connect(*ContactAvatar, SIGNAL(updated()), this, SLOT(avatarUpdated()));
}

KaduShared_PropertyPtrReadDef(ContactShared, Account, contactAccount, ContactAccount)
KaduShared_PropertyPtrReadDef(ContactShared, Avatar, contactAvatar, ContactAvatar)
KaduShared_PropertyPtrReadDef(ContactShared, Buddy, ownerBuddy, OwnerBuddy)
