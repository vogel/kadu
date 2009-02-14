/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "gadu-contact-account-data.h"

#include "gadu-contact-account-data-manager.h"

GaduContactAccountDataManager::GaduContactAccountDataManager(GaduContactAccountData* data)
	: ContactAccountDataManager(data), Data(data)
{
}

void GaduContactAccountDataManager::writeEntry(const QString &section, const QString &name, const QVariant &value)
{
	if (section != "Gadu-Gadu")
	{
		ContactAccountDataManager::writeEntry(section, name, value);
		return;
	}

	//if (name == "Uin")
	//	Data->setId(value.value<QString>());

	// other data
}

QVariant GaduContactAccountDataManager::readEntry(const QString &section, const QString &name)
{
	if (section != "Gadu-Gadu")
		return ContactAccountDataManager::readEntry(section, name);

	if (name == "Uin")
		return Data->uin();

	// other data

	return QVariant(QString::null);
}
