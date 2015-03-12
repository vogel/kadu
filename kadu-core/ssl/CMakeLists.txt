set (ssl_SRCS
	gui/ssl-certificate-error-dialog.cpp
	gui/ssl-certificate-widget.cpp

	ssl-certificate-manager.cpp
	ssl-certificate-repository.cpp
	ssl-certificate-storage.cpp
	ssl-certificate.cpp
	ssl-module.cpp
)

kadu_subdirectory (ssl "${ssl_SRCS}" "" "")
