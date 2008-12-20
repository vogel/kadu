/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "xml_config_file.h"

#include "contact.h"

Contact::Contact()
	: Data(new ContactData())
{
}

Contact::Contact(const Contact &copy)
	: Data(copy.Data)
{
}

Contact::~Contact()
{
}

Contact & Contact::operator = (const Contact& copy)
{
	Data = copy.Data;
	return *this;
}

void Contact::importConfiguration(XmlConfigFile *configurationStorage, QDomElement parent)
{
	Data->importConfiguration(configurationStorage, parent);
}

void Contact::loadConfiguration(XmlConfigFile *configurationStorage, QDomElement parent)
{
	Data->loadConfiguration(configurationStorage, parent);
}

void Contact::storeConfiguration(XmlConfigFile *configurationStorage, QDomElement parent)
{
	Data->storeConfiguration(configurationStorage, parent);
}
