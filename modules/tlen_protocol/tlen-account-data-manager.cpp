/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "tlen-account.h"
#include "tlen-account-data-manager.h"

TlenAccountDataManager::TlenAccountDataManager(TlenAccount* data)
	: AccountDataManager(data)
{
}

void TlenAccountDataManager::writeEntry(const QString &section, const QString &name, const QVariant &value)
{
	if (section != "Tlen")
	{
		AccountDataManager::writeEntry(section, name, value);
		return;
	}

	// other data
}

QVariant TlenAccountDataManager::readEntry(const QString &section, const QString &name)
{
	if (section != "Tlen")
		return AccountDataManager::readEntry(section, name);

	// other data

	return QVariant(QString::null);
}
