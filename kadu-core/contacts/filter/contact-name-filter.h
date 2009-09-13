 /***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef CONTACT_NAME_FILTER_H
#define CONTACT_NAME_FILTER_H

#include "contacts/filter/abstract-contact-filter.h"

class ContactNameFilter : public AbstractContactFilter
{
	QString Name;

public:
	ContactNameFilter(QObject *parent = 0) : AbstractContactFilter(parent) {}

	virtual bool acceptContact(Contact contact);

	void setName(const QString &name);

};

#endif // CONTACT_NAME_FILTER_H
