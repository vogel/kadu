/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "contacts/account-data/contact-account-data.h"
#include "status/status.h"

#include "configuration-contact-account-data-manager.h"

ConfigurationContactAccountDataManager::ConfigurationContactAccountDataManager(ContactAccountData *data, QObject *parent)
	: ConfigurationWindowDataManager(parent), Data(data)
{
}

void ConfigurationContactAccountDataManager::writeEntry(const QString &section, const QString &name, const QVariant &value)
{
	if (section != "Contact")
		return;

	if (name == "Blocked")
		Data->setBlocked(value.value<bool>());

	if (name == "OfflineTo")
		Data->setOfflineTo(value.value<bool>());
}

QVariant ConfigurationContactAccountDataManager::readEntry(const QString &section, const QString &name)
{
	if (section != "Contact")
		return QVariant(QString::null);

	if (name == "Id")
		return Data->id();

	if (name == "Addr")
	// TODO : if not valid : (unknown):(unknown)
		return QString("%1:%2").arg(Data->ip().toString(), QString::number(Data->port()));

	if (name == "DnsName")
		return Data->dnsName();

	if (name == "Version")
		return Data->protocolVersion();

	if (name == "Status")
		return Status::name(Data->status());

	if (name == "Blocked")
		return Data->isBlocked();

	if (name == "OfflineTo")
		return Data->isOfflineTo();

	return QVariant(QString::null);
}
