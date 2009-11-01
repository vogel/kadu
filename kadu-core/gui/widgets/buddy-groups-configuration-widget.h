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

#include <QtGui/QCheckBox>
#include <QtGui/QScrollArea>

#include "buddies/buddy.h"

#include "exports.h"

class KADUAPI BuddyGroupsConfigurationWidget : public QScrollArea
{
	Q_OBJECT

	Buddy MyBuddy;
	QList<QCheckBox *> GroupCheckBoxList;
	void createGui();

public:
	BuddyGroupsConfigurationWidget(Buddy &contact, QWidget *parent = 0);
	~BuddyGroupsConfigurationWidget();
	void saveConfiguration();

};

#endif // BUDDY_GROUPS_CONFIGURATION_WIDGET_H
