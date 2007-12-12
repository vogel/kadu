/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef ADVANCED_USERLIST_H
#define ADVANCED_USERLIST_H

#include "main_configuration_window.h"
#include "userlistelement.h"

class UserInfo;

class AdvancedUserList : public ConfigurationUiHandler
{
	Q_OBJECT

	QListBox *sortingListBox;
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
