/*
 * %kadu copyright begin%
 * Copyright 2009, 2010, 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2012 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2009, 2009, 2010, 2011, 2012 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2011 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#ifndef KADU_WINDOW_ACTIONS_H
#define KADU_WINDOW_ACTIONS_H

#include <QtCore/QObject>

#include "configuration/configuration-aware-object.h"

#include "status/status.h"

class QAction;

class Action;
class ActionContext;
class ActionDescription;
class AddConferenceAction;
class AddRoomChatAction;
class ChangeStatusAction;
class DeleteTalkableAction;
class DefaultProxyAction;
class EditTalkableAction;
class MainWindow;
class RecentChatsAction;
class StatusContainer;

class KaduWindowActions : QObject, ConfigurationAwareObject
{
	Q_OBJECT

	friend class KaduWindow;
	ActionDescription *Configuration;
	ActionDescription *ShowYourAccounts;
	ActionDescription *ShowMultilogons;
	RecentChatsAction *RecentChats;
	ActionDescription *ExitKadu;
	ActionDescription *AddUser;
	AddConferenceAction *AddConference;
	AddRoomChatAction *AddRoomChat;
	ActionDescription *MergeContact;
	ActionDescription *AddGroup;
	ActionDescription *OpenSearch;
	ActionDescription *Help;
	ActionDescription *Bugs;
	ActionDescription *GetInvolved;
	ActionDescription *About;
	ActionDescription *Translate;
	ActionDescription *ShowInfoPanel;
	ActionDescription *ShowBlockedBuddies;
	ActionDescription *ShowMyself;

	ActionDescription *WriteEmail;
	ActionDescription *CopyDescription;
	ActionDescription *OpenDescriptionLink;
	ActionDescription *CopyPersonalInfo;
	ActionDescription *LookupUserInfo;
	DeleteTalkableAction *DeleteTalkable;

	ActionDescription *InactiveUsers;
	ActionDescription *DescriptionUsers;
	ActionDescription *ShowDescriptions;
	ActionDescription *OnlineAndDescriptionUsers;

	EditTalkableAction *EditTalkable;
	ChangeStatusAction *ChangeStatus;
	DefaultProxyAction *DefaultProxy;

private slots:
	void showMultilogonsActionCreated(Action *action);
	void openSearchActionCreated(Action *action);
	void inactiveUsersActionCreated(Action *action);
	void descriptionUsersActionCreated(Action *action);
	void showDescriptionsActionCreated(Action *action);
	void onlineAndDescUsersActionCreated(Action *action);
	void showInfoPanelActionCreated(Action *action);
	void showBlockedActionCreated(Action *action);
	void showMyselfActionCreated(Action *action);
	void writeEmailActionCreated(Action *action);

	void configurationActionActivated(QAction *sender, bool toggled);
	void yourAccountsActionActivated(QAction *sender, bool toggled);
	void showMultilogonsActionActivated(QAction *sender, bool toggled);
	void exitKaduActionActivated(QAction *sender, bool toggled);
	void addUserActionActivated(QAction *sender, bool toggled);
	void mergeContactActionActivated(QAction *sender, bool toggled);
	void addGroupActionActivated(QAction *sender, bool toggled);
	void openSearchActionActivated(QAction *sender, bool toggled);
	void helpActionActivated(QAction *sender, bool toggled);
	void bugsActionActivated(QAction *sender, bool toggled);
	void getInvolvedActionActivated(QAction *sender, bool toggled);
	void aboutActionActivated(QAction *sender, bool toggled);
	void translateActionActivated(QAction *sender, bool toggled);
	void showInfoPanelActionActivated(QAction *sender, bool toggled);
	void showBlockedActionActivated(QAction *sender, bool toggled);
	void showMyselfActionActivated(QAction *sender, bool toggled);

	void writeEmailActionActivated(QAction *sender, bool toggled);
	void copyDescriptionActionActivated(QAction *sender, bool toggled);
	void openDescriptionLinkActionActivated(QAction *sender, bool toggled);
	void copyPersonalInfoActionActivated(QAction *sender, bool toggled);
	void lookupInDirectoryActionActivated(QAction *sender, bool toggled);

	void inactiveUsersActionActivated(QAction *sender, bool toggled);
	void descriptionUsersActionActivated(QAction *sender, bool toggled);
	void showDescriptionsActionActivated(QAction *sender, bool toggled);
	void onlineAndDescUsersActionActivated(QAction *sender, bool toggled);

protected:
	virtual void configurationUpdated();

public:
	explicit KaduWindowActions(QObject *parent);
	virtual ~KaduWindowActions();

	AddConferenceAction * addConference() { return AddConference; }
	AddRoomChatAction * addRoomChat() { return AddRoomChat; }
	EditTalkableAction * editTalkable() { return EditTalkable; }
	DeleteTalkableAction * deleteTalkable() { return DeleteTalkable; }

};

void disableNonIdUles(Action *action);
void disableNotOneUles(Action *action);
void disableNoContact(Action *action);
void disableNoDescription(Action *action);
void disableNoDescriptionUrl(Action *action);
void disableNoEMail(Action *action);

#endif // KADU_WINDOW_ACTIONS_H
