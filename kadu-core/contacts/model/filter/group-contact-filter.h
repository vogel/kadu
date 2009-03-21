 /***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef GROUP_CONTACT_FILTER
#define GROUP_CONTACT_FILTER

#include "abstract-contact-filter.h"

class Group;

class GroupContactFilter : public AbstractContactFilter
{
	Q_OBJECT

	Group *CurrentGroup;
	bool AllGroupShown;

public:
	GroupContactFilter(QObject *parent = 0);

	void setGroup(Group *group);
	virtual bool acceptContact(Contact contact);

	void refresh();

	void setAllGroupShown(bool shown);
};

#endif // GROUP_CONTACT_FILTER
