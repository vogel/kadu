set (actions_SRCS
	actions-module.cpp
	show-configuration-window-action.cpp
)

kadu_subdirectory (actions "${actions_SRCS}" "" "")
