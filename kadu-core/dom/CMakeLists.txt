set (dom_SRCS
	dom-processor.cpp
	dom-processor-service.cpp
	dom-text-regexp-visitor.cpp
	ignore-links-dom-visitor.cpp
)

set (dom_MOC_SRCS
	dom-processor-service.h
)

kadu_subdirectory (dom "${dom_SRCS}" "${dom_MOC_SRCS}" "")
