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

GaduAccountData::GaduAccountData(const QString &name, const QString &id, const QString &password)
	: AccountData(name, id, password)
{
	setId(id);
}

GaduAccountData::GaduAccountData(const QString &name, UinType uin, const QString &password)
	: AccountData(name, QString::number(uin), password)
{
	setId(QString::number(uin));
}

bool GaduAccountData::setId(const QString &id)
{
	if (!validateId(id))
		return false;

	Uin = id.toLong();
	return true;
}

bool GaduAccountData::validateId(const QString &id)
{
	bool ok;
	UinType tmpUin = id.toLong(&ok);
	return ok;
}

bool GaduAccountData::loadConfiguration(XmlConfigFile *configurationStorage, QDomElement parent)
{
	QDomElement gaduNode = configurationStorage->getNode(parent, "Gadu");
	QString uinString = configurationStorage->getTextNode(gaduNode, "Uin");
	QString passwordString = configurationStorage->getTextNode(gaduNode, "Password");

	// TODO 0.6.6 if false -> error msg
	setId(uinString);
	setPassword(pwHash(passwordString));
}

void GaduAccountData::storeConfiguration(XmlConfigFile *configurationStorage, QDomElement parent)
{
	QDomElement gaduNode = configurationStorage->getNode(parent, "Gadu");
	configurationStorage->createTextNode(gaduNode, "Uin", id());
	configurationStorage->createTextNode(gaduNode, "Password", pwHash(password()));
}
