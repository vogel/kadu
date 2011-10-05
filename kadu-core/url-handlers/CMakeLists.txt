set (url-handlers_SRCS
	mail-url-handler.cpp
	standard-url-handler.cpp
	url-handler-manager.cpp
)

kadu_subdirectory (url-handlers "${url-handlers_SRCS}" "" "")
