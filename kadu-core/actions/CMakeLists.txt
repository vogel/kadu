set (actions_SRCS
	chat/leave-chat-action.cpp
	chat/add-conference-action.cpp
	chat/add-room-chat-action.cpp

	chat-widget/italic-action.cpp
	chat-widget/block-user-action.cpp
	chat-widget/underline-action.cpp
	chat-widget/open-chat-with-action.cpp
	chat-widget/send-action.cpp
	chat-widget/more-actions-action.cpp
	chat-widget/bold-action.cpp
	chat-widget/clear-chat-action.cpp
	chat-widget/auto-send-action.cpp
	chat-widget/open-chat-action.cpp
	chat-widget/insert-image-action.cpp

	main/open-forum-action.cpp
	main/change-status-action.cpp
	main/show-about-window-action.cpp
	main/show-only-buddies-with-description-or-online-action.cpp
	main/show-only-buddies-with-description-action.cpp
	main/show-configuration-window-action.cpp
	main/exit-action.cpp
	main/show-blocked-buddies-action.cpp
	main/show-offline-buddies-action.cpp
	main/show-myself-action.cpp
	main/open-redmine-action.cpp
	main/recent-chats-action.cpp
	main/open-get-involved-action.cpp
	main/show-info-panel-action.cpp
	main/default-proxy-action.cpp
	main/toggle-silent-mode-action.cpp
	main/show-file-transfer-window-action.cpp
	main/open-translate-action.cpp
	main/show-descriptions-action.cpp
	main/show-your-accounts-action.cpp
	main/show-multilogons-action.cpp

	search/first-search-action.cpp
	search/stop-search-action.cpp
	search/chat-found-action.cpp
	search/clear-results-action.cpp
	search/add-found-buddy-action.cpp
	search/next-results-action.cpp

	talkable/lookup-buddy-info-action.cpp
	talkable/add-group-action.cpp
	talkable/delete-talkable-action.cpp
	talkable/copy-personal-info-action.cpp
	talkable/edit-talkable-action.cpp
	talkable/open-search-action.cpp
	talkable/open-description-link-action.cpp
	talkable/add-user-action.cpp
	talkable/copy-description-action.cpp
	talkable/merge-buddies-action.cpp
	talkable/open-buddy-email-action.cpp
	talkable/send-file-action.cpp
	talkable/notify-about-buddy-action.cpp

	tree-view/collapse-action.cpp
	tree-view/expand-action.cpp

	action-context.cpp
	action-description.cpp
	actions-module.cpp
	actions.cpp
	action.cpp
	base-action-context.cpp
	injector-registered-actions.cpp
)

kadu_subdirectory (actions "${actions_SRCS}" "" "")
