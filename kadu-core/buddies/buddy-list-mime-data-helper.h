/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef BUDDY_LIST_MIME_DATA_HELPER_H
#define BUDDY_LIST_MIME_DATA_HELPER_H

#include "buddies/buddy-list.h"

class QMimeData;

class BuddyList;

class BuddyListMimeDataHelper
{
	static QLatin1String MimeType;

public:
	static QStringList mimeTypes();
	static QMimeData * toMimeData(BuddyList contactList);
	static BuddyList fromMimeData(const QMimeData *mimeData);

};

#endif // BUDDY_LIST_MIME_DATA_HELPER_H
