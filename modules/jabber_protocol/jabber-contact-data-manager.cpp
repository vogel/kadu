/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "jabber-contact.h"

#include "jabber-contact-data-manager.h"

JabberContactDataManager::JabberContactDataManager(JabberContact* data, QObject *parent)
: ConfigurationContactAccountDataManager(data, parent), Data(data)
{
}

void JabberContactDataManager::writeEntry(const QString &section, const QString &name, const QVariant &value)
{
	if (section != "Jabber")
	{
		ConfigurationContactAccountDataManager::writeEntry(section, name, value);
		return;
	}

	// TODO : setId

	// other data
}

QVariant JabberContactDataManager::readEntry(const QString &section, const QString &name)
{
	if (section != "Jabber")
		return ConfigurationContactAccountDataManager::readEntry(section, name);

	if (name == "Jid")
		return Data->id();

	// other data

	return QVariant(QString::null);
}
