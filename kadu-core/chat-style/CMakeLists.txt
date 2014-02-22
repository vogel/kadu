set (chat-style_SRCS
	chat-style.cpp
	chat-style-manager.cpp

	engine/adium/adium-style-renderer.cpp
	engine/adium/adium-style-renderer-factory.cpp
	engine/adium/adium-style.cpp
	engine/adium/adium-time-formatter.cpp
	engine/adium/adium-style-engine.cpp

	engine/kadu/kadu-style-renderer.cpp
	engine/kadu/kadu-style-renderer-factory.cpp
	engine/kadu/kadu-chat-syntax.cpp
	engine/kadu/kadu-style-engine.cpp

	engine/chat-style-renderer.cpp
	engine/chat-style-renderer-configuration.cpp
	engine/chat-style-renderer-factory-provider.cpp
	engine/configured-chat-style-renderer-factory-provider.cpp
)

kadu_subdirectory (chat-style "${chat-style_SRCS}" "" "")
