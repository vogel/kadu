/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef CONTACT_LIST_MIME_DATA_HELPER_H
#define CONTACT_LIST_MIME_DATA_HELPER_H

#include "contacts/contact-list.h"

class QMimeData;

class ContactList;

class ContactListMimeDataHelper
{
	static QLatin1String MimeType;

public:
	static QStringList mimeTypes();
	static QMimeData * toMimeData(ContactList contactList);
	static ContactList fromMimeData(QMimeData *mimeData);

};

#endif // CONTACT_LIST_MIME_DATA_HELPER_H
