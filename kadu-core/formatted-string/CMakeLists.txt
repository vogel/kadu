set (formatted-string_SRCS
	composite-formatted-string.cpp
	force-nbsp-dom-visitor.cpp
	formatted-string.cpp
	formatted-string-clone-visitor.cpp
	formatted-string-factory.cpp
	formatted-string-html-visitor.cpp
	formatted-string-image-block.cpp
	formatted-string-is-plain-text-visitor.cpp
	formatted-string-module.cpp
	formatted-string-plain-text-visitor.cpp
	formatted-string-text-block.cpp
	text-converter-service.cpp
)

kadu_add_test (force-nbsp-dom-visitor)
kadu_add_test (formatted-string-factory)
kadu_add_test (text-converter-service)

kadu_subdirectory (formatted-string "${formatted-string_SRCS}" "" "")
