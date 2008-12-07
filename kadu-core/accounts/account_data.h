/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef ACCOUNT_DATA
#define ACCOUNT_DATA

#include <QtXml/QDomElement>

class XmlConfigFile;

class AccountData
{
public:
	AccountData() {}
	virtual ~AccountData() {}

	virtual bool loadConfiguration(XmlConfigFile *configurationStorage, QDomElement parent) = 0;
	virtual void storeConfiguration(XmlConfigFile *configurationStorage, QDomElement parent) = 0;

};

#endif // ACCOUNT_DATA
