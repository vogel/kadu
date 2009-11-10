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
#include "buddies/buddy-manager.h"
#include "buddies/buddy-remove-predicate-object.h"
#include "contacts/contact.h"
#include "core/core.h"
#include "protocols/protocols-manager.h"
#include "icons-manager.h"

#include "buddy.h"

Buddy Buddy::null(BuddyShared::TypeNull);

Buddy Buddy::loadFromStorage(StoragePoint* contactStoragePoint)
{
	return Buddy(BuddyShared::loadFromStorage(contactStoragePoint));
}

Buddy::Buddy(BuddyShared *contactData) :
		Data(contactData)
{
}

Buddy::Buddy(BuddyShared::BuddyType type) :
		Data(BuddyShared::TypeNull != type ? new BuddyShared(type) : 0)
{
}

Buddy::Buddy(const Buddy &copy) :
		Data(copy.Data)
{
}

Buddy::~Buddy()
{
}

void Buddy::checkNull()
{
	if (isNull())
		Data = new BuddyShared(BuddyShared::TypeNull);
}

Buddy & Buddy::operator = (const Buddy& copy)
{
	Data = copy.Data;
	return *this;
}

bool Buddy::operator == (const Buddy& compare) const
{
	return Data == compare.Data;
}

bool Buddy::operator != (const Buddy& compare) const
{
	return Data != compare.Data;
}

int Buddy::operator < (const Buddy& compare) const
{
	return Data.data() - compare.Data.data();
}

void Buddy::importConfiguration(XmlConfigFile *configurationStorage, QDomElement parent)
{
	Data->importConfiguration(configurationStorage, parent);
}

void Buddy::loadConfiguration()
{
	Data->load();
}

void Buddy::store()
{
	if ((!isNull() && !isAnonymous()) || (isAnonymous() && !BuddyRemovePredicateObject::inquireAll(*this)))
		Data->store();
	else
		Data->removeFromStorage();
}

void Buddy::removeFromStorage()
{
	Data->removeFromStorage();
}

StoragePoint * Buddy::storagePointForModuleData(const QString& module, bool create) const
{
	return Data->storagePointForModuleData(module, create);
}

QUuid Buddy::uuid() const
{
	return isNull() ? QUuid() : Data->uuid();
}

QMap<QString, QString> & Buddy::customData()
{
	checkNull();
	return Data->customData();
}

Account Buddy::prefferedAccount() const
{
	return isNull()
			? Account::null
			: Data->prefferedAccount();
}

QList<Account> Buddy::accounts() const
{
	return isNull()
		? QList<Account>()
		: Data->accounts();
}

void Buddy::addContact(Contact *contact)
{
	if (!contact)
		return;

	checkNull();
	Data->addContact(contact);
}

void Buddy::removeContact(Contact *contact) const
{
	if (!isNull())
		Data->removeContact(contact);
}

void Buddy::removeContact(Account account) const
{
	if (!isNull())
		Data->removeContact(account);
}

Contact * Buddy::contact(Account account) const
{
	return isNull()
			? 0
			: Data->contact(account);
}

QList<Contact *> Buddy::contacts() const
{
	return isNull()
			? QList<Contact *>()
			: Data->contacts();
}

bool Buddy::hasContact(Account account) const
{
	return isNull()
			? false
			: 0 != Data->contact(account);
}

QString Buddy::id(Account account) const
{
	return isNull()
			? QString::null
			: Data->id(account);
}

bool Buddy::isIgnored() const
{
	return isNull()
			? false
			: Data->isIgnored();
}

bool Buddy::setIgnored(bool ignored)
{
	return isNull()
		? false
		: Data->setIgnored(ignored);
}

bool Buddy::isBlocked(Account account) const
{
	return isNull()
			? false
			: Data->isBlocked(account);
}

bool Buddy::isOfflineTo(Account account) const
{
	return isNull()
			? false
			: Data->isOfflineTo(account);
}

void Buddy::setOfflineTo(Account account, bool offlineTo) const
{
	if (!isNull())
		Data->setOfflineTo(account, offlineTo);
}

bool Buddy::isInGroup(Group *group) const
{
	return isNull()
			? false
			: Data->isInGroup(group);
}

bool Buddy::showInAllGroup() const
{
	return isNull()
			? false
			: Data->showInAllGroup();
}

void Buddy::addToGroup(Group *group)
{
	if (!isNull() && !Data->isInGroup(group))
			Data->addToGroup(group);

}
void Buddy::removeFromGroup(Group *group)
{
	if (!isNull() && Data->isInGroup(group))
		Data->removeFromGroup(group);
}

QString Buddy::display() const
{
	return isNull()
			? QString::null
			: isAnonymous() && !prefferedAccount().isNull()
					? (prefferedAccount().name() + ":" + id(prefferedAccount()))
					: Data->display().isEmpty()
							? Data->nickName().isEmpty()
									? Data->firstName()
									: Data->nickName()
							: Data->display();
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

	Contact *contactData = new Contact(account, example, "999999", true);
	contactData->setStatus(Status("Away", tr("Example description")));
	contactData->setIp(QHostAddress(2130706433));
	contactData->setPort(80);
	Avatar &avatar = contactData->avatar();
	avatar.setLastUpdated(QDateTime::currentDateTime());
	avatar.setPixmap(IconsManager::instance()->loadPixmap("ContactsTab"));
	avatar.setFileName(IconsManager::instance()->iconPath("ContactsTab"));

	example.addContact(contactData);

	return example;
}

uint qHash(const Buddy &buddy)
{
	return qHash(buddy.uuid().toString());
}
