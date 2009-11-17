/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "accounts/account.h"
#include "accounts/account-manager.h"
#include "configuration/configuration-manager.h"
#include "configuration/xml-configuration-file.h"
#include "buddies/avatar.h"
#include "buddies/avatar-shared.h"
#include "buddies/buddy-manager.h"
#include "buddies/buddy-remove-predicate-object.h"
#include "contacts/contact.h"
#include "contacts/contact-shared.h"
#include "core/core.h"
#include "protocols/protocol.h"
#include "protocols/protocols-manager.h"
#include "icons-manager.h"

#include "buddy.h"

Buddy Buddy::null(true);

Buddy Buddy::loadFromStorage(StoragePoint* contactStoragePoint)
{
	return Buddy(BuddyShared::loadFromStorage(contactStoragePoint));
}

Buddy::Buddy(bool null) :
		SharedBase<BuddyShared>(null)
{
}

Buddy::Buddy()
{
	data()->setState(StorableObject::StateNew);
}

Buddy::Buddy(BuddyShared *data) :
		SharedBase<BuddyShared>(data)
{
}

Buddy::Buddy(const Buddy &copy) :
		SharedBase<BuddyShared>(copy)
{
}

Buddy::~Buddy()
{
}

void Buddy::importConfiguration(XmlConfigFile *configurationStorage, QDomElement parent)
{
	data()->importConfiguration(configurationStorage, parent);
}

void Buddy::store()
{
	if ((!isNull() && !isAnonymous()) || (isAnonymous() && !BuddyRemovePredicateObject::inquireAll(*this)))
		data()->store();
	else
		data()->removeFromStorage();
}

StoragePoint * Buddy::storagePointForModuleData(const QString& module, bool create) const
{
	return data()->storagePointForModuleData(module, create);
}

QString Buddy::customData(const QString &key)
{
	return isNull() ? QString::null : data()->customData()[key];
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

Account Buddy::prefferedAccount() const
{
	return isNull() ? Account::null : data()->prefferedAccount();
}

QList<Account> Buddy::accounts() const
{
	return isNull() ? QList<Account>() : data()->accounts();
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

void Buddy::removeContact(Account account) const
{
	if (!isNull())
		data()->removeContact(account);
}

Contact Buddy::contact(Account account) const
{
	return isNull() ? Contact::null : data()->contact(account);
}

QList<Contact> Buddy::contacts() const
{
	return isNull() ? QList<Contact>() : data()->contacts();
}

bool Buddy::hasContact(Account account) const
{
	return isNull() ? false : !data()->contact(account).isNull();
}

QString Buddy::id(Account account) const
{
	return isNull() ? QString::null : data()->id(account);
}

bool Buddy::isInGroup(Group *group) const
{
	return isNull() ? false : data()->isInGroup(group);
}

bool Buddy::showInAllGroup() const
{
	return isNull() ? false : data()->showInAllGroup();
}

void Buddy::addToGroup(Group *group)
{
	if (!isNull() && !data()->isInGroup(group))
		data()->addToGroup(group);

}
void Buddy::removeFromGroup(Group *group)
{
	if (!isNull() && data()->isInGroup(group))
		data()->removeFromGroup(group);
}

QString Buddy::display() const
{
	return isNull()
			? QString::null
			: isAnonymous() && !prefferedAccount().isNull()
					? (prefferedAccount().name() + ":" + id(prefferedAccount()))
					: data()->display().isEmpty()
							? data()->nickName().isEmpty()
									? data()->firstName()
									: data()->nickName()
							: data()->display();
}

Buddy Buddy::dummy()
{
	Buddy example;

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
		account.data()->setState(StorableObject::StateNew);
		account.setProtocolName(ProtocolsManager::instance()->protocolFactories()[0]->name());
		account.data()->protocolRegistered(ProtocolsManager::instance()->protocolFactories()[0]);
		account.setDetails(ProtocolsManager::instance()->protocolFactories()[0]->createAccountDetails(account));
	}

	if (!account.isNull())
	{
		Contact contactData;
		contactData.setContactAccount(account);
		contactData.setOwnerBuddy(example);
		contactData.setId("999999");
		contactData.data()->setState(StorableObject::StateNew);
		contactData.setCurrentStatus(Status("Away", tr("Example description")));
		contactData.setAddress(QHostAddress(2130706433));
		contactData.setPort(80);
		contactData.setDetails(account.protocolHandler()->protocolFactory()->createContactDetails(contactData));

		Avatar avatar;
		avatar.data()->setState(StorableObject::StateNew);
		avatar.setLastUpdated(QDateTime::currentDateTime());
		avatar.setPixmap(IconsManager::instance()->loadPixmap("ContactsTab"));
		avatar.setFileName("ContactsTab");
		contactData.setContactAvatar(avatar);

		example.addContact(contactData);

		return example;
	}
	return null;
}

KaduSharedBase_PropertyWriteDef(Buddy, QString, display, Display, QString::null)
KaduSharedBase_PropertyDef(Buddy, QString, firstName, FirstName, QString::null)
KaduSharedBase_PropertyDef(Buddy, QString, lastName, LastName, QString::null)
KaduSharedBase_PropertyDef(Buddy, QString, familyName, FamilyName, QString::null)
KaduSharedBase_PropertyDef(Buddy, QString, city, City, QString::null)
KaduSharedBase_PropertyDef(Buddy, QString, familyCity, FamilyCity, QString::null)
KaduSharedBase_PropertyDef(Buddy, QString, nickName, NickName, QString::null)
KaduSharedBase_PropertyDef(Buddy, QString, homePhone, HomePhone, QString::null)
KaduSharedBase_PropertyDef(Buddy, QString, mobile, Mobile, QString::null)
KaduSharedBase_PropertyDef(Buddy, QString, email, Email, QString::null)
KaduSharedBase_PropertyDef(Buddy, QString, website, Website, QString::null)
KaduSharedBase_PropertyDef(Buddy, unsigned short, birthYear, BirthYear, 0)
KaduSharedBase_PropertyDef(Buddy, BuddyShared::BuddyGender, gender, Gender, BuddyShared::GenderUnknown)
KaduSharedBase_PropertyDef(Buddy, QList<Group *>, groups, Groups, QList<Group *>())
KaduSharedBase_PropertyBoolDef(Buddy, Anonymous, false)
KaduSharedBase_PropertyBoolDef(Buddy, Ignored, false)
KaduSharedBase_PropertyBoolDef(Buddy, Blocked, false)
KaduSharedBase_PropertyBoolDef(Buddy, OfflineTo, false)
