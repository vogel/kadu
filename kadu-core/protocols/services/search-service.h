/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef SEARCH_SERVICE_H
#define SEARCH_SERVICE_H

#include <QtCore/QObject>

#include "contacts/contact-list.h"

#include "exports.h"

class KADUAPI SearchService : public QObject
{
	Q_OBJECT

public:
	SearchService(QObject *parent = 0) : QObject(parent) {}

	virtual void searchFirst(Contact contact) = 0;
	virtual void searchNext() = 0;
	virtual void stop() = 0;

signals:
	void newResults(ContactList contacts);

};

#endif // SEARCH_SERVICE_H
