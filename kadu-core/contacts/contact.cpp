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
#include "contacts/avatar.h"
#include "contacts/contact-account-data.h"
#include "contacts/contact-manager.h"
#include "contacts/contact-remove-predicate-object.h"
#include "core/core.h"
#include "protocols/protocols-manager.h"
#include "icons-manager.h"

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
	if ((!isNull() && !isAnonymous()) || (isAnonymous() && !ContactRemovePredicateObject::inquireAll(*this)))
		Data->store();
	else
		Data->removeFromStorage();
}

void Contact::removeFromStorage()
{
	Data->removeFromStorage();
}

StoragePoint * Contact::storagePointForModuleData(const QString& module, bool create) const
{
	return Data->storagePointForModuleData(module, create);
}

QUuid Contact::uuid() const
{
	return isNull() ? QUuid() : Data->uuid();
}

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

StoragePoint * Contact::storagePointForAccountData(Account *account) const
{
	return isNull()
		? 0
		: Data->storagePointForAccountData(account);
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

Contact Contact::dummy()
{
    	Contact example;
	example.setFirstName("Mark");
	example.setLastName("Smith");
	example.setNickName("Jimbo");
	example.setDisplay("Jimbo");
	example.setMobile("+48123456789");
	example.setEmail("jimbo@mail.server.net");
	example.setHomePhone("+481234567890");
	Account *account = 0;
	if (AccountManager::instance()->defaultAccount())
		account = AccountManager::instance()->defaultAccount();
	else if (ProtocolsManager::instance()->protocolFactories().count())
		account = ProtocolsManager::instance()->protocolFactories()[0]->newAccount();

	if (account)
	{
		ContactAccountData *contactData = new ContactAccountData(account, example, "999999");
		contactData->setStatus(Status("Away", tr("Example description")));
		contactData->setIp(QHostAddress(2130706433));
		contactData->setPort(80);
		Avatar &avatar = contactData->avatar();
		avatar.setLastUpdated(QDateTime::currentDateTime());
		avatar.setPixmap(IconsManager::instance()->loadPixmap("ContactsTab"));
		avatar.setFileName(IconsManager::instance()->iconPath("ContactsTab"));

		example.addAccountData(contactData);

		return example;
	}
	return null;
}

void Contact::mergeWith(Contact contact)
{
	printf("merging %s with %s\n", qPrintable(display()), qPrintable(contact.display()));

	QList<Account *> myAccounts = accounts();
	foreach (Account *account, myAccounts)
	{
		printf("moving data for accoun: %s\n", qPrintable(account->name()));

		ContactAccountData* cad = accountData(account);
		removeAccountData(account);
		contact.addAccountData(cad);
		cad->setContact(contact);

		cad->recreateStoragePoint();
	}
	
	ContactManager::instance()->removeContact(*this);
	Data->setUuid(contact.uuid()); // just for case
	Data = contact.data(); // TODO: 0.8 tricky merge, this should work well ;)

	printf("merge done\n");

	Core::instance()->configuration()->flush();
}
