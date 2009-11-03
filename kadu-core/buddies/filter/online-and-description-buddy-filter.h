/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef ONLINE_AND_DESCRIPTION_BUDDY_FILTER_H
#define ONLINE_AND_DESCRIPTION_BUDDY_FILTER_H

#include <QtCore/QMetaType>

#include "abstract-buddy-filter.h"

class OnlineAndDescriptionBuddyFilter : public AbstractBuddyFilter
{
	Q_OBJECT

	bool Enabled;

public:
	OnlineAndDescriptionBuddyFilter(QObject *parent = 0);

	void setEnabled(bool enabled);
	virtual bool acceptBuddy(Buddy buddy);

};

Q_DECLARE_METATYPE(OnlineAndDescriptionBuddyFilter *)

#endif // ONLINE_AND_DESCRIPTION_BUDDY_FILTER_H
