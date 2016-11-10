/*
 * %kadu copyright begin%
 * Copyright 2014, 2015 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "actions-module.h"

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
#include "gui/actions/chat/add-conference-action.h"
#include "gui/actions/chat/add-room-chat-action.h"
#include "gui/actions/recent-chats-action.h"
#include "gui/actions/talkable-tree-view/expand-action.h"

ActionsModule::ActionsModule()
{
	add_type<AddConferenceAction>();
	add_type<AddGroupAction>();
	add_type<AddRoomChatAction>();
	add_type<AddUserAction>();
	add_type<CopyDescriptionAction>();
	add_type<CopyPersonalInfoAction>();
	add_type<ExitAction>();
	add_type<ExpandAction>();
	add_type<LookupBuddyInfoAction>();
	add_type<MergeBuddiesAction>();
	add_type<OpenBuddyEmailAction>();
	add_type<OpenDescriptionLinkAction>();
	add_type<OpenForumAction>();
	add_type<OpenGetInvolvedAction>();
	add_type<OpenRedmineAction>();
	add_type<OpenSearchAction>();
	add_type<OpenTranslateAction>();
	add_type<RecentChatsAction>();
	add_type<ShowAboutWindowAction>();
	add_type<ShowBlockedBuddiesAction>();
	add_type<ShowConfigurationWindowAction>();
	add_type<ShowDescriptionsAction>();
	add_type<ShowInfoPanelAction>();
	add_type<ShowMultilogonsAction>();
	add_type<ShowMyselfAction>();
	add_type<ShowOfflineBuddiesAction>();
	add_type<ShowOnlyBuddiesWithDescriptionAction>();
	add_type<ShowOnlyBuddiesWithDescriptionOrOnlineAction>();
	add_type<ShowYourAccountsAction>();
}

ActionsModule::~ActionsModule()
{
}
