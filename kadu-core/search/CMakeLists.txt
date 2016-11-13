set (search_SRCS
	actions/add-found-buddy-action.cpp
	actions/chat-found-action.cpp
	actions/clear-results-action.cpp
	actions/first-search-action.cpp
	actions/next-results-action.cpp
	actions/search-actions-module.cpp
	actions/stop-search-action.cpp
)

kadu_subdirectory (search "${search_SRCS}" "" "")
