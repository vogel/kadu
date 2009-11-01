/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef BUDDY_SET_CONFIGURATION_HELPER_H
#define BUDDY_SET_CONFIGURATION_HELPER_H

#include "buddies/buddy-set.h"

class BuddySetConfigurationHelper
{
public:
	static BuddySet loadFromConfiguration(StorableObject *parent, const QString &nodeName);
	static BuddySet loadFromConfiguration(XmlConfigFile *configurationStorage, QDomElement contactSetNode);
	static void saveToConfiguration(StorableObject *parent, const QString &nodeName, BuddySet contactSet);
	static void saveToConfiguration(XmlConfigFile *configurationStorage, QDomElement contactSetNode, BuddySet contactSet);

};

#endif // BUDDY_SET_CONFIGURATION_HELPER_H
