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

#include "accounts/account.h"

#include "jabber-account-details.h"

#include "jabber-account-data-manager.h"

JabberAccountDataManager::JabberAccountDataManager(Account data) :
		AccountDataManager(data)
{
	Data = dynamic_cast<JabberAccountDetails *>(data.details());
}

void JabberAccountDataManager::writeEntry(const QString &section, const QString &name, const QVariant &value)
{
	if (section != "Jabber")
	{
		AccountDataManager::writeEntry(section, name, value);
		return;
	}

	if (!Data)
		return;

	if (name == "Resource")
		Data->setResource(value.toString());

	if (name == "Priority")
	{
		bool ok;
		QString prioString(value.toString());
		Data->setPriority(prioString.toInt(&ok));
		if (prioString.isEmpty() || !ok )
			Data->setPriority(5);
	}

	if (name == "DataTransferProxy")
		Data->setDataTransferProxy(value.toString());

	// other data
}

QVariant JabberAccountDataManager::readEntry(const QString &section, const QString &name)
{
	if (!Data)
		return QVariant(QString());

	if (section != "Jabber")
		return AccountDataManager::readEntry(section, name);

	if (name == "Resource")
		return Data->resource();

	if (name == "Priority")
		return Data->priority();

	if (name == "DataTransferProxy")
		return Data->dataTransferProxy();

	// other data

	return QVariant(QString());
}
