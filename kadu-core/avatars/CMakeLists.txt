set (avatars_SRCS
	avatar.cpp
	avatar-job-manager.cpp
	avatar-job-runner.cpp
	avatar-manager.cpp
	avatar-shared.cpp
)

set (avatars_MOC_SRCS
	avatar-job-manager.h
	avatar-job-runner.h
	avatar-manager.h
	avatar-shared.h
)

kadu_subdirectory (avatars "${avatars_SRCS}" "${avatars_MOC_SRCS}" "")
