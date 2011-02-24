/*
 * %kadu copyright begin%
 * Copyright 2009, 2010 Piotr Galiszewski (piotr.galiszewski@kadu.im)
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

#include "contacts/contact.h"
#include "contacts/contact-shared.h"

#include "tlen-contact-details.h"

#include "contacts/contact-shared.h"

#include "tlen-contact-manager.h"

TlenContactManager::TlenContactManager(Contact data, QObject *parent) :
		ConfigurationContactDataManager(data, parent), Data(dynamic_cast<TlenContactDetails *>(data.details()))
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
		return Data->mainData()->id();

	// other data

	return QVariant(QString());
}
