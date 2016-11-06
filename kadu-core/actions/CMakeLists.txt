set (actions_SRCS
	actions-module.cpp
	add-group-action.cpp
	add-user-action.cpp
	exit-action.cpp
	open-forum-action.cpp
	open-get-involved-action.cpp
	open-redmine-action.cpp
	open-search-action.cpp
	show-configuration-window-action.cpp
	show-multilogons-action.cpp
	show-your-accounts-action.cpp
)

kadu_subdirectory (actions "${actions_SRCS}" "" "")
