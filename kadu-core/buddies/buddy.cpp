/*
 * %kadu copyright begin%
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2008, 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2009, 2009 Bartłomiej Zimoń (uzi18@o2.pl)
 * Copyright 2008, 2009, 2010 Piotr Galiszewski (piotrgaliszewski@gmail.com)
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

#include "accounts/account.h"
#include "accounts/account-details.h"
#include "accounts/account-manager.h"
#include "avatars/avatar.h"
#include "avatars/avatar-manager.h"
#include "avatars/avatar-shared.h"
#include "buddies/buddy-manager.h"
#include "buddies/buddy-remove-predicate-object.h"
#include "buddies/buddy-shared.h"
#include "configuration/configuration-manager.h"
#include "configuration/xml-configuration-file.h"
#include "contacts/contact.h"
#include "contacts/contact-shared.h"
#include "core/core.h"
#include "protocols/protocol.h"
#include "protocols/protocols-manager.h"
#include "icons-manager.h"

#include "buddy.h"

KaduSharedBaseClassImpl(Buddy)

Buddy Buddy::null;

Buddy Buddy::create()
{
	return new BuddyShared();
}

Buddy Buddy::loadStubFromStorage(const QSharedPointer<StoragePoint> &contactStoragePoint)
{
	return BuddyShared::loadStubFromStorage(contactStoragePoint);
}

Buddy Buddy::loadFromStorage(const QSharedPointer<StoragePoint> &contactStoragePoint)
{
	return BuddyShared::loadFromStorage(contactStoragePoint);
}

Buddy::Buddy()
{
}

Buddy::Buddy(BuddyShared *data) :
		SharedBase<BuddyShared>(data)
{
	data->ref.ref();
}

Buddy::Buddy(QObject *data)
{
	BuddyShared *shared = dynamic_cast<BuddyShared *>(data);
	if (shared)
		setData(shared);
}

Buddy::Buddy(const Buddy &copy) :
		SharedBase<BuddyShared>(copy)
{
}

Buddy::~Buddy()
{
}

void Buddy::importConfiguration(const QDomElement &parent)
{
	if (data())
		data()->importConfiguration(parent);
}

void Buddy::store()
{
	if ((!isNull() && !isAnonymous()) || (isAnonymous() && !BuddyRemovePredicateObject::inquireAll(*this)))
		data()->store();
	else
		data()->removeFromStorage();
}

QSharedPointer<StoragePoint> Buddy::storagePointForModuleData(const QString& module, bool create) const
{
	return data()->storagePointForModuleData(module, create);
}

QString Buddy::customData(const QString &key)
{
	return isNull() ? QString() : data()->customData()[key];
}

void Buddy::setCustomData(const QString &key, const QString &value)
{
	if (!isNull())
		data()->customData().insert(key, value);
}

void Buddy::removeCustomData(const QString &key)
{
	if (!isNull())
		data()->customData().remove(key);
}

void Buddy::sortContacts()
{
	if (!isNull())
		data()->sortContacts();
}

void Buddy::normalizePriorities()
{
	if (!isNull())
		data()->normalizePriorities();
}

void Buddy::addContact(Contact contact)
{
	if (isNull() || contact.isNull())
		return;

	data()->addContact(contact);
}

void Buddy::removeContact(Contact contact) const
{
	if (!isNull())
		data()->removeContact(contact);
}

QList<Contact> Buddy::contacts(Account account) const
{
	return isNull() ? QList<Contact>() : data()->contacts(account);
}

QList<Contact> Buddy::contacts() const
{
	return isNull() ? QList<Contact>() : data()->contacts();
}

bool Buddy::hasContact(Account account) const
{
	return isNull() ? false : data()->contacts(account).count() > 0;
}

QString Buddy::id(Account account) const
{
	return isNull() ? QString() : data()->id(account);
}

bool Buddy::isInGroup(Group group) const
{
	return isNull() ? false : data()->isInGroup(group);
}

bool Buddy::showInAllGroup() const
{
	return isNull() ? false : data()->showInAllGroup();
}

void Buddy::addToGroup(Group group)
{
	if (!isNull() && !data()->isInGroup(group))
		data()->addToGroup(group);

}
void Buddy::removeFromGroup(Group group)
{
	if (!isNull() && data()->isInGroup(group))
		data()->removeFromGroup(group);
}

bool Buddy::isEmpty() const
{
	if (isNull())
		return true;
	else
		return data()->isEmpty();
}

QString Buddy::display() const
{
	if (isNull())
		return QString();

	QString result = data()->display().isEmpty()
			? data()->nickName().isEmpty()
					? data()->firstName()
					: data()->nickName()
			: data()->display();

	if (result.isEmpty())
	{
		if (!data()->contacts().isEmpty())
		{
			Contact contact = data()->contacts().first();
			if (contact)
			{
				Account account = contact.contactAccount();
				if (account)
					result = account.accountIdentity().name() + ':' + contact.id();
			}
		}
	}

	return result;
}

Buddy Buddy::dummy()
{
	Buddy example = Buddy::create();

	example.setFirstName("Mark");
	example.setLastName("Smith");
	example.setNickName("Jimbo");
	example.setDisplay("Jimbo");
	example.setMobile("+48123456789");
	example.setEmail("jimbo@mail.server.net");
	example.setHomePhone("+481234567890");

	Account account;

	if (!AccountManager::instance()->defaultAccount().isNull())
		account = AccountManager::instance()->defaultAccount();
	else if (ProtocolsManager::instance()->protocolFactories().count())
	{
		account = Account::create();
		ProtocolFactory *firstProto = ProtocolsManager::instance()->protocolFactories().first() ;
		account.setProtocolName(firstProto->name());
		account.details()->setState(StorableObject::StateNew);
	}

	if (!account.isNull())
	{
		Contact contact = Contact::create();
		contact.setContactAccount(account);
		contact.setOwnerBuddy(example);
		contact.setId("999999");
		contact.setCurrentStatus(Status("Away", example.data()->tr("Example description")));
		contact.setAddress(QHostAddress(2130706433));
		contact.setPort(80);
		contact.setDetails(account.protocolHandler()->protocolFactory()->createContactDetails(contact));

		// this is just an example contact, do not add avatar to list
		Avatar avatar = AvatarManager::instance()->byContact(contact, ActionCreate);

		avatar.setLastUpdated(QDateTime::currentDateTime());
		avatar.setFilePath(IconsManager::instance()->iconPath("system-users", "64x64"));

		example.addContact(contact);
		example.setAnonymous(false);

		return example;
	}

	return null;
}

KaduSharedBase_PropertyDef(Buddy, Avatar, buddyAvatar, BuddyAvatar, Avatar::null)
KaduSharedBase_PropertyWriteDef(Buddy, QString, display, Display)
KaduSharedBase_PropertyDef(Buddy, QString, firstName, FirstName, QString())
KaduSharedBase_PropertyDef(Buddy, QString, lastName, LastName, QString())
KaduSharedBase_PropertyDef(Buddy, QString, familyName, FamilyName, QString())
KaduSharedBase_PropertyDef(Buddy, QString, city, City, QString())
KaduSharedBase_PropertyDef(Buddy, QString, familyCity, FamilyCity, QString())
KaduSharedBase_PropertyDef(Buddy, QString, nickName, NickName, QString())
KaduSharedBase_PropertyDef(Buddy, QString, homePhone, HomePhone, QString())
KaduSharedBase_PropertyDef(Buddy, QString, mobile, Mobile, QString())
KaduSharedBase_PropertyDef(Buddy, QString, email, Email, QString())
KaduSharedBase_PropertyDef(Buddy, QString, website, Website, QString())
KaduSharedBase_PropertyDef(Buddy, unsigned short, birthYear, BirthYear, 0)
KaduSharedBase_PropertyDef(Buddy, BuddyGender, gender, Gender, GenderUnknown)
KaduSharedBase_PropertyDef(Buddy, QList<Group>, groups, Groups, QList<Group>())
KaduSharedBase_PropertyBoolDef(Buddy, Anonymous, true)
KaduSharedBase_PropertyBoolDef(Buddy, Blocked, false)
KaduSharedBase_PropertyBoolDef(Buddy, OfflineTo, false)
