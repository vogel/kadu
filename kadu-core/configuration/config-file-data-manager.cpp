/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "config_file.h"

#include "config-file-data-manager.h"

void ConfigFileDataManager::writeEntry(const QString &section, const QString &name, const QVariant &value)
{
	if (section.isEmpty() || name.isEmpty())
		return;

	config_file.writeEntry(section, name, value.value<QString>());
}

QVariant ConfigFileDataManager::readEntry(const QString &section, const QString &name)
{
	if (section.isEmpty() || name.isEmpty())
		return QVariant(QString::null);

	return QVariant(config_file.readEntry(section, name));
}

