set (emoticons_SRCS
	animated-emoticon-path-provider.cpp
	emoticon.cpp
	emoticon-expander.cpp
	emoticon-prefix-tree.cpp
	emoticon-selector-button.cpp
	emoticon-selector.cpp
	emoticons-manager.cpp
	emots-walker.cpp
	static-emoticon-path-provider.cpp
)

set (emoticons_MOC_SRCS
	emoticon-selector-button.h
	emoticon-selector.h
	emoticons-manager.h
)

kadu_subdirectory (emoticons "${emoticons_SRCS}" "${emoticons_MOC_SRCS}" "")
