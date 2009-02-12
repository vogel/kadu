/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef CONTAT_LIST_SERVICE_H
#define CONTAT_LIST_SERVICE_H

#include <QtCore/QObject>

#include "contacts/contact-list.h"

class ContactListService : public QObject
{
	Q_OBJECT

public:
	ContactListService(QObject *parent = 0)
		: QObject(parent) {}

	virtual void importContactList() = 0;
	virtual void exportContactList() = 0;
	virtual void exportContactList(ContactList contacts) = 0;

signals:
	void contactListImported(bool ok, ContactList contacts);
	void contactListExported(bool ok);

};

#endif // CONTAT_LIST_SERVICE_H
