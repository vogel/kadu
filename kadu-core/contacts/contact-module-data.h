/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef CONTACT_MODULE_DATA
#define CONTACT_MODULE_DATA

#include <QtNetwork/QHostAddress>
#include <QtXml/QDomElement>

class Account;
class StoragePoint;
class XmlConfigFile;

class ContactModuleData
{
	StoragePoint *Storage;

public:
	ContactModuleData(StoragePoint *storage);
	virtual ~ContactModuleData();

	virtual void storeConfiguration() const = 0;

};

#endif
