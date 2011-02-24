set (SOURCES
	xmms.cpp
)

set (MOC_SOURCES
	xmms.h
)

pkg_search_module (GLIB REQUIRED glib)

set (XMMS_LIBRARIES
)

set (XMMS_INCLUDE_DIRS
)

kadu_module (xmms_mediaplayer
	MODULE_SOURCES ${SOURCES}
	MODULE_MOC_SOURCES ${MOC_SOURCES}
)

set_target_properties (xmms_mediaplayer PROPERTIES LINK_FLAGS "-lxmms")

target_link_libraries (xmms_mediaplayer ${XMMS_LIBRARIES} ${GLIB_LIBRARIES})
include_directories (${XMMS_INCLUDE_DIRS} ${GLIB_INCLUDE_DIRS})
