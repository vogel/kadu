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

#include "actions/chat/add-conference-action.h"
#include "actions/chat/add-room-chat-action.h"
#include "actions/chat/leave-chat-action.h"
#include "actions/chat-widget/auto-send-action.h"
#include "actions/chat-widget/block-user-action.h"
#include "actions/chat-widget/bold-action.h"
#include "actions/chat-widget/clear-chat-action.h"
#include "actions/chat-widget/insert-image-action.h"
#include "actions/chat-widget/italic-action.h"
#include "actions/chat-widget/more-actions-action.h"
#include "actions/chat-widget/open-chat-action.h"
#include "actions/chat-widget/open-chat-with-action.h"
#include "actions/chat-widget/send-action.h"
#include "actions/chat-widget/underline-action.h"
#include "actions/main/change-status-action.h"
#include "actions/main/default-proxy-action.h"
#include "actions/main/exit-action.h"
#include "actions/main/open-forum-action.h"
#include "actions/main/open-get-involved-action.h"
#include "actions/main/open-redmine-action.h"
#include "actions/main/open-translate-action.h"
#include "actions/main/recent-chats-action.h"
#include "actions/main/show-about-window-action.h"
#include "actions/main/show-blocked-buddies-action.h"
#include "actions/main/show-configuration-window-action.h"
#include "actions/main/show-descriptions-action.h"
#include "actions/main/show-file-transfer-window-action.h"
#include "actions/main/show-info-panel-action.h"
#include "actions/main/show-multilogons-action.h"
#include "actions/main/show-myself-action.h"
#include "actions/main/show-offline-buddies-action.h"
#include "actions/main/show-only-buddies-with-description-action.h"
#include "actions/main/show-only-buddies-with-description-or-online-action.h"
#include "actions/main/show-your-accounts-action.h"
#include "actions/main/toggle-silent-mode-action.h"
#include "actions/search//add-found-buddy-action.h"
#include "actions/search//chat-found-action.h"
#include "actions/search//clear-results-action.h"
#include "actions/search//first-search-action.h"
#include "actions/search//next-results-action.h"
#include "actions/search//stop-search-action.h"
#include "actions/talkable/add-group-action.h"
#include "actions/talkable/add-user-action.h"
#include "actions/talkable/copy-description-action.h"
#include "actions/talkable/copy-personal-info-action.h"
#include "actions/talkable/delete-talkable-action.h"
#include "actions/talkable/edit-talkable-action.h"
#include "actions/talkable/lookup-buddy-info-action.h"
#include "actions/talkable/merge-buddies-action.h"
#include "actions/talkable/notify-about-buddy-action.h"
#include "actions/talkable/open-buddy-email-action.h"
#include "actions/talkable/open-description-link-action.h"
#include "actions/talkable/open-search-action.h"
#include "actions/talkable/send-file-action.h"
#include "actions/tree-view/collapse-action.h"
#include "actions/tree-view/expand-action.h"

ActionsModule::ActionsModule()
{
	// chat
	add_type<AddConferenceAction>();
	add_type<AddRoomChatAction>();
	add_type<LeaveChatAction>();

	// chat-widget
	add_type<AutoSendAction>();
	add_type<BlockUserAction>();
	add_type<BoldAction>();
	add_type<ClearChatAction>();
	add_type<InsertImageAction>();
	add_type<ItalicAction>();
	add_type<MoreActionsAction>();
	add_type<OpenChatAction>();
	add_type<OpenChatWithAction>();
	add_type<SendAction>();
	add_type<UnderlineAction>();

	// main
	add_type<ChangeStatusAction>();
	add_type<DefaultProxyAction>();
	add_type<ExitAction>();
	add_type<OpenForumAction>();
	add_type<OpenGetInvolvedAction>();
	add_type<OpenRedmineAction>();
	add_type<OpenTranslateAction>();
	add_type<RecentChatsAction>();
	add_type<ShowAboutWindowAction>();
	add_type<ShowBlockedBuddiesAction>();
	add_type<ShowConfigurationWindowAction>();
	add_type<ShowDescriptionsAction>();
	add_type<ShowFileTransferWindowAction>();
	add_type<ShowInfoPanelAction>();
	add_type<ShowMultilogonsAction>();
	add_type<ShowMyselfAction>();
	add_type<ShowOfflineBuddiesAction>();
	add_type<ShowOnlyBuddiesWithDescriptionAction>();
	add_type<ShowOnlyBuddiesWithDescriptionOrOnlineAction>();
	add_type<ShowYourAccountsAction>();
	add_type<ToggleSilentModeAction>();

	// search
	add_type<AddFoundBuddyAction>();
	add_type<ChatFoundAction>();
	add_type<ClearResultsAction>();
	add_type<FirstSearchAction>();
	add_type<NextResultsAction>();
	add_type<StopSearchAction>();

	// talkable
	add_type<AddGroupAction>();
	add_type<AddUserAction>();
	add_type<CopyDescriptionAction>();
	add_type<CopyPersonalInfoAction>();
	add_type<DeleteTalkableAction>();
	add_type<EditTalkableAction>();
	add_type<LookupBuddyInfoAction>();
	add_type<MergeBuddiesAction>();
	add_type<NotifyAboutBuddyAction>();
	add_type<OpenBuddyEmailAction>();
	add_type<OpenDescriptionLinkAction>();
	add_type<OpenSearchAction>();
	add_type<SendFileAction>();

	// tree-view
	add_type<CollapseAction>();
	add_type<ExpandAction>();
}

ActionsModule::~ActionsModule()
{
}
