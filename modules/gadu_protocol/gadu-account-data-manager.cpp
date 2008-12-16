/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "gadu_account_data.h"

#include "gadu-account-data-manager.h"

GaduAccountDataManager::GaduAccountDataManager(GaduAccountData* data)
	: AccountDataManager(data)
{
}

void GaduAccountDataManager::writeEntry(const QString &section, const QString &name, const QVariant &value)
{
	if (section != "Gadu-Gadu")
	{
		AccountDataManager::writeEntry(section, name, value);
		return;
	}

	// other data
}

QVariant GaduAccountDataManager::readEntry(const QString &section, const QString &name)
{
	if (section != "Gadu-Gadu")
		return AccountDataManager::readEntry(section, name);

	// other data

	return QVariant(QString::null);
}
