/*
 * %kadu copyright begin%
 * Copyright 2009, 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009, 2010, 2012 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2010, 2011 Piotr Dąbrowski (ultr@ultr.pl)
 * Copyright 2009 Michał Podsiadlik (michal@kadu.net)
 * Copyright 2009 Bartłomiej Zimoń (uzi18@o2.pl)
 * Copyright 2010 Radosław Szymczyszyn (lavrin@gmail.com)
 * Copyright 2010, 2011, 2012, 2013, 2014 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2009, 2010, 2011, 2012, 2013, 2014, 2015 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2009 Longer (longer89@gmail.com)
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

#include "kadu-window-actions.h"

#include "actions/add-group-action.h"
#include "actions/add-user-action.h"
#include "actions/copy-description-action.h"
#include "actions/copy-personal-info-action.h"
#include "actions/exit-action.h"
#include "actions/lookup-buddy-info-action.h"
#include "actions/merge-buddies-action.h"
#include "actions/open-buddy-email-action.h"
#include "actions/open-description-link-action.h"
#include "actions/open-forum-action.h"
#include "actions/open-get-involved-action.h"
#include "actions/open-redmine-action.h"
#include "actions/open-search-action.h"
#include "actions/open-translate-action.h"
#include "actions/show-about-window-action.h"
#include "actions/show-blocked-buddies-action.h"
#include "actions/show-configuration-window-action.h"
#include "actions/show-descriptions-action.h"
#include "actions/show-info-panel-action.h"
#include "actions/show-multilogons-action.h"
#include "actions/show-myself-action.h"
#include "actions/show-offline-buddies-action.h"
#include "actions/show-only-buddies-with-description-action.h"
#include "actions/show-only-buddies-with-description-or-online-action.h"
#include "actions/show-your-accounts-action.h"
#include "gui/actions/change-status-action.h"
#include "gui/actions/chat/add-conference-action.h"
#include "gui/actions/chat/add-room-chat-action.h"
#include "gui/actions/default-proxy-action.h"
#include "gui/actions/delete-talkable-action.h"
#include "gui/actions/edit-talkable-action.h"
#include "gui/actions/recent-chats-action.h"
#include "gui/actions/talkable-tree-view/collapse-action.h"
#include "gui/actions/talkable-tree-view/expand-action.h"
#include "gui/menu/menu-inventory.h"
#include "gui/widgets/chat-widget/actions/chat-widget-actions.h"

KaduWindowActions::KaduWindowActions(QObject *parent) : QObject(parent)
{
}

KaduWindowActions::~KaduWindowActions()
{
}

void KaduWindowActions::setAddConferenceAction(AddConferenceAction *addConferenceAction)
{
	m_addConferenceAction = addConferenceAction;
}

void KaduWindowActions::setAddGroupAction(AddGroupAction *addGroupAction)
{
	m_addGroupAction = addGroupAction;
}

void KaduWindowActions::setAddRoomChatAction(AddRoomChatAction *addRoomChatAction)
{
	m_addRoomChatAction = addRoomChatAction;
}

void KaduWindowActions::setAddUserAction(AddUserAction *addUserAction)
{
	m_addUserAction = addUserAction;
}

void KaduWindowActions::setChatWidgetActions(ChatWidgetActions *chatWidgetActions)
{
	m_chatWidgetActions = chatWidgetActions;
}

void KaduWindowActions::setChangeStatusAction(ChangeStatusAction *changeStatusAction)
{
	m_changeStatusAction = changeStatusAction;
}

void KaduWindowActions::setCollapseAction(CollapseAction *collapseAction)
{
	m_collapseAction = collapseAction;
}

void KaduWindowActions::setCopyDescriptionAction(CopyDescriptionAction *copyDescriptionAction)
{
	m_copyDescriptionAction = copyDescriptionAction;
}

void KaduWindowActions::setCopyPersonalInfoAction(CopyPersonalInfoAction *copyPersonalInfoAction)
{
	m_copyPersonalInfoAction = copyPersonalInfoAction;
}

void KaduWindowActions::setDefaultProxyAction(DefaultProxyAction *defaultProxyAction)
{
	m_defaultProxyAction = defaultProxyAction;
}

void KaduWindowActions::setDeleteTalkableAction(DeleteTalkableAction *deleteTalkableAction)
{
	m_deleteTalkableAction = deleteTalkableAction;
}

void KaduWindowActions::setEditTalkableAction(EditTalkableAction *editTalkableAction)
{
	m_editTalkableAction = editTalkableAction;
}

void KaduWindowActions::setExpandAction(ExpandAction *expandAction)
{
	m_expandAction = expandAction;
}

void KaduWindowActions::setExitAction(ExitAction *exitAction)
{
	m_exitAction = exitAction;
}

void KaduWindowActions::setLookupBuddyInfoAction(LookupBuddyInfoAction *lookupBuddyInfoAction)
{
	m_lookupBuddyInfoAction = lookupBuddyInfoAction;
}

void KaduWindowActions::setMenuInventory(MenuInventory *menuInventory)
{
	m_menuInventory = menuInventory;
}

void KaduWindowActions::setMergeBuddiesAction(MergeBuddiesAction *mergeBuddiesAction)
{
	m_mergeBuddiesAction = mergeBuddiesAction;
}

void KaduWindowActions::setOpenBuddyEmailAction(OpenBuddyEmailAction *openBuddyEmailAction)
{
	m_openBuddyEmailAction = openBuddyEmailAction;
}

void KaduWindowActions::setOpenDescriptionLinkAction(OpenDescriptionLinkAction *openDescriptionLinkAction)
{
	m_openDescriptionLinkAction = openDescriptionLinkAction;
}

void KaduWindowActions::setOpenForumAction(OpenForumAction *openForumAction)
{
	m_openForumAction = openForumAction;
}

void KaduWindowActions::setOpenGetInvolvedAction(OpenGetInvolvedAction *openGetInvolvedAction)
{
	m_openGetInvolvedAction = openGetInvolvedAction;
}

void KaduWindowActions::setOpenRedmineAction(OpenRedmineAction *openRedmineAction)
{
	m_openRedmineAction = openRedmineAction;
}

void KaduWindowActions::setOpenSearchAction(OpenSearchAction *openSearchAction)
{
	m_openSearchAction = openSearchAction;
}

void KaduWindowActions::setOpenTranslateAction(OpenTranslateAction *openTranslateAction)
{
	m_openTranslateAction = openTranslateAction;
}

void KaduWindowActions::setRecentChatsAction(RecentChatsAction *recentChatsAction)
{
	m_recentChatsAction = recentChatsAction;
}

void KaduWindowActions::setShowAboutWindowAction(ShowAboutWindowAction *showAboutWindowAction)
{
	m_showAboutWindowAction = showAboutWindowAction;
}

void KaduWindowActions::setShowBlockedBuddiesAction(ShowBlockedBuddiesAction *showBlockedBuddiesAction)
{
	m_showBlockedBuddiesAction = showBlockedBuddiesAction;
}

void KaduWindowActions::setShowConfigurationWindowAction(ShowConfigurationWindowAction *showConfigurationWindowAction)
{
	m_showConfigurationWindowAction = showConfigurationWindowAction;
}

void KaduWindowActions::setShowDescriptionsAction(ShowDescriptionsAction *showDescriptionsAction)
{
	m_showDescriptionsAction = showDescriptionsAction;
}

void KaduWindowActions::setShowInfoPanelAction(ShowInfoPanelAction *showInfoPanelAction)
{
	m_showInfoPanelAction = showInfoPanelAction;
}

void KaduWindowActions::setShowMultilogonsAction(ShowMultilogonsAction *showMultilogonsAction)
{
	m_showMultilogonsAction = showMultilogonsAction;
}

void KaduWindowActions::setShowMyselfAction(ShowMyselfAction *showMyselfAction)
{
	m_showMyselfAction = showMyselfAction;
}

void KaduWindowActions::setShowOfflineBuddiesAction(ShowOfflineBuddiesAction *showOfflineBuddiesAction)
{
	m_showOfflineBuddiesAction = showOfflineBuddiesAction;
}

void KaduWindowActions::setShowOnlyBuddiesWithDescriptionAction(ShowOnlyBuddiesWithDescriptionAction *showOnlyBuddiesWithDescriptionAction)
{
	m_showOnlyBuddiesWithDescriptionAction = showOnlyBuddiesWithDescriptionAction;
}

void KaduWindowActions::setShowOnlyBuddiesWithDescriptionOrOnlineAction(ShowOnlyBuddiesWithDescriptionOrOnlineAction *showOnlyBuddiesWithDescriptionOrOnlineAction)
{
	m_showOnlyBuddiesWithDescriptionOrOnlineAction = showOnlyBuddiesWithDescriptionOrOnlineAction;
}

void KaduWindowActions::setShowYourAccountsAction(ShowYourAccountsAction *showYourAccountsAction)
{
	m_showYourAccountsAction = showYourAccountsAction;
}

void KaduWindowActions::init()
{
	m_menuInventory
		->menu("buddy-list")
		->addAction(m_expandAction, KaduMenu::SectionActionsGui, 2);
	m_menuInventory
		->menu("buddy-list")
		->addAction(m_collapseAction, KaduMenu::SectionActionsGui, 1);

	m_menuInventory
		->menu("buddy-list")
		->addAction(m_copyDescriptionAction, KaduMenu::SectionActions, 10);

	m_menuInventory
		->menu("buddy-list")
		->addAction(m_copyPersonalInfoAction, KaduMenu::SectionActions, 20);

	m_menuInventory
		->menu("buddy-list")
		->addAction(m_openDescriptionLinkAction, KaduMenu::SectionActions, 30);

	m_menuInventory
		->menu("buddy-list")
		->addAction(m_openBuddyEmailAction, KaduMenu::SectionSend, 200);

	m_menuInventory
		->menu("buddy-list")
		->addAction(m_editTalkableAction, KaduMenu::SectionView);

	m_menuInventory
		->menu("buddy-list")
		->addAction(m_mergeBuddiesAction, KaduMenu::SectionManagement, 100);

	m_menuInventory
		->menu("buddy-list")
		->addAction(m_chatWidgetActions->blockUser(), KaduMenu::SectionManagement, 500);

	m_menuInventory
		->menu("buddy-list")
		->addAction(m_deleteTalkableAction, KaduMenu::SectionManagement, 1000);
}

#include "moc_kadu-window-actions.cpp"
