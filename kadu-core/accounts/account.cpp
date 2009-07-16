/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "accounts/account-manager.h"
#include "configuration/xml-configuration-file.h"
#include "contacts/contact-account-data.h"
#include "contacts/contact-manager.h"
#include "protocols/protocol.h"
#include "protocols/protocol-factory.h"
#include "protocols/protocols-manager.h"
#include "misc/misc.h"

#include "account.h"

Account::Account(const QUuid &uuid) :
		UuidStorableObject("Account", AccountManager::instance()), ProtocolHandler(0),
		RememberPassword(false), HasPassword(false), UseProxy(false), ProxyHost(QHostAddress()),
		ProxyPort(0), ProxyUser(QString()), ProxyPassword(QString())
{
	Uuid = uuid.isNull()
		? QUuid::createUuid()
		: uuid;
}

Account::~Account()
{
	if (0 != ProtocolHandler)
	{
		delete ProtocolHandler;
		ProtocolHandler = 0;
	}
}

void Account::setProtocol(Protocol *protocolHandler)
{
	ProtocolHandler = protocolHandler;

	connect(ProtocolHandler, SIGNAL(statusChanged(Account *, Status)), this, SIGNAL(statusChanged()));
	connect(ProtocolHandler, SIGNAL(contactStatusChanged(Account *, Contact, Status)),
			this, SIGNAL(contactStatusChanged(Account *, Contact, Status)));
}

bool Account::setId(const QString &id)
{
	// TODO: 0.6.6 reconnect
	Id = id;
	return true;
}

void Account::contactAdded(Contact contact)
{
	ContactAccountData *cad = ProtocolHandler->protocolFactory()->loadContactAccountData(contact, this);
	if (cad)
		contact.addAccountData(cad);
}

void Account::contactRemoved(Contact contact)
{
}

void Account::load()
{
	if (!isValidStorage())
		return;

	ConnectAtStart = loadValue<bool>("ConnectAtStart", true);

	Uuid = QUuid(storage()->point().attribute("uuid"));
	Name = loadValue<QString>("Name");
	setId(loadValue<QString>("Id"));

	RememberPassword = loadValue<bool>("RememberPassword", true);
	HasPassword = RememberPassword;
	if (RememberPassword)
		Password = pwHash(loadValue<QString>("Password"));

	UseProxy = loadValue<bool>("UseProxy");
	ProxyPort = loadValue<int>("ProxyPort");
	ProxyReqAuthentication = loadValue<bool>("ProxyRequiresAuthentication");
	ProxyUser = loadValue<QString>("ProxyUser");
	ProxyPassword = loadValue<QString>("ProxyPassword");

	QHostAddress host;
	if (!host.setAddress(loadValue<QString>("ProxyHost")))
		host.setAddress("0.0.0.0");
	ProxyHost = host;

	triggerAllContactsAdded();
}

void Account::store()
{
	if (!isValidStorage())
		return;

	storage()->point().setAttribute("uuid", Uuid.toString());

	storeValue("ConnectAtStart", ConnectAtStart);

	storeValue("Protocol", ProtocolHandler->protocolFactory()->name());
	storeValue("Name", Name);
	storeValue("Id", id());

	storeValue("RememberPassword", RememberPassword);
	if (RememberPassword && HasPassword)
		storeValue("Password", pwHash(password()));
	else
		removeValue("Password");

	storeValue("UseProxy", UseProxy);
	storeValue("ProxyPort", ProxyPort);
	storeValue("ProxyRequiresAuthentication", ProxyReqAuthentication);
	storeValue("ProxyUser", ProxyUser);
	storeValue("ProxyPassword", ProxyPassword);
	storeValue("ProxyHost", ProxyHost.toString());
}

Contact Account::getContactById(const QString& id)
{
	return ContactManager::instance()->byId(this, id);
}

Contact Account::createAnonymous(const QString& id)
{
	Contact result(ContactData::TypeAnonymous);
	ProtocolFactory *protocolFactory = ProtocolHandler->protocolFactory();
	ContactAccountData *contactAccountData = protocolFactory->newContactAccountData(result, this, id, false);
	if (!contactAccountData->isValid())
	{
		delete contactAccountData;
		return Contact::null;
	}

	result.addAccountData(contactAccountData);
	return result;
}

void Account::importProxySettings()
{
	Account *defaultAccount = AccountManager::instance()->defaultAccount();
	if (defaultAccount && defaultAccount->proxyHost().toString() != "0.0.0.0")
	{
		UseProxy = defaultAccount->useProxy();
		ProxyPort = defaultAccount->proxyPort();
		ProxyUser = defaultAccount->proxyUser();
		ProxyReqAuthentication = defaultAccount->proxyReqAuthentication();
		ProxyPassword = defaultAccount->proxyPassword();
		ProxyHost = defaultAccount->proxyHost();
	}
}

QString Account::statusContainerName()
{
	return name();
}

void Account::setStatus(Status status)
{
	if (0 != ProtocolHandler)
		ProtocolHandler->setStatus(status);
}

Status Account::status()
{
	return 0 != ProtocolHandler
		? ProtocolHandler->status()
		: Status();
}

QString Account::statusName()
{
	return Status::name(status(), false);
}

QPixmap Account::statusPixmap()
{
	return statusPixmap(status());
}

QPixmap Account::statusPixmap(const QString &statusType)
{
	return ProtocolHandler->statusPixmap(statusType);
}

QPixmap Account::statusPixmap(Status status)
{
	return ProtocolHandler->statusPixmap(status);
}

QList<StatusType *> Account::supportedStatusTypes()
{
	return ProtocolHandler->protocolFactory()->supportedStatusTypes();
}
