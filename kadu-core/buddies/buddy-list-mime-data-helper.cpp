/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

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
