/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef GADU_ACCOUNT_DATA
#define GADU_ACCOUNT_DATA

#include <QtCore/QString>

#include "accounts/account_data.h"
#include "gadu.h"

class GaduAccountData : public AccountData
{
	UinType Uin;

public:
	GaduAccountData();
	GaduAccountData(const QString &name, const QString &id, const QString &password);
	GaduAccountData(const QString &name, UinType uin, const QString &password);

	UinType uin() { return Uin; }

	virtual bool setId(const QString &id);
	virtual bool validateId(const QString &id);
	virtual bool loadConfiguration(XmlConfigFile *configurationStorage, QDomElement parent);
	virtual void storeConfiguration(XmlConfigFile *configurationStorage, QDomElement parent);

};

#endif // GADU_ACCOUNT_DATA
