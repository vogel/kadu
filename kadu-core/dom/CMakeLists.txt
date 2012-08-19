set (dom_SRCS
	dom-processor.cpp
)

set (dom_MOC_SRCS
)

kadu_subdirectory (dom "${dom_SRCS}" "${dom_MOC_SRCS}" "")
