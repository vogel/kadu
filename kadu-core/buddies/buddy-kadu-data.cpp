/*
 * %kadu copyright begin%
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2009, 2010 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009, 2010, 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include <QtCore/QVariant>

#include "configuration/xml-configuration-file.h"
#include "misc/misc.h"
#include "storage/storage-point.h"

#include "buddy-kadu-data.h"

BuddyKaduData::BuddyKaduData(const QString &moduleName, StorableObject *parent, QObject *qobjectParent) :
		ModuleData(moduleName, parent, qobjectParent)
{
}

BuddyKaduData::~BuddyKaduData()
{
}

void BuddyKaduData::load()
{
	if (!isValidStorage())
		return;

	StorableObject::load();

	HideDescription = loadValue<bool>("HideDescription");
}

void BuddyKaduData::store()
{
	if (!isValidStorage())
		return;

	storeValue("HideDescription", HideDescription);
}

QString BuddyKaduData::name() const
{
	return QLatin1String("kadu");
}
