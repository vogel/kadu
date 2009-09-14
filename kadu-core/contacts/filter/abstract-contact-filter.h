 /***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef ABSTRACT_CONTACT_FILTER
#define ABSTRACT_CONTACT_FILTER

#include <QtCore/QObject>

class Contact;

class AbstractContactFilter : public QObject
{
	Q_OBJECT

public:
	AbstractContactFilter(QObject *parent = 0)
		: QObject(parent) {}

	virtual bool acceptContact(Contact contact) = 0;

signals:
	void filterChanged();

};

#endif // ABSTRACT_CONTACT_FILTER
