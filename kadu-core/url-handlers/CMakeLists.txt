set (url-handlers_SRCS
	mail-url-dom-visitor-provider.cpp
	mail-url-expander.cpp
	mail-url-handler.cpp
	simple-url-expander.cpp
	standard-url-dom-visitor-provider.cpp
	standard-url-expander.cpp
	standard-url-expander-configuration.cpp
	standard-url-expander-configurator.cpp
	standard-url-handler.cpp
	url-clipboard-html-transformer.cpp
	url-handler-manager.cpp
)

set (url-handlers_MOC_SRCS
	standard-url-expander.h
)

kadu_subdirectory (url-handlers "${url-handlers_SRCS}" "${url-handlers_MOC_SRCS}" "")
