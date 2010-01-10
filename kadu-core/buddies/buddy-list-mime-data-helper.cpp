/*
 * %kadu copyright begin%
 * Copyright 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2009 Piotr Galiszewski (piotrgaliszewski@gmail.com)
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

#include "buddies/buddy.h"
#include "buddies/buddy-list.h"
#include "buddies/buddy-list-mime-data-helper.h"
#include "buddies/buddy-manager.h"
#include "buddies/buddy-shared.h"

QLatin1String BuddyListMimeDataHelper::MimeType("application/x-kadu-ules");

QStringList BuddyListMimeDataHelper::mimeTypes()
{
	QStringList result;
	result << MimeType;
	return result;
}

QMimeData * BuddyListMimeDataHelper::toMimeData(BuddyList contactList)
{
	if (!contactList.count())
		return 0;

	QMimeData *mimeData = new QMimeData();

	QStringList contactListStrings;
	foreach (Buddy buddy, contactList)
		contactListStrings << buddy.uuid().toString();

	mimeData->setData(MimeType, contactListStrings.join(":").toAscii());
	return mimeData;
}

BuddyList BuddyListMimeDataHelper::fromMimeData(const QMimeData * mimeData)
{
	BuddyList result;

	QString contactListString(mimeData->data(MimeType));
	if (contactListString.isEmpty())
		return result;

	QStringList contactListStrings = contactListString.split(":");
	foreach (const QString &contactListString, contactListStrings)
	{
		Buddy buddy = BuddyManager::instance()->byUuid(contactListString);
		if (buddy.isNull())
			continue;

		result << buddy;
	}

	return result;
}
