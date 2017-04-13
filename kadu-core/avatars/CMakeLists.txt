set (avatars_SRCS
	account-avatar-service.cpp
	aggregated-account-avatar-service.cpp
	aggregated-contact-avatar-service.cpp
	avatars.cpp
	avatar-id.cpp
	avatar-module.cpp
	contact-avatar-downloader.cpp
	contact-avatar-global-id.cpp
	contact-avatar-id.cpp
	contact-avatar-service.cpp
	serial-avatar-downloader.cpp
)

kadu_subdirectory (avatars "${avatars_SRCS}" "" "")
