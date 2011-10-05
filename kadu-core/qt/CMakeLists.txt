set (qt_SRCS
	long-validator.cpp
)

set (qt_MOC_SRCS
	long-validator.h
)

kadu_subdirectory (qt "${qt_SRCS}" "${qt_MOC_SRCS}" "")
