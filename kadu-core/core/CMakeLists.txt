set (core_SRCS
	core.cpp
	core-actions.cpp
	crash-aware-object.cpp
)

set (core_MOC_SRCS
	core.h
)

kadu_subdirectory (core "${core_SRCS}" "${core_MOC_SRCS}" "")
