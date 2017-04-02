set (avatars_SRCS
	aggregated-contact-avatar-service.cpp
	avatar.cpp
	avatar-job-manager.cpp
	avatar-job-runner.cpp
	avatar-manager.cpp
	avatar-module.cpp
	avatar-shared.cpp
	avatar-storage.cpp
	contact-avatar-service.cpp
)

kadu_subdirectory (avatars "${avatars_SRCS}" "" "")
