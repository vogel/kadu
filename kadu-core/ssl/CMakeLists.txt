set (ssl_SRCS
	gui/ssl-certificate-widget.cpp
	gui/ssl-certificate-window.cpp

	ssl-certificate-manager.cpp
	ssl-certificate-repository.cpp
	ssl-certificate-storage.cpp
	ssl-module.cpp
)

kadu_subdirectory (ssl "${ssl_SRCS}" "" "")
