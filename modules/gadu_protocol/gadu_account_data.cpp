/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "misc.h"
#include "xml_config_file.h"

#include "gadu_account_data.h"

GaduAccountData::GaduAccountData()
{
}

GaduAccountData::GaduAccountData(UinType uin, const QString &password)
	: Uin(uin), Password(password)
{
}

bool GaduAccountData::loadConfiguration(XmlConfigFile *configurationStorage, QDomElement parent)
{
	QDomElement gaduNode = configurationStorage->getNode(parent, "Gadu");
	QString uinString = configurationStorage->getTextNode(gaduNode, "Uin");
	QString passwordString = configurationStorage->getTextNode(gaduNode, "Password");

	bool ok;
	Uin = uinString.toLong(&ok);
	if (!ok)
		return false;

	Password = pwHash(passwordString);
}

void GaduAccountData::storeConfiguration(XmlConfigFile *configurationStorage, QDomElement parent)
{
	QDomElement gaduNode = configurationStorage->getNode(parent, "Gadu");
	configurationStorage->createTextNode(gaduNode, "Uin", QString::number(Uin));
	configurationStorage->createTextNode(gaduNode, "Password", pwHash(Password));
}
