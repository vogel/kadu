/*
 * %kadu copyright begin%
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2007, 2008, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2008 Tomasz Rostański (rozteck@interia.pl)
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

#ifndef ADVANCED_USERLIST_H
#define ADVANCED_USERLIST_H

#include <QtCore/QStringList>
#include <QtGui/QListWidget>

#include "main_configuration_window.h"
#include "userlistelement.h"

class UserInfo;

class AdvancedUserList : public ConfigurationUiHandler
{
	Q_OBJECT

	QListWidget *sortingListBox;
	QStringList order, newOrder;

	void displayFunctionList();

private slots:
	void userAdded(UserListElement elem, bool massively, bool last);
	void userboxCreated(QObject *new_object);
	void userInfoWindowCreated(QObject *new_object);
	void updateClicked(UserInfo *);

	void onUpButton();
	void onDownButton();

	void configurationWindowApplied();

public:
	AdvancedUserList();
	virtual ~AdvancedUserList();

	virtual void mainConfigurationWindowCreated(MainConfigurationWindow *mainConfigurationWindow);

};

#endif
