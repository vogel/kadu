set (url_handlers_SRCS
	mail-url-handler.cpp
	standard-url-handler.cpp
	url-handler-manager.cpp
)

add_library (url-handlers
	STATIC
	${url_handlers_SRCS}
)
