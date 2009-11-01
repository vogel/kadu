/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef BUDDY_LIST_CONFIGURATION_HELPER_H
#define BUDDY_LIST_CONFIGURATION_HELPER_H

#include "buddies/buddy-list.h"

class BuddyListConfigurationHelper
{
public:
	static BuddyList loadFromConfiguration(XmlConfigFile *configurationStorage, QDomElement contactListNode);
	static void saveToConfiguration(XmlConfigFile *configurationStorage, QDomElement contactListNode, BuddyList contactList);

};

#endif // BUDDY_LIST_CONFIGURATION_HELPER_H
