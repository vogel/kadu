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
#include "core/core.h"
#include "protocols/protocol.h"
#include "protocols/protocol-factory.h"
#include "protocols/protocols-manager.h"

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
		Shared(uuid), Details(0),
		Priority(-1), MaximumImageSize(0), UnreadMessagesCount(0),
		Blocking(false), Dirty(true), Port(0)
{
	ContactAccount = new Account();
	ContactAvatar = new Avatar();
	OwnerBuddy = new Buddy();

	connect(ProtocolsManager::instance(), SIGNAL(protocolFactoryRegistered(ProtocolFactory*)),
	        this, SLOT(protocolFactoryRegistered(ProtocolFactory*)));
	connect(ProtocolsManager::instance(), SIGNAL(protocolFactoryUnregistered(ProtocolFactory*)),
	        this, SLOT(protocolFactoryUnregistered(ProtocolFactory*)));
}

ContactShared::~ContactShared()
{
	ref.ref();

	disconnect(ProtocolsManager::instance(), SIGNAL(protocolFactoryRegistered(ProtocolFactory*)),
	           this, SLOT(protocolFactoryRegistered(ProtocolFactory*)));
	disconnect(ProtocolsManager::instance(), SIGNAL(protocolFactoryUnregistered(ProtocolFactory*)),
	           this, SLOT(protocolFactoryUnregistered(ProtocolFactory*)));

	protocolFactoryUnregistered(ProtocolsManager::instance()->byName(ContactAccount->protocolName()));

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

	doSetContactAvatar(AvatarManager::instance()->byUuid(loadValue<QString>("Avatar")));

	protocolFactoryRegistered(ProtocolsManager::instance()->byName(ContactAccount->protocolName()));
}

void ContactShared::aboutToBeRemoved()
{
	// clean up references
	*ContactAccount = Account::null;
	*OwnerBuddy = Buddy::null;

	AvatarManager::instance()->removeItem(*ContactAvatar);
	doSetContactAvatar(Avatar::null);

	deleteDetails();

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
	storeValue("Buddy", !isAnonymous()
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

	if (!Details)
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
	if (!Details || !buddy)
	{
		*OwnerBuddy = buddy;
		return;
	}

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

	bool reattaching = !isAnonymous() && !buddy.isAnonymous();
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
		protocolFactoryUnregistered(ContactAccount->protocolHandler()->protocolFactory());

	*ContactAccount = account;

	if (*ContactAccount && ContactAccount->protocolHandler() && ContactAccount->protocolHandler()->protocolFactory())
		protocolFactoryRegistered(ContactAccount->protocolHandler()->protocolFactory());

	dataUpdated();
}

void ContactShared::protocolFactoryRegistered(ProtocolFactory *protocolFactory)
{
	ensureLoaded();

	if (!protocolFactory || !*ContactAccount || ContactAccount->protocolName() != protocolFactory->name())
		return;

	if (Details)
		return;

	Details = protocolFactory->createContactDetails(this);
	Q_ASSERT(Details);

	Details->ensureLoaded();

	dataUpdated();

	ContactManager::instance()->registerItem(this);
	attach(*OwnerBuddy, false, true);
}

void ContactShared::protocolFactoryUnregistered(ProtocolFactory *protocolFactory)
{
	ensureLoaded();

	if (!protocolFactory || ContactAccount->protocolName() != protocolFactory->name())
		return;

	/* NOTE: This guard is needed to avoid deleting this object when detaching
	 * Contact from Buddy which may hold last reference to it and thus wants to
	 * delete it. But we don't want this to happen.
	 */
	Contact guard(this);

	deleteDetails();

	dataUpdated();
}

void ContactShared::deleteDetails()
{
	if (Details)
	{
		// do not store contacts that are not in contact manager
		if (ContactManager::instance()->allItems().contains(uuid()))
			Details->ensureStored();

		detach(false, false, true);
		delete Details;
		Details = 0;
	}

	ContactManager::instance()->unregisterItem(this);
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

void ContactShared::doSetContactAvatar(const Avatar &contactAvatar)
{
	if (*ContactAvatar)
		disconnect(*ContactAvatar, SIGNAL(updated()), this, SLOT(avatarUpdated()));

	*ContactAvatar = contactAvatar;

	if (*ContactAvatar)
		connect(*ContactAvatar, SIGNAL(updated()), this, SLOT(avatarUpdated()));
}

void ContactShared::setContactAvatar(const Avatar &contactAvatar)
{
	ensureLoaded();

	if (*ContactAvatar == contactAvatar)
		return;

	doSetContactAvatar(contactAvatar);
	dataUpdated();
}

bool ContactShared::isAnonymous()
{
	ensureLoaded();

	if (!OwnerBuddy)
		return true;

	if (!(*OwnerBuddy))
		return true;

	return OwnerBuddy->isAnonymous();
}

QString ContactShared::display(bool useBuddyData)
{
	ensureLoaded();

	if (!useBuddyData || !OwnerBuddy || !(*OwnerBuddy) || OwnerBuddy->display().isEmpty())
		return Id;

	return OwnerBuddy->display();
}

Avatar ContactShared::avatar(bool useBuddyData)
{
	ensureLoaded();

	if (!useBuddyData || !OwnerBuddy || !(*OwnerBuddy) || OwnerBuddy->buddyAvatar().isEmpty())
		return ContactAvatar ? *ContactAvatar : Avatar::null;

	return OwnerBuddy->buddyAvatar();
}

KaduShared_PropertyPtrReadDef(ContactShared, Account, contactAccount, ContactAccount)
KaduShared_PropertyPtrReadDef(ContactShared, Avatar, contactAvatar, ContactAvatar)
KaduShared_PropertyPtrReadDef(ContactShared, Buddy, ownerBuddy, OwnerBuddy)
