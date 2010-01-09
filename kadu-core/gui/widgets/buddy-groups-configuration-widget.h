/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef BUDDY_GROUPS_CONFIGURATION_WIDGET_H
#define BUDDY_GROUPS_CONFIGURATION_WIDGET_H

#include <QtGui/QWidget>

#include "buddies/buddy.h"

#include "exports.h"

class QCheckBox;
class QScrollArea;

class KADUAPI BuddyGroupsConfigurationWidget : public QWidget
{
	Q_OBJECT

	Buddy MyBuddy;

	QScrollArea *Groups;
	QList<QCheckBox *> GroupCheckBoxList;

	void createGui();

public:
	explicit BuddyGroupsConfigurationWidget(Buddy &buddy, QWidget *parent = 0);
	virtual ~BuddyGroupsConfigurationWidget();

	void save();

};

#endif // BUDDY_GROUPS_CONFIGURATION_WIDGET_H
