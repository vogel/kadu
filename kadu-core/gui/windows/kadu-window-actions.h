/*
 * %kadu copyright begin%
 * Copyright 2009, 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2012 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2011 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2009, 2010, 2011, 2012, 2013, 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#pragma once

#include <QtCore/QObject>
#include <QtCore/QPointer>
#include <injeqt/injeqt.h>

#include "configuration/configuration-aware-object.h"

#include "status/status.h"

class QAction;

class AccountManager;
class ActionContext;
class ActionDescription;
class Actions;
class Action;
class AddConferenceAction;
class AddGroupAction;
class AddUserAction;
class AddRoomChatAction;
class Application;
class ChangeStatusAction;
class ChatWidgetActions;
class Configuration;
class CopyDescriptionAction;
class DeleteTalkableAction;
class DefaultProxyAction;
class EditTalkableAction;
class ExitAction;
class InjectedFactory;
class KaduWindowService;
class MainConfigurationWindowService;
class MainWindow;
class MenuInventory;
class MultilogonWindowService;
class Myself;
class OpenForumAction;
class OpenGetInvolvedAction;
class OpenRedmineAction;
class OpenSearchAction;
class OpenTranslateAction;
class Parser;
class RecentChatsAction;
class ShowAboutWindowAction;
class ShowBlockedBuddiesAction;
class ShowConfigurationWindowAction;
class ShowInfoPanelAction;
class ShowMultilogonsAction;
class ShowMyselfAction;
class ShowYourAccountsAction;
class StatusContainer;
class UrlHandlerManager;
class UrlOpener;
class YourAccountsWindowService;

class KaduWindowActions : public QObject, ConfigurationAwareObject
{
	Q_OBJECT

	QPointer<AccountManager> m_accountManager;
	QPointer<Actions> m_actions;
	QPointer<AddGroupAction> m_addGroupAction;
	QPointer<AddUserAction> m_addUserAction;
	QPointer<Application> m_application;
	QPointer<ChatWidgetActions> m_chatWidgetActions;
	QPointer<Configuration> m_configuration;
	QPointer<CopyDescriptionAction> m_copyDescriptionAction;
	QPointer<ExitAction> m_exitAction;
	QPointer<InjectedFactory> m_injectedFactory;
	QPointer<KaduWindowService> m_kaduWindowService;
	QPointer<MainConfigurationWindowService> m_mainConfigurationWindowService;
	QPointer<MenuInventory> m_menuInventory;
	QPointer<MultilogonWindowService> m_multilogonWindowService;
	QPointer<Myself> m_myself;
	QPointer<OpenForumAction> m_openForumAction;
	QPointer<OpenGetInvolvedAction> m_openGetInvolvedAction;
	QPointer<OpenRedmineAction> m_openRedmineAction;
	QPointer<OpenSearchAction> m_openSearchAction;
	QPointer<OpenTranslateAction> m_openTranslateAction;
	QPointer<Parser> m_parser;
	QPointer<ShowAboutWindowAction> m_showAboutWindowAction;
	QPointer<ShowBlockedBuddiesAction> m_showBlockedBuddiesAction;
	QPointer<ShowConfigurationWindowAction> m_showConfigurationWindowAction;
	QPointer<ShowInfoPanelAction> m_showInfoPanelAction;
	QPointer<ShowMultilogonsAction> m_showMultilogonsAction;
	QPointer<ShowMyselfAction> m_showMyselfAction;
	QPointer<ShowYourAccountsAction> m_showYourAccountsAction;
	QPointer<UrlHandlerManager> m_urlHandlerManager;
	QPointer<UrlOpener> m_urlOpener;
	QPointer<YourAccountsWindowService> m_yourAccountsWindowService;

	friend class KaduWindow;
	RecentChatsAction *RecentChats;
	AddConferenceAction *AddConference;
	AddRoomChatAction *AddRoomChat;
	ActionDescription *MergeContact;

	ActionDescription *WriteEmail;
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
	INJEQT_SET void setAccountManager(AccountManager *accountManager);
	INJEQT_SET void setActions(Actions *actions);
	INJEQT_SET void setAddGroupAction(AddGroupAction *addGroupAction);
	INJEQT_SET void setAddUserAction(AddUserAction *addUserAction);
	INJEQT_SET void setApplication(Application *application);
	INJEQT_SET void setChatWidgetActions(ChatWidgetActions *chatWidgetActions);
	INJEQT_SET void setConfiguration(Configuration *configuration);
	INJEQT_SET void setCopyDescriptionAction(CopyDescriptionAction *copyDescriptionAction);
	INJEQT_SET void setExitAction(ExitAction *exitAction);
	INJEQT_SET void setInjectedFactory(InjectedFactory *injectedFactory);
	INJEQT_SET void setKaduWindowService(KaduWindowService *kaduWindowService);
	INJEQT_SET void setMainConfigurationWindowService(MainConfigurationWindowService *mainConfigurationWindowService);
	INJEQT_SET void setMenuInventory(MenuInventory *menuInventory);
	INJEQT_SET void setMultilogonWindowService(MultilogonWindowService *multilogonWindowService);
	INJEQT_SET void setMyself(Myself *myself);
	INJEQT_SET void setOpenForumAction(OpenForumAction *openForumAction);
	INJEQT_SET void setOpenGetInvolvedAction(OpenGetInvolvedAction *openGetInvolvedAction);
	INJEQT_SET void setOpenRedmineAction(OpenRedmineAction *openRedmineAction);
	INJEQT_SET void setOpenTranslateAction(OpenTranslateAction *openTranslateAction);
	INJEQT_SET void setOpenSearchAction(OpenSearchAction *openSearchAction);
	INJEQT_SET void setParser(Parser *parser);
	INJEQT_SET void setShowAboutWindowAction(ShowAboutWindowAction *showAboutWindowAction);
	INJEQT_SET void setShowBlockedBuddiesAction(ShowBlockedBuddiesAction *showBlockedBuddiesAction);
	INJEQT_SET void setShowConfigurationWindowAction(ShowConfigurationWindowAction *showConfigurationWindowAction);
	INJEQT_SET void setShowInfoPanelAction(ShowInfoPanelAction *showInfoPanelAction);
	INJEQT_SET void setShowMultilogonsAction(ShowMultilogonsAction *showMultilogonsAction);
	INJEQT_SET void setShowMyselfAction(ShowMyselfAction *showMyselfAction);
	INJEQT_SET void setShowYourAccountsAction(ShowYourAccountsAction *showYourAccountsAction);
	INJEQT_SET void setUrlHandlerManager(UrlHandlerManager *urlHandlerManager);
	INJEQT_SET void setUrlOpener(UrlOpener *urlOpener);
	INJEQT_SET void setYourAccountsWindowService(YourAccountsWindowService *yourAccountsWindowService);
	INJEQT_INIT void init();

	void inactiveUsersActionCreated(Action *action);
	void descriptionUsersActionCreated(Action *action);
	void showDescriptionsActionCreated(Action *action);
	void onlineAndDescUsersActionCreated(Action *action);
	void writeEmailActionCreated(Action *action);

	void mergeContactActionActivated(QAction *sender, bool toggled);

	void writeEmailActionActivated(QAction *sender, bool toggled);
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

void disableNoContact(Action *action);
void disableNoDescription(Action *action);
void disableNoDescriptionUrl(Action *action);
void disableNoEMail(Action *action);
