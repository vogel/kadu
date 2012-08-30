set (emoticons_SRCS
	animated-emoticon-path-provider.cpp
	emoticon.cpp
	emoticon-expander.cpp
	emoticon-expander-dom-visitor-provider.cpp
	emoticon-prefix-tree.cpp
	emoticon-prefix-tree-builder.cpp
	emoticon-selector-button.cpp
	emoticon-selector.cpp
	emoticon-theme-manager.cpp
	emoticon-walker.cpp
	emoticons-configuration-ui-handler.cpp
	emoticons-manager.cpp
	insert-emoticon-action.cpp
	static-emoticon-path-provider.cpp
)

set (emoticons_MOC_SRCS
	emoticon-selector-button.h
	emoticon-selector.h
	emoticon-theme-manager.h
	emoticons-configuration-ui-handler.h
	emoticons-manager.h
	insert-emoticon-action.h
)

kadu_subdirectory (emoticons "${emoticons_SRCS}" "${emoticons_MOC_SRCS}" "")
