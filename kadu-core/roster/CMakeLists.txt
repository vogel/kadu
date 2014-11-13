set (roster_SRCS
	roster.cpp
	roster-entry.cpp
	roster-notifier.cpp
	roster-service.cpp
	roster-task.cpp
	roster-task-collection-storage.cpp
)

kadu_subdirectory (roster "${roster_SRCS}" "" "")
