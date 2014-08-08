set (core_SRCS
	application.cpp
	core.cpp
	core-module.cpp
	crash-aware-object.cpp
)

set (core_MOC_SRCS
	core.h
)

kadu_subdirectory (core "${core_SRCS}" "${core_MOC_SRCS}" "")
