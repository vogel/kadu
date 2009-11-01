/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef HAS_DESCRIPTION_BUDDY_FILTER_H
#define HAS_DESCRIPTION_BUDDY_FILTER_H

#include <QtCore/QMetaType>

#include "abstract-buddy-filter.h"

class HasDescriptionBuddyFilter : public AbstractBuddyFilter
{
	Q_OBJECT

	bool Enabled;

public:
	HasDescriptionBuddyFilter(QObject *parent = 0);

	void setEnabled(bool enabled);
	virtual bool acceptBuddy(Buddy contact);

};

Q_DECLARE_METATYPE(HasDescriptionBuddyFilter *)

#endif // HAS_DESCRIPTION_BUDDY_FILTER_H
