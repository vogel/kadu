/*
 * %kadu copyright begin%
 * Copyright 2010 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2010 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2009 Bartłomiej Zimoń (uzi18@o2.pl)
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

#include "gadu-contact-details.h"

#include "gadu-contact-data-manager.h"

GaduContactDataManager::GaduContactDataManager(Contact data, QObject *parent) :
		ConfigurationContactDataManager(data, parent), Data(dynamic_cast<GaduContactDetails *>(data.details()))
{
}

void GaduContactDataManager::writeEntry(const QString &section, const QString &name, const QVariant &value)
{
	if (section != "Gadu-Gadu")
	{
		ConfigurationContactDataManager::writeEntry(section, name, value);
		return;
	}

	//if (name == "Uin")
	//	Data->setId(value.toString());

	// other data
}

QVariant GaduContactDataManager::readEntry(const QString &section, const QString &name)
{
	if (section != "Gadu-Gadu")
		return ConfigurationContactDataManager::readEntry(section, name);

	if (name == "Uin")
		return Data->uin();

	// other data

	return QVariant(QString());
}
