/*
 * %kadu copyright begin%
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2010 Piotr Galiszewski (piotrgaliszewski@gmail.com)
 * Copyright 2009, 2009 Bartłomiej Zimoń (uzi18@o2.pl)
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
#include "jabber-contact-details.h"

#include "jabber-contact-data-manager.h"

JabberContactDataManager::JabberContactDataManager(Contact data, QObject *parent) :
		ConfigurationContactDataManager(data, parent), Data(dynamic_cast<JabberContactDetails *>(data.details()))
{
}

void JabberContactDataManager::writeEntry(const QString &section, const QString &name, const QVariant &value)
{
	if (section != "Jabber")
	{
		ConfigurationContactDataManager::writeEntry(section, name, value);
		return;
	}

	// TODO : setId

	// other data
}

QVariant JabberContactDataManager::readEntry(const QString &section, const QString &name)
{
	if (section != "Jabber")
		return ConfigurationContactDataManager::readEntry(section, name);

	if (name == "Jid")
		return Data->mainData()->id();

	// other data

	return QVariant(QString());
}
