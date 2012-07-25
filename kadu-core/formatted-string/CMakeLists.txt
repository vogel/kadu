set (formatted-string_SRCS
	formatted-string.cpp
	formatted-string-factory.cpp
	formatted-string-part.cpp
)

set (formatted-string_MOC_SRCS
	formatted-string-factory.h
)

kadu_subdirectory (formatted-string "${formatted-string_SRCS}" "${formatted-string_MOC_SRCS}" "")
