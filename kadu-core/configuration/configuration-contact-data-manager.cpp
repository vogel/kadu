/*
 * %kadu copyright begin%
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
#include "status/status.h"

#include "configuration-contact-data-manager.h"

ConfigurationContactDataManager::ConfigurationContactDataManager(Contact data, QObject *parent)
	: ConfigurationWindowDataManager(parent), Data(data)
{
}

void ConfigurationContactDataManager::writeEntry(const QString &section, const QString &name, const QVariant &value)
{
	if (section != "Contact")
		return;

	if (name == "Blocked")
		Data.ownerBuddy().setBlocked(value.toBool());

	if (name == "OfflineTo")
		Data.ownerBuddy().setOfflineTo(value.toBool());
}

QVariant ConfigurationContactDataManager::readEntry(const QString &section, const QString &name)
{
	if (section != "Contact")
		return QVariant(QString());

	if (name == "Id")
		return Data.id();

	if (name == "Addr")
	// TODO : if not valid : (unknown):(unknown)
		return QString("%1:%2").arg(Data.address().toString(), QString::number(Data.port()));

	if (name == "DnsName")
		return Data.dnsName();

	if (name == "Version")
		return Data.protocolVersion();

	if (name == "Status")
		return Status::name(Data.currentStatus());

	if (name == "Blocked")
		return Data.ownerBuddy().isBlocked();

	if (name == "OfflineTo")
		return Data.ownerBuddy().isOfflineTo();

	return QVariant(QString());
}
