/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef KADU_WINDOW_ACTIONS_H
#define KADU_WINDOW_ACTIONS_H

#include <QtCore/QObject>

class QAction;

class ActionDescription;

class KaduWindowActions : QObject
{
	Q_OBJECT

	friend class KaduWindow;
	ActionDescription *Configuration;
	ActionDescription *YourAccounts;
	ActionDescription *HideKadu;
	ActionDescription *ExitKadu;
	ActionDescription *AddUser;
	ActionDescription *AddGroup;
	ActionDescription *OpenSearch;
	ActionDescription *ManageIgnored;
	ActionDescription *ImportExportContacts;
	ActionDescription *Help;
	ActionDescription *Bugs;
	ActionDescription *Support;
	ActionDescription *GetInvolved;
	ActionDescription *About;

private slots:
	void configurationActionActivated(QAction *sender, bool toggled);
	void yourAccountsActionActivated(QAction *sender, bool toggled);
	void hideKaduActionActivated(QAction *sender, bool toggled);
	void exitKaduActionActivated(QAction *sender, bool toggled);
	void addUserActionActivated(QAction *sender, bool toggled);
	void addGroupActionActivated(QAction *sender, bool toggled);
	void searchInDirectoryActionActivated(QAction *sender, bool toggled);
	void manageIgnoredActionActivated(QAction *sender, bool toggled);
	void importExportContactsActionActivated(QAction *sender, bool toggled);
	void helpActionActivated(QAction *sender, bool toggled);
	void bugsActionActivated(QAction *sender, bool toggled);
	void supportActionActivated(QAction *sender, bool toggled);
	void getInvolvedActionActivated(QAction *sender, bool toggled);
	void aboutActionActivated(QAction *sender, bool toggled);

public:
	explicit KaduWindowActions(QObject *parent);
	virtual ~KaduWindowActions();

};

#endif // KADU_WINDOW_ACTIONS_H

