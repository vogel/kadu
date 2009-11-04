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
#include "buddies/account-data/contact-account-data.h"
#include "misc/misc.h"
#include "protocols/protocol.h"

#include "account-shared.h"

AccountShared * AccountShared::loadFromStorage(StoragePoint *storagePoint)
{
	AccountShared *result = new AccountShared(TypeNormal);
	result->setStorage(storagePoint);
	result->load();

	return result;
}

AccountShared::AccountShared(AccountType type, QUuid uuid) :
		BaseStatusContainer("Account", AccountManager::instance()),
		Uuid(uuid.isNull() ? QUuid::createUuid() : uuid), Type(type),
		BlockUpdatedSignalCount(0), Updated(false),
		ProtocolHandler(0), Details(0),
		RememberPassword(false), HasPassword(false),
		ConnectAtStart(true),
		UseProxy(false), ProxyPort(0)
{
}

AccountShared::~AccountShared()
{
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

	BaseStatusContainer::load();
	
	ConnectAtStart = loadValue<bool>("ConnectAtStart", true);
	
	Uuid = QUuid(loadAttribute<QString>("uuid"));
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
}

void AccountShared::store()
{
	if (!isValidStorage())
		return;

	storeValue("uuid", Uuid.toString(), true);

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

void AccountShared::dataUpdated()
{
	Updated = true;
	emitUpdated();
}

void AccountShared::emitUpdated()
{
	if (0 == BlockUpdatedSignalCount && Updated)
	{
		emit updated();
		Updated = false;
	}
}

void AccountShared::loadProtocol(ProtocolFactory* protocolFactory)
{
	ensureLoaded();

	ProtocolHandler = protocolFactory->createProtocolHandler(Account(this));
	Details = protocolFactory->createAccountDetails(Account(this));

	connect(ProtocolHandler, SIGNAL(statusChanged(Account, Status)), this, SIGNAL(statusChanged()));
	connect(ProtocolHandler, SIGNAL(buddyStatusChanged(Account, Buddy, Status)),
			this, SIGNAL(buddyStatusChanged(Account, Buddy, Status)));
}

void AccountShared::unloadProtocol()
{
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
