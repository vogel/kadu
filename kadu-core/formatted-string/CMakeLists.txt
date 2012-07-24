set (formatted-string_SRCS
	formatted-message.cpp
	formatted-message-part.cpp
	formatted-string-factory.cpp
)

set (formatted-string_MOC_SRCS
)

kadu_subdirectory (formatted-string "${formatted-string_SRCS}" "${formatted-string_MOC_SRCS}" "")
