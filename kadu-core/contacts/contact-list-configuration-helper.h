/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef CONTACT_LIST_CONFIGURATION_HELPER_H
#define CONTACT_LIST_CONFIGURATION_HELPER_H

#include "contacts/contact-list.h"

class ContactListConfigurationHelper
{
public:
	static ContactList loadFromConfiguration(XmlConfigFile *configurationStorage, QDomElement contactListNode);
	static void saveToConfiguration(XmlConfigFile *configurationStorage, QDomElement contactListNode, ContactList contactList);

};

#endif // CONTACT_LIST_CONFIGURATION_HELPER_H
