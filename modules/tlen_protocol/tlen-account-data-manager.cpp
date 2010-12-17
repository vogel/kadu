/*
 * %kadu copyright begin%
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

#include "tlen-account-details.h"

#include "tlen-account-data-manager.h"

TlenAccountDataManager::TlenAccountDataManager(Account data) :
		AccountDataManager(data)
{
	Data = dynamic_cast<TlenAccountDetails *>(data.details());
}

void TlenAccountDataManager::writeEntry(const QString &section, const QString &name, const QVariant &value)
{
	if (section != "Tlen")
	{
		AccountDataManager::writeEntry(section, name, value);
		return;
	}

	if (!Data)
		return;

	// other data
}

QVariant TlenAccountDataManager::readEntry(const QString &section, const QString &name)
{
	if (!Data)
		return QVariant(QString());

	if (section != "Tlen")
		return AccountDataManager::readEntry(section, name);

	// other data

	return QVariant(QString());
}
