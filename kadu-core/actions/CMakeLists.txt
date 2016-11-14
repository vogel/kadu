set (actions_SRCS
	actions-module.cpp
	add-group-action.cpp
	add-user-action.cpp
	copy-description-action.cpp
	copy-personal-info-action.cpp
	exit-action.cpp
	lookup-buddy-info-action.cpp
	merge-buddies-action.cpp
	notify-about-buddy-action.cpp
	open-buddy-email-action.cpp
	open-description-link-action.cpp
	open-forum-action.cpp
	open-get-involved-action.cpp
	open-redmine-action.cpp
	open-search-action.cpp
	open-translate-action.cpp
	show-about-window-action.cpp
	show-blocked-buddies-action.cpp
	show-configuration-window-action.cpp
	show-descriptions-action.cpp
	show-info-panel-action.cpp
	show-multilogons-action.cpp
	show-myself-action.cpp
	show-offline-buddies-action.cpp
	show-only-buddies-with-description-action.cpp
	show-only-buddies-with-description-or-online-action.cpp
	show-your-accounts-action.cpp
	toggle-silent-mode-action.cpp
)

kadu_subdirectory (actions "${actions_SRCS}" "" "")
