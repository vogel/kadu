set (dom_SRCS
	dom-processor.cpp
	dom-text-regexp-callback.cpp
)

set (dom_MOC_SRCS
)

kadu_subdirectory (dom "${dom_SRCS}" "${dom_MOC_SRCS}" "")
