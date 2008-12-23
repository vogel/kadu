/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "xml_config_file.h"

#include "contact-account-data.h"

ContactAccountData::ContactAccountData()
	: ContactAccount(0), Id(QString::null)
{
}

ContactAccountData::ContactAccountData(Account *account, const QString &id)
	: ContactAccount(account), Id(id)
{
}

void ContactAccountData::loadConfiguration(XmlConfigFile *configurationStorage, QDomElement parent)
{
}

void ContactAccountData::storeConfiguration(XmlConfigFile *configurationStorage, QDomElement parent)
{
	configurationStorage->createTextNode(parent, "Id", Id);
}

bool ContactAccountData::isValid()
{
	return validateId();
}

void ContactAccountData::setAddressAndPort(QHostAddress address, int port)
{
	Address = address;
	Port = port;
}
