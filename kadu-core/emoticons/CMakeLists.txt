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

qt4_wrap_cpp (emoticons_MOC_FILES ${emoticons_MOC_SRCS})

add_library (emoticons
	STATIC
	${emoticons_SRCS}
	${emoticons_MOC_FILES}
)

set_target_properties (emoticons PROPERTIES COMPILE_DEFINITIONS KADULIB)
