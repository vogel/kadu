/*
 * %kadu copyright begin%
 * Copyright 2011 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "gadu-account-data-manager.h"

GaduAccountDataManager::GaduAccountDataManager(Account data) :
		AccountDataManager(data)
{
}

void GaduAccountDataManager::writeEntry(const QString &section, const QString &name, const QVariant &value)
{
	if (section != "Gadu-Gadu")
	{
		AccountDataManager::writeEntry(section, name, value);
		return;
	}

	// other data
}

QVariant GaduAccountDataManager::readEntry(const QString &section, const QString &name)
{
	if (section != "Gadu-Gadu")
		return AccountDataManager::readEntry(section, name);

	// other data

	return QVariant(QString());
}
