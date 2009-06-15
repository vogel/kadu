/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "accounts/account.h"
#include "configuration/xml-configuration-file.h"
#include "pending_msgs.h"

#include "contact.h"

Contact Contact::null(ContactData::TypeNull);

Contact Contact::loadFromStorage(StoragePoint* contactStoragePoint)
{
	return Contact(ContactData::loadFromStorage(contactStoragePoint));
}

Contact::Contact(ContactData *contactData)
	: Data(contactData)
{
}

Contact::Contact()
	: Data(new ContactData(ContactData::TypeNormal))
{
}

Contact::Contact(ContactData::ContactType type)
	:Data(ContactData::TypeNull != type ? new ContactData(type) : 0)
{
}

Contact::Contact(const Contact &copy)
	: Data(copy.Data)
{
}

Contact::~Contact()
{
}

void Contact::checkNull()
{
	if (isNull())
		Data = new ContactData(ContactData::TypeNull);
}

Contact & Contact::operator = (const Contact& copy)
{
	Data = copy.Data;
	return *this;
}

bool Contact::operator == (const Contact& compare) const
{
	return Data == compare.Data;
}

bool Contact::operator != (const Contact& compare) const
{
	return Data != compare.Data;
}

int Contact::operator < (const Contact& compare) const
{
	return Data.data() - compare.Data.data();
}

void Contact::importConfiguration(XmlConfigFile *configurationStorage, QDomElement parent)
{
	Data->importConfiguration(configurationStorage, parent);
}

void Contact::loadConfiguration()
{
	Data->load();
}

void Contact::store()
{
	if ((!isNull() && !isAnonymous()) || (isAnonymous() && pending.pendingMsgs(*this)))
		Data->store();
	else
		Data->removeFromStorage();
}

void Contact::removeFromStorage()
{
	Data->removeFromStorage();
}

StoragePoint * Contact::storagePointForAccountData(Account* account) const
{
	return Data->storagePointForAccountData(account);
}

StoragePoint * Contact::storagePointForModuleData(const QString& module, bool create) const
{
	return Data->storagePointForModuleData(module, create);
}

QUuid Contact::uuid() const
{
	return isNull() ? QUuid() : Data->uuid();
};

QMap<QString, QString> & Contact::customData()
{
	checkNull();
	return Data->customData();
}

Account * Contact::prefferedAccount() const
{
	return isNull()
		? 0
		: Data->prefferedAccount();
}

QList<Account *> Contact::accounts() const
{
	return isNull()
		? QList<Account *>()
		: Data->accounts();
}

void Contact::addAccountData(ContactAccountData *accountData)
{
	if (!accountData)
		return;

	checkNull();
	Data->addAccountData(accountData);
}

void Contact::removeAccountData(Account* account) const
{
	if (!isNull())
		Data->removeAccountData(account);
}

ContactAccountData * Contact::accountData(Account *account) const
{
	return isNull()
		? 0
		: Data->accountData(account);
}

QList<ContactAccountData *> Contact::accountDatas() const
{
	return isNull()
		? QList<ContactAccountData *>()
		: Data->accountDatas();
}

bool Contact::hasStoredAccountData(Account *account) const
{
	return isNull()
		? false
		: Data->hasStoredAccountData(account);
}

bool Contact::hasAccountData(Account *account) const
{
	return isNull()
		? false
		: 0 != Data->accountData(account);
}

QString Contact::id(Account *account) const
{
	return isNull()
		? QString::null
		: Data->id(account);
}

uint qHash(const Contact &contact)
{
	return qHash(contact.uuid().toString());
}

bool Contact::isIgnored() const
{
	return isNull()
		? false
		: Data->isIgnored();
}

bool Contact::setIgnored(bool ignored)
{
	return isNull()
		? false
		: Data->setIgnored(ignored);
}

bool Contact::isBlocked(Account *account) const
{
	return isNull()
		? false
		: Data->isBlocked(account);
}

bool Contact::isOfflineTo(Account *account) const
{
	return isNull()
		? false
		: Data->isOfflineTo(account);
}

void Contact::setOfflineTo(Account *account, bool offlineTo) const
{
	if (!isNull())
		Data->setOfflineTo(account, offlineTo);
}

bool Contact::isInGroup(Group *group) const
{
	return isNull()
		? false
		: Data->isInGroup(group);
}

bool Contact::showInAllGroup() const
{
	return isNull()
		? false
		: Data->showInAllGroup();
}

void Contact::addToGroup(Group *group)
{
	if (!isNull() && !Data->isInGroup(group))
		Data->addToGroup(group);

}
void Contact::removeFromGroup(Group *group)
{
	if (!isNull() && Data->isInGroup(group))
		Data->removeFromGroup(group);
}

QString Contact::display() const
{
	return isNull()
		? QString::null
		: isAnonymous() && prefferedAccount()
			? (prefferedAccount()->name() + ":" + id(prefferedAccount()))
			: Data->display().isEmpty()
				? Data->nickName().isEmpty()
					? Data->firstName()
					: Data->nickName()
				: Data->display();
}
