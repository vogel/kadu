set (emoticons_SRCS
	emoticon-selector-button.cpp
	emoticon-selector.cpp
	emoticons-manager.cpp
	emots-walker.cpp
	prefix-node.cpp
)

set (emoticons_MOC_SRCS
	emoticon-selector-button.h
	emoticon-selector.h
	emoticons-manager.h
)

kadu_subdirectory (emoticons "${emoticons_SRCS}" "${emoticons_MOC_SRCS}" "")
