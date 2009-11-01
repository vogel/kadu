/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef ONLINE_BUDDY_FILTER_H
#define ONLINE_BUDDY_FILTER_H

#include <QtCore/QMetaType>

#include "abstract-buddy-filter.h"

class OnlineBuddyFilter : public AbstractBuddyFilter
{
	Q_OBJECT

	bool Enabled;

public:
	OnlineBuddyFilter(QObject *parent = 0);

	void setEnabled(bool enabled);
	virtual bool acceptBuddy(Buddy contact);

};

Q_DECLARE_METATYPE(OnlineBuddyFilter *)

#endif // ONLINE_BUDDY_FILTER_H
