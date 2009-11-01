/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef ANONYMOUS_WITHOUT_MESSAGES_BUDDY_FILTER_H
#define ANONYMOUS_WITHOUT_MESSAGES_BUDDY_FILTER_H

#include <QtCore/QMetaType>

#include "abstract-buddy-filter.h"

class AnonymousWithoutMessagesBuddyFilter : public AbstractBuddyFilter
{
	Q_OBJECT

	bool Enabled;

public:
	AnonymousWithoutMessagesBuddyFilter(QObject *parent = 0);

	void setEnabled(bool enabled);
	virtual bool acceptBuddy(Buddy contact);

};

Q_DECLARE_METATYPE(AnonymousWithoutMessagesBuddyFilter *)

#endif // ANONYMOUS_WITHOUT_MESSAGES_BUDDY_FILTER_H
