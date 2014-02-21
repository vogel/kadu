set (chat-style_SRCS
	chat-style.cpp
	chat-style-manager.cpp
	html-messages-renderer.cpp

	engine/adium-style-engine/adium-chat-messages-renderer.cpp
	engine/adium-style-engine/adium-chat-messages-renderer-factory.cpp
	engine/adium-style-engine/adium-style.cpp
	engine/adium-style-engine/adium-time-formatter.cpp
	engine/adium-style-engine/adium-style-engine.cpp

	engine/kadu-style-engine/kadu-chat-messages-renderer.cpp
	engine/kadu-style-engine/kadu-chat-messages-renderer-factory.cpp
	engine/kadu-style-engine/kadu-chat-syntax.cpp
	engine/kadu-style-engine/kadu-style-engine.cpp

	engine/chat-messages-renderer.cpp
	engine/chat-messages-renderer-configuration.cpp
	engine/chat-messages-renderer-factory-provider.cpp
	engine/configured-chat-messages-renderer-factory-provider.cpp
)

kadu_subdirectory (chat-style "${chat-style_SRCS}" "" "")
