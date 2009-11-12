/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "accounts/account-manager.h"
#include "contacts/contact.h"
#include "misc/misc.h"
#include "protocols/protocol.h"

#include "identity-shared.h"

IdentityShared * IdentityShared::loadFromStorage(StoragePoint *storagePoint)
{
	IdentityShared *identityShared = new IdentityShared(storagePoint);
	identityShared->load();

	return identityShared;
}

IdentityShared::IdentityShared(StoragePoint *storagePoint) :
		BaseStatusContainer(storagePoint)
{
	connect(AccountManager::instance(), SIGNAL(accountRemoved(Account*)), this, SLOT(removeAccount(Account*)));
}

IdentityShared::IdentityShared(const QUuid &uuid) :
		BaseStatusContainer("Identity", IdentityManager::instance()),
		Uuid(uuid.isNull() ? QUuid::createUuid() : uuid)
{
    	connect(AccountManager::instance(), SIGNAL(accountRemoved(Account*)), this, SLOT(removeAccount(Account*)));
}

IdentityShared::~IdentityShared()
{
	disconnect(AccountManager::instance(), SIGNAL(accountRemoved(Account*)), this, SLOT(removeAccount(Account*)));
}

void IdentityShared::load()
{
	StorableObject::load();

	if (!isValidStorage())
		return;

	Uuid = QUuid(storage()->point().attribute("uuid"));
	Name = loadValue<QString>("Name");

	XmlConfigFile *configurationStorage = storage()->storage();

	QDomElement accountsNode = configurationStorage->getNode(storage()->point(), "Accounts", XmlConfigFile::ModeFind);
	if (!accountsNode.isNull())
	{
		QDomNodeList accountsList = accountsNode.elementsByTagName("Account");

		int count = accountsList.count();
		for (int i = 0; i < count; i++)
		{
			QDomElement accountElement = accountsList.at(i).toElement();
			if (accountElement.isNull())
				continue;

			AccountsUuids << accountElement.text();
		}
	}

	triggerAllAccountsRegistered();
}

void IdentityShared::store()
{
	if (!isValidStorage())
		return;

	storage()->point().setAttribute("uuid", Uuid.toString());

	storeValue("Name", Name);

	XmlConfigFile *configurationStorage = storage()->storage();

	if (AccountsUuids.count())
	{
		QDomElement accountsNode = configurationStorage->getNode(storage()->point(), "Accounts", XmlConfigFile::ModeCreate);
		foreach (const QString &account, AccountsUuids)
			configurationStorage->appendTextNode(accountsNode, "Account", account);
	}
	else
		configurationStorage->removeNode(storage()->point(), "Accounts");
}

void IdentityShared::accountRegistered(Account account)
{
	if (AccountsUuids.contains(account.uuid()))
		Accounts << account;
}

void IdentityShared::accountUnregistered(Account account)
{
	Accounts.removeAll(account);
}

void IdentityShared::addAccount(Account account)
{
	Accounts << account;
	AccountsUuids << account.uuid();
}

void IdentityShared::setStatus(Status status)
{
	foreach (Account account, Accounts)
		account.statusContainer()->setStatus(status);
}

const Status & IdentityShared::status()
{
	return Accounts.count()
			? Accounts[0].statusContainer()->status()
			: Status::null;
}

QString IdentityShared::statusName()
{
	return Status::name(status(), false);
}

QPixmap IdentityShared::statusPixmap()
{
	return statusPixmap(status().type());
}

QPixmap IdentityShared::statusPixmap(Status status)
{
	return statusPixmap(status.type());
}

QPixmap IdentityShared::statusPixmap(const QString &statusType)
{
	return Accounts.count()
			? Accounts[0].statusContainer()->statusPixmap(statusType)
			: QPixmap();
}

QList<StatusType *> IdentityShared::supportedStatusTypes()
{
	return Accounts.count()
			? Accounts[0].statusContainer()->supportedStatusTypes()
			: QList<StatusType *>();
}

int IdentityShared::maxDescriptionLength()
{
	return Accounts.count()
			? Accounts[0].statusContainer()->maxDescriptionLength()
			: -1;
}

void IdentityShared::setPrivateStatus(bool isPrivate)
{
	foreach (Account account, Accounts)
		account.statusContainer()->setPrivateStatus(isPrivate);
}
