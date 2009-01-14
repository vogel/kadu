if (UNIX AND NOT APPLE)
set (SOURCES
	idle.cpp
	idle_x11.cpp
)
endif (UNIX AND NOT APPLE)

if (APPLE)
set (SOURCES
	idle.cpp
	idle_mac.cpp
)
endif (APPLE)

if (WIN32)
set (SOURCES
	idle.cpp
	idle_win.cpp
)
endif (WIN32)

set (MOC_SOURCES
	idle.h
)

kadu_module (idle
	MODULE_SOURCES ${SOURCES}
	MODULE_MOC_SOURCES ${MOC_SOURCES}
)
set_target_properties(idle PROPERTIES LINK_FLAGS "-lXss")
