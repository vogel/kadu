/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "tlen-contact-account-data.h"

#include "tlen-contact-account-data-manager.h"

TlenContactAccountDataManager::TlenContactAccountDataManager(TlenContactAccountData* data, QObject *parent)
	: ContactAccountDataManager(data, parent), Data(data)
{
}

void TlenContactAccountDataManager::writeEntry(const QString &section, const QString &name, const QVariant &value)
{
	if (section != "Tlen")
	{
		ContactAccountDataManager::writeEntry(section, name, value);
		return;
	}

	// TODO : setId

	// other data
}

QVariant TlenContactAccountDataManager::readEntry(const QString &section, const QString &name)
{
	if (section != "Tlen")
		return ContactAccountDataManager::readEntry(section, name);

	if (name == "Tlenid")
		return Data->id();

	// other data

	return QVariant(QString::null);
}
