/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef ABSTRACT_BUDDY_FILTER
#define ABSTRACT_BUDDY_FILTER

#include <QtCore/QObject>

class Buddy;

class AbstractBuddyFilter : public QObject
{
	Q_OBJECT

public:
	AbstractBuddyFilter(QObject *parent = 0)
			: QObject(parent) {}

	virtual bool acceptBuddy(Buddy buddy) = 0;

signals:
	void filterChanged();

};

#endif // ABSTRACT_BUDDY_FILTER
