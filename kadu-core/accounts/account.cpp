/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "account_data.h"

#include "contacts/contact-account-data.h"
#include "contacts/contact-manager.h"

#include "protocols/protocol.h"
#include "protocols/protocol_factory.h"
#include "protocols/protocols_manager.h"

#include "xml_config_file.h"

#include "account.h"

Account::Account(const QUuid &uuid)
	: ProtocolHandler(0), Data(0)
{
	Uuid = uuid.isNull()
		? QUuid::createUuid()
		: uuid;
}

Account::Account(const QUuid &uuid, Protocol *protocol, AccountData *data)
	: Data(data)
{
	Uuid = uuid.isNull()
		? QUuid::createUuid()
		: uuid;

	setProtocol(protocol);

	triggerAllContactsAdded();
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
	ProtocolHandler->setAccount(this);
	ProtocolHandler->setData(Data);

	connect(ProtocolHandler, SIGNAL(contactStatusChanged(Account *, Contact, Status)),
			this, SIGNAL(contactStatusChanged(Account *, Contact, Status)));

	triggerAllContactsAdded();
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

bool Account::loadConfiguration(XmlConfigFile *configurationStorage, QDomElement parent)
{
	QString protocolName = configurationStorage->getTextNode(parent, "Protocol");
	QString name  = configurationStorage->getTextNode(parent, "Name");

	if (name.isEmpty())
		name = parent.attribute("name");

	Protocol *protocol = ProtocolsManager::instance()->newInstance(protocolName);
	if (0 == protocol)
		return false;

	Data = protocol->createAccountData();
	if (0 == Data)
		return false;

	Data->setName(name);
	setProtocol(protocol);

	return Data->loadConfiguration(configurationStorage, parent);
}

void Account::storeConfiguration(XmlConfigFile *configurationStorage, QDomElement parent)
{
	parent.setAttribute("uuid", Uuid.toString());
	parent.removeAttribute("name");

	configurationStorage->createTextNode(
			parent, "Protocol",
			ProtocolHandler->protocolFactory()->name());
	configurationStorage->createTextNode(
			parent, "Name",
			Data->name());

	Data->storeConfiguration(configurationStorage, parent);
}

UserStatus Account::currentStatus()
{
	UserStatus x;
	if (0 != ProtocolHandler)
		x.setStatus(ProtocolHandler->status());

	return x;
}

QString Account::name()
{
	return Data->name();
}

Contact Account::getContactById(const QString& id)
{
	return ContactManager::instance()->byId(this, id);
}

Contact Account::createAnonymous(const QString& id)
{
	Contact result(Contact::TypeAnonymous);

	ProtocolFactory *protocolFactory = ProtocolHandler->protocolFactory();
	ContactAccountData *contactAccountData = protocolFactory->newContactAccountData(result, this, id);
	if (!contactAccountData->isValid())
	{
		delete contactAccountData;
		return Contact::null;
	}

	result.addAccountData(contactAccountData);
	return result;
}

QPixmap Account::statusPixmap(Status status)
{
	return ProtocolHandler->statusPixmap(status);
}
