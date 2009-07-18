/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "jabber-account.h"
#include "jabber-account-data-manager.h"

JabberAccountDataManager::JabberAccountDataManager(JabberAccount* data)
	: AccountDataManager(data), Data(data)
{
}

void JabberAccountDataManager::writeEntry(const QString &section, const QString &name, const QVariant &value)
{
	if (section != "Jabber")
	{
		AccountDataManager::writeEntry(section, name, value);
		return;
	}

	if (name == "Resource")
		Data->setResource(value.value<QString>());

	if (name == "Priority")
	{
		bool ok;
		QString prioString(value.value<QString>());
		Data->setPriority(prioString.toInt(&ok));
		if (prioString.isEmpty() || !ok )
			Data->setPriority(5);
	}

	// other data
}

QVariant JabberAccountDataManager::readEntry(const QString &section, const QString &name)
{
	if (section != "Jabber")
		return AccountDataManager::readEntry(section, name);

	if (name == "Resource")
		return Data->resource();

	if (name == "Priority")
		return Data->priority();

	// other data

	return QVariant(QString::null);
}
