/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "accounts/account-details.h"
#include "accounts/account-manager.h"
#include "contacts/contact.h"
#include "misc/misc.h"
#include "protocols/protocol.h"

#include "account-shared.h"

AccountShared * AccountShared::loadFromStorage(StoragePoint *storagePoint)
{
	AccountShared *result = new AccountShared();
	result->setStorage(storagePoint);
	result->load();

	return result;
}

AccountShared::AccountShared(QUuid uuid) :
		Shared(uuid, "Account", AccountManager::instance()),
		BaseStatusContainer(this),
		ProtocolHandler(0), Details(0),
		RememberPassword(false), HasPassword(false),
		ConnectAtStart(true),
		UseProxy(false), ProxyPort(0)
{
}

AccountShared::~AccountShared()
{
	triggerAllProtocolsUnregistered();

	if (ProtocolHandler)
	{
		delete ProtocolHandler;
		ProtocolHandler = 0;
	}
}

void AccountShared::load()
{
	if (!isValidStorage())
		return;

	Shared::load();

	ConnectAtStart = loadValue<bool>("ConnectAtStart", true);
	
	Name = loadValue<QString>("Name");
	ProtocolName = loadValue<QString>("Protocol");
	setId(loadValue<QString>("Id"));

	RememberPassword = loadValue<bool>("RememberPassword", true);
	HasPassword = RememberPassword;
	if (RememberPassword)
		Password = pwHash(loadValue<QString>("Password"));

	UseProxy = loadValue<bool>("UseProxy");
	ProxyPort = loadValue<int>("ProxyPort");
	ProxyRequiresAuthentication = loadValue<bool>("ProxyRequiresAuthentication");
	ProxyUser = loadValue<QString>("ProxyUser");
	ProxyPassword = loadValue<QString>("ProxyPassword");

	QHostAddress host;
	if (!host.setAddress(loadValue<QString>("ProxyHost")))
		host.setAddress("0.0.0.0");
	ProxyHost = host;

	triggerAllProtocolsRegistered();
}

void AccountShared::store()
{
	if (!isValidStorage())
		return;

	Shared::store();

	storeValue("ConnectAtStart", ConnectAtStart);

	storeValue("Protocol", ProtocolName);
	storeValue("Name", Name);
	storeValue("Id", id());

	storeValue("RememberPassword", RememberPassword);
	if (RememberPassword && HasPassword)
		storeValue("Password", pwHash(password()));
	else
		removeValue("Password");

	storeValue("UseProxy", UseProxy);
	storeValue("ProxyPort", ProxyPort);
	storeValue("ProxyRequiresAuthentication", ProxyRequiresAuthentication);
	storeValue("ProxyUser", ProxyUser);
	storeValue("ProxyPassword", ProxyPassword);
	storeValue("ProxyHost", ProxyHost.toString());
}

void AccountShared::emitUpdated()
{
	emit updated();
}

void AccountShared::protocolRegistered(ProtocolFactory *factory)
{
	ensureLoaded();

	if (factory->name() != ProtocolName)
		return;

	ProtocolHandler = factory->createProtocolHandler(this);
	Details = factory->createAccountDetails(this);

	connect(ProtocolHandler, SIGNAL(statusChanged(Account, Status)), this, SIGNAL(statusChanged()));
	connect(ProtocolHandler, SIGNAL(buddyStatusChanged(Account, Buddy, Status)),
			this, SIGNAL(buddyStatusChanged(Account, Buddy, Status)));

	emit protocolLoaded();
}

void AccountShared::protocolUnregistered(ProtocolFactory* factory)
{
	ensureLoaded();

	if (factory->name() != ProtocolName)
		return;
	
	emit protocolUnloaded();

	delete ProtocolHandler;
	ProtocolHandler = 0;

	delete Details;
	Details = 0;
}

QString AccountShared::statusContainerName()
{
	return name();
}

void AccountShared::setStatus(Status status)
{
	if (ProtocolHandler)
		ProtocolHandler->setStatus(status);
}

const Status & AccountShared::status()
{
	if (ProtocolHandler)
		return ProtocolHandler->status();
	else
		return Status::null;
}

int AccountShared::maxDescriptionLength()
{
	if (ProtocolHandler)
		return ProtocolHandler->maxDescriptionLength();
	else
		return 0;
}

QString AccountShared::statusName()
{
	return Status::name(status(), false);
}

QPixmap AccountShared::statusPixmap()
{
	return statusPixmap(status());
}

QPixmap AccountShared::statusPixmap(const QString &statusType)
{
	if (ProtocolHandler)
		return ProtocolHandler->statusPixmap(statusType);
	else
		return QPixmap();
}

QPixmap AccountShared::statusPixmap(Status status)
{
	if (ProtocolHandler)
		return ProtocolHandler->statusPixmap(status);
	else
		return QPixmap();
}

void AccountShared::setPrivateStatus(bool isPrivate)
{
	if (ProtocolHandler)
		ProtocolHandler->setPrivateMode(isPrivate);
}

QList<StatusType *> AccountShared::supportedStatusTypes()
{
	if (ProtocolHandler)
		return ProtocolHandler->protocolFactory()->supportedStatusTypes();
	else
		return QList<StatusType *>();
}
