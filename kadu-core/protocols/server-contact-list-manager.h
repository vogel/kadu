/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef SERVER_CONTAT_LIST_MANAGER_H
#define SERVER_CONTAT_LIST_MANAGER_H

#include <QtCore/QObject>

#include "contacts/contact-list.h"

class ServerContactListManager : public QObject
{
	Q_OBJECT

public:
	ServerContactListManager(QObject *parent = 0)
		: QObject(parent) {}

	virtual void importContactList() = 0;
	virtual void exportContactList() = 0;
	virtual void exportContactList(ContactList contacts) = 0;

signals:
	void contactListImported(bool ok, ContactList contacts);
	void contactListExported(bool ok);

};

#endif // SERVER_CONTAT_LIST_MANAGER_H
