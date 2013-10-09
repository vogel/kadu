/*
 * %kadu copyright begin%
 * Copyright 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2004 Adrian Smarzewski (adrian@kadu.net)
 * Copyright 2007, 2008, 2009, 2010, 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2010, 2013 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2004, 2006 Marcin Ślusarz (joi@kadu.net)
 * %kadu copyright end%
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include "configuration/configuration-file.h"

#include "config-file-data-manager.h"

void ConfigFileDataManager::writeEntry(const QString &section, const QString &name, const QVariant &value)
{
	if (section.isEmpty() || name.isEmpty())
		return;

	config_file.writeEntry(section, name, value.toString());
}

QVariant ConfigFileDataManager::readEntry(const QString &section, const QString &name)
{
	if (section.isEmpty() || name.isEmpty())
		return QVariant(QString());

	return QVariant(config_file.readEntry(section, name));
}


#include "moc_config-file-data-manager.cpp"
