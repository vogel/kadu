set (dom_SRCS
	dom-module.cpp
	dom-processor.cpp
	dom-processor-service.cpp
	dom-text-regexp-visitor.cpp
	dom-visitor-provider-repository.cpp
	ignore-links-dom-visitor.cpp
)

kadu_subdirectory (dom "${dom_SRCS}" "" "")
