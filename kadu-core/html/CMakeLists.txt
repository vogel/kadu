set (html_SRCS
	html-conversion.cpp
	html-string.cpp
	normalized-html-string.cpp
)

kadu_add_test (html-conversion)

kadu_subdirectory (html "${html_SRCS}")
