/*
 * %kadu copyright begin%
 * Copyright 2008, 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "account-data-manager.h"

AccountDataManager::AccountDataManager(Account data, QObject *parent)
	: ConfigurationWindowDataManager(parent), Data(data)
{
}

void AccountDataManager::writeEntry(const QString &section, const QString &name, const QVariant &value)
{
	if (section != "Account")
		return;

	if (name == "Id")
		Data.setId(value.toString());

	if (name == "Password")
		Data.setPassword(value.toString());
}

QVariant AccountDataManager::readEntry(const QString &section, const QString &name)
{
	if (section != "Account")
		return QVariant(QString());

	if (name == "Id")
		return Data.id();

	if (name == "Password")
		return Data.password();

	return QVariant(QString());
}
