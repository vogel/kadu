set (formatted-string_SRCS
	composite-formatted-string.cpp
	force-space-dom-visitor.cpp
	formatted-string.cpp
	formatted-string-clone-visitor.cpp
	formatted-string-factory.cpp
	formatted-string-html-visitor.cpp
	formatted-string-image-block.cpp
	formatted-string-is-plain-text-visitor.cpp
	formatted-string-module.cpp
	formatted-string-plain-text-visitor.cpp
	formatted-string-text-block.cpp
)

kadu_add_test (force-space-dom-visitor)
kadu_add_test (formatted-string-factory)

kadu_subdirectory (formatted-string "${formatted-string_SRCS}" "" "")
