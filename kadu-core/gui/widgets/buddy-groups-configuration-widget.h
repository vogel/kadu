/*
 * %kadu copyright begin%
 * Copyright 2010 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * %kadu copyright end%
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

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
