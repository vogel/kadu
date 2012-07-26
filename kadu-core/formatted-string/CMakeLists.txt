set (formatted-string_SRCS
	composite-formatted-string.cpp
	formatted-string-factory.cpp
	formatted-string-html-visitor.cpp
	formatted-string-image-block.cpp
	formatted-string-part.cpp
	formatted-string-plain-text-visitor.cpp
)

set (formatted-string_MOC_SRCS
	formatted-string-factory.h
)

kadu_subdirectory (formatted-string "${formatted-string_SRCS}" "${formatted-string_MOC_SRCS}" "")
