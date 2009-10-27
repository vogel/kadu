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
#include "misc/misc.h"
#include "protocols/protocol.h"

#include "account-data.h"

AccountData * AccountData::loadFromStorage(StoragePoint *storagePoint)
{
	AccountData *result = new AccountData(TypeNormal);
	result->setStorage(storagePoint);
	result->load();

	return result;
}

AccountData::AccountData(AccountType type, QUuid uuid) :
		BaseStatusContainer("Account", AccountManager::instance()),
		Uuid(uuid.isNull() ? QUuid::createUuid() : uuid), Type(type),
		BlockUpdatedSignalCount(0), Updated(false),
		ProtocolHandler(0), Details(0),
		RememberPassword(false), HasPassword(false),
		ConnectAtStart(true),
		UseProxy(false), ProxyPort(0)
{
}

AccountData::~AccountData()
{
	if (ProtocolHandler)
	{
		delete ProtocolHandler;
		ProtocolHandler = 0;
	}
}

void AccountData::load()
{
	if (!isValidStorage())
		return;

	BaseStatusContainer::load();
	
	ConnectAtStart = loadValue<bool>("ConnectAtStart", true);
	
	Uuid = QUuid(loadValue<QString>("uuid"));
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

void AccountData::store()
{
	if (!isValidStorage())
		return;

	storage()->point().setAttribute("uuid", Uuid.toString());

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

void AccountData::dataUpdated()
{
	Updated = true;
	emitUpdated();
}

void AccountData::emitUpdated()
{
	if (0 == BlockUpdatedSignalCount && Updated)
	{
		emit updated();
		Updated = false;
	}
}

void AccountData::unloadProtocol()
{
	delete ProtocolHandler;
	ProtocolHandler = 0;

	delete Details;
	Details = 0;
}

QString AccountData::statusContainerName()
{
	return name();
}

void AccountData::setStatus(Status status)
{
	if (ProtocolHandler)
		ProtocolHandler->setStatus(status);
}

Status AccountData::status()
{
	if (ProtocolHandler)
		ProtocolHandler->status();
	else
		return Status();
}

int AccountData::maxDescriptionLength()
{
	if (ProtocolHandler)
		return ProtocolHandler->maxDescriptionLength();
	else
		return 0;
}

QString AccountData::statusName()
{
	return Status::name(status(), false);
}

QPixmap AccountData::statusPixmap()
{
	return statusPixmap(status());
}

QPixmap AccountData::statusPixmap(const QString &statusType)
{
	if (ProtocolHandler)
		return ProtocolHandler->statusPixmap(statusType);
	else
		return QPixmap();
}

QPixmap AccountData::statusPixmap(Status status)
{
	if (ProtocolHandler)
		return ProtocolHandler->statusPixmap(status);
	else
		return QPixmap();
}

void AccountData::setPrivateStatus(bool isPrivate)
{
	if (ProtocolHandler)
		ProtocolHandler->setPrivateMode(isPrivate);
}

QList<StatusType *> AccountData::supportedStatusTypes()
{
	if (ProtocolHandler)
		return ProtocolHandler->protocolFactory()->supportedStatusTypes();
	else
		return QList<StatusType *>();
}
