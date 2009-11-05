/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "gadu-contact.h"

#include "gadu-contact-data-manager.h"

GaduContactDataManager::GaduContactDataManager(GaduContact *data, QObject *parent)
	: ConfigurationContactAccountDataManager(data, parent), Data(data)
{
}

void GaduContactDataManager::writeEntry(const QString &section, const QString &name, const QVariant &value)
{
	if (section != "Gadu-Gadu")
	{
		ConfigurationContactAccountDataManager::writeEntry(section, name, value);
		return;
	}

	//if (name == "Uin")
	//	Data->setId(value.value<QString>());

	// other data
}

QVariant GaduContactDataManager::readEntry(const QString &section, const QString &name)
{
	if (section != "Gadu-Gadu")
		return ConfigurationContactAccountDataManager::readEntry(section, name);

	if (name == "Uin")
		return Data->uin();

	// other data

	return QVariant(QString::null);
}
