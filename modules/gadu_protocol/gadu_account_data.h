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
	QString Password;

public:
	GaduAccountData();
	GaduAccountData(UinType uin, const QString &password);

	virtual bool loadConfiguration(XmlConfigFile *configurationStorage, QDomElement parent);
	virtual void storeConfiguration(XmlConfigFile *configurationStorage, QDomElement parent);

	UinType uin() { return Uin; }
	QString password() { return Password; }

};

#endif // GADU_ACCOUNT_DATA
