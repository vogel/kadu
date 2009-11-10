/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "tlen-contact.h"

#include "tlen-contact-manager.h"

TlenContactManager::TlenContactManager(TlenContact* data, QObject *parent)
	: ConfigurationContactDataManager(data, parent), Data(data)
{
}

void TlenContactManager::writeEntry(const QString &section, const QString &name, const QVariant &value)
{
	if (section != "Tlen")
	{
		ConfigurationContactDataManager::writeEntry(section, name, value);
		return;
	}

	// TODO : setId

	// other data
}

QVariant TlenContactManager::readEntry(const QString &section, const QString &name)
{
	if (section != "Tlen")
		return ConfigurationContactDataManager::readEntry(section, name);

	if (name == "Tlenid")
		return Data->id();

	// other data

	return QVariant(QString::null);
}
