/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef CONTACT_SET_CONFIGURATION_HELPER_H
#define CONTACT_SET_CONFIGURATION_HELPER_H

#include "contacts/contact-set.h"

class ContactSetConfigurationHelper
{
public:
	static ContactSet loadFromConfiguration(StorableObject *parent, const QString &nodeName);
	static ContactSet loadFromConfiguration(XmlConfigFile *configurationStorage, QDomElement contactSetNode);
	static void saveToConfiguration(StorableObject *parent, const QString &nodeName, ContactSet contactSet);
	static void saveToConfiguration(XmlConfigFile *configurationStorage, QDomElement contactSetNode, ContactSet contactSet);
};

#endif // CONTACT_SET_CONFIGURATION_HELPER_H
