set (roster_SRCS
	roster.cpp
	roster-entry.cpp
	roster-module.cpp
	roster-notifier.cpp
	roster-replacer.cpp
	roster-service.cpp
	roster-service-tasks.cpp
	roster-task.cpp
	roster-task-collection-storage.cpp
)

kadu_subdirectory (roster "${roster_SRCS}" "" "")
