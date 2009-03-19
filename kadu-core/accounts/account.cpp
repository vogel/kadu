/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "contacts/contact-account-data.h"
#include "contacts/contact-manager.h"

#include "protocols/protocol.h"
#include "protocols/protocol_factory.h"
#include "protocols/protocols_manager.h"

#include "misc/misc.h"
#include "xml_config_file.h"

#include "account.h"

Account::Account(const QUuid &uuid)
	: ProtocolHandler(0)
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

	connect(ProtocolHandler, SIGNAL(contactStatusChanged(Account *, Contact, Status)),
			this, SIGNAL(contactStatusChanged(Account *, Contact, Status)));
}

bool Account::setId(const QString &id)
{
	Id = id;
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

void Account::loadConfiguration(XmlConfigFile *configurationStorage, QDomElement parent)
{
	Uuid = QUuid(parent.attribute("uuid"));
	Name = configurationStorage->getTextNode(parent, "Name");
	setId(configurationStorage->getTextNode(parent, "Id"));
	Password = pwHash(configurationStorage->getTextNode(parent, "Password"));

	triggerAllContactsAdded();
}

void Account::storeConfiguration(XmlConfigFile *configurationStorage, QDomElement parent)
{
	parent.setAttribute("uuid", Uuid.toString());

	configurationStorage->createTextNode(parent, "Protocol",
			ProtocolHandler->protocolFactory()->name());
	configurationStorage->createTextNode( parent, "Name", Name);
	configurationStorage->createTextNode(parent, "Id", id());
	configurationStorage->createTextNode(parent, "Password", pwHash(password()));
}

Status Account::currentStatus()
{
	return 0 != ProtocolHandler
		? ProtocolHandler->status()
		: Status();
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
