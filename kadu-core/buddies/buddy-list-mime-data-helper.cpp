/*
 * %kadu copyright begin%
 * Copyright 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2010, 2011 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2009, 2010, 2011 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include <QtCore/QMimeData>
#include <QtCore/QStringList>

#include "buddies/buddy-manager.h"
#include "buddies/buddy.h"

#include "buddy-list-mime-data-helper.h"

QLatin1String BuddyListMimeDataHelper::MimeType("application/x-kadu-buddy-list");

QStringList BuddyListMimeDataHelper::mimeTypes()
{
	QStringList result;
	result << MimeType;
	return result;
}

QMimeData * BuddyListMimeDataHelper::toMimeData(const BuddyList &buddyList)
{
	if (buddyList.isEmpty())
		return 0;

	QMimeData *mimeData = new QMimeData();

	QStringList buddyListStrings;
	foreach (const Buddy &buddy, buddyList)
		buddyListStrings << buddy.uuid().toString();

	mimeData->setData(MimeType, buddyListStrings.join(":").toUtf8());
	return mimeData;
}

BuddyList BuddyListMimeDataHelper::fromMimeData(const QMimeData *mimeData)
{
	BuddyList result;

	QString buddyListString(mimeData->data(MimeType));
	if (buddyListString.isEmpty())
		return result;

	QStringList buddyListStrings = buddyListString.split(':');
	foreach (const QString &buddyListString, buddyListStrings)
	{
		Buddy buddy = BuddyManager::instance()->byUuid(buddyListString);
		if (buddy.isNull())
			continue;

		result << buddy;
	}

	return result;
}
