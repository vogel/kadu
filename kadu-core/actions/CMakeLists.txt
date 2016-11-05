set (actions_SRCS
	actions-module.cpp
	show-configuration-window-action.cpp
	show-multilogons-action.cpp
	show-your-accounts-action.cpp
)

kadu_subdirectory (actions "${actions_SRCS}" "" "")
