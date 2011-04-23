set (COMPILE_PLUGINS
	# Enables notifications about buddies presence and other in chat windows
	chat_notifier
	# Shows image links and youtube links as images and videos in chat
#	imagelink
)

foreach (PLUGIN ${COMPILE_PLUGINS})
	message (STATUS "Plugin: " ${PLUGIN})
	if (WIN32)
		set (${PLUGIN} SHARED)
	else (WIN32)
		set (${PLUGIN} PLUGIN)
	endif (WIN32)
	add_subdirectory (${PLUGIN})
endforeach (PLUGIN)
