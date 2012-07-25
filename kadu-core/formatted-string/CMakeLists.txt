set (formatted-string_SRCS
	formatted-message.cpp
	formatted-message-part.cpp
	formatted-string-factory.cpp
)

set (formatted-string_MOC_SRCS
	formatted-string-factory.h
)

kadu_subdirectory (formatted-string "${formatted-string_SRCS}" "${formatted-string_MOC_SRCS}" "")
