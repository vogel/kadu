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

#include "status/status.h"

class QAction;

class Account;
class Action;
class ActionDescription;

class KaduWindowActions : QObject
{
	Q_OBJECT

	friend class KaduWindow;
	ActionDescription *Configuration;
	ActionDescription *ShowYourAccounts;
	ActionDescription *ExitKadu;
	ActionDescription *AddUser;
	ActionDescription *MergeContact;
	ActionDescription *AddGroup;
	ActionDescription *OpenSearch;
	ActionDescription *ManageIgnored;
	ActionDescription *ManageModules;
	ActionDescription *ImportExportContacts;
	ActionDescription *Help;
	ActionDescription *Bugs;
	ActionDescription *Support;
	ActionDescription *GetInvolved;
	ActionDescription *About;

	ActionDescription *WriteEmail;
	ActionDescription *CopyDescription;
	ActionDescription *OpenDescriptionLink;
	ActionDescription *CopyPersonalInfo;
	ActionDescription *LookupUserInfo;
	ActionDescription *OfflineToUser;
	ActionDescription *HideDescription;
	ActionDescription *DeleteUsers;

	ActionDescription *InactiveUsers;
	ActionDescription *DescriptionUsers;
	ActionDescription *OnlineAndDescriptionUsers;
	ActionDescription *EditUser;
	ActionDescription *ShowStatus;
	ActionDescription *UseProxy;

private slots:
	void statusChanged(Account account, Status status);

	void inactiveUsersActionCreated(Action *action);
	void descriptionUsersActionCreated(Action *action);
	void onlineAndDescUsersActionCreated(Action *action);
	void editUserActionCreated(Action *action);
	void showStatusActionCreated(Action *action);
	void useProxyActionCreated(Action *action);

	void configurationActionActivated(QAction *sender, bool toggled);
	void yourAccountsActionActivated(QAction *sender, bool toggled);
	void exitKaduActionActivated(QAction *sender, bool toggled);
	void addUserActionActivated(QAction *sender, bool toggled);
	void mergeContactActionActivated(QAction *sender, bool toggled);
	void addGroupActionActivated(QAction *sender, bool toggled);
	void openSearchActionActivated(QAction *sender, bool toggled);
	void manageIgnoredActionActivated(QAction *sender, bool toggled);
	void importExportContactsActionActivated(QAction *sender, bool toggled);
	void helpActionActivated(QAction *sender, bool toggled);
	void bugsActionActivated(QAction *sender, bool toggled);
	void supportActionActivated(QAction *sender, bool toggled);
	void getInvolvedActionActivated(QAction *sender, bool toggled);
	void aboutActionActivated(QAction *sender, bool toggled);

	void writeEmailActionActivated(QAction *sender, bool toggled);
	void copyDescriptionActionActivated(QAction *sender, bool toggled);
	void openDescriptionLinkActionActivated(QAction *sender, bool toggled);
	void copyPersonalInfoActionActivated(QAction *sender, bool toggled);
	void lookupInDirectoryActionActivated(QAction *sender, bool toggled);
	void offlineToUserActionActivated(QAction *sender, bool toggled);
	void hideDescriptionActionActivated(QAction *sender, bool toggled);
	void deleteUsersActionActivated(QAction *sender, bool toggled);

	void inactiveUsersActionActivated(QAction *sender, bool toggled);
	void descriptionUsersActionActivated(QAction *sender, bool toggled);
	void onlineAndDescUsersActionActivated(QAction *sender, bool toggled);
	void editUserActionActivated(QAction *sender, bool toggled);
	void showStatusActionActivated(QAction *sender, bool toggled);
	void useProxyActionActivated(QAction *sender, bool toggled);

public:
	explicit KaduWindowActions(QObject *parent);
	virtual ~KaduWindowActions();

};

void disableNonIdUles(Action *action);
void disableContainsSelfUles(Action *action);
void checkOfflineTo(Action *action);
void checkHideDescription(Action *action);
void disableNotOneUles(Action *action);
void disableNoGaduUle(Action *action);
void disableNoGaduDescription(Action *action);
void disableNoGaduDescriptionUrl(Action *action);
void disableNoEMail(Action *action);

#include "accounts/account.h" // for MOC

#endif // KADU_WINDOW_ACTIONS_H
