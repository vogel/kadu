/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef MODULE_DATA
#define MODULE_DATA

#include <QtNetwork/QHostAddress>
#include <QtXml/QDomElement>

#include "exports.h"


class Account;
class StoragePoint;
class XmlConfigFile;

class KADUAPI ModuleData
{
	StoragePoint *Storage;

protected:
	StoragePoint * storage() const { return Storage; }

public:
	ModuleData(StoragePoint *storage);
	virtual ~ModuleData();

	virtual void storeConfiguration() const = 0;
	virtual void loadFromStorage() = 0;
};

#endif
