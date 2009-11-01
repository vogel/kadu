 /***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef NON_IN_CONTACT_FILTER
#define NON_IN_CONTACT_FILTER

#include "buddies/buddy.h"

#include "accounts/filter/abstract-account-filter.h"

class NonInContactFilter : public AbstractAccountFilter
{
	Q_OBJECT

	Buddy MyContact;

protected:
	virtual bool acceptAccount(Account account);

public:
	explicit NonInContactFilter(QObject *parent = 0);
	virtual ~NonInContactFilter();

	void setContact(Buddy contact);

};

#endif // NON_IN_CONTACT_FILTER
