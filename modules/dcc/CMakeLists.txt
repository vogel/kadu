set (SOURCES
	dcc.cpp
	dcc_handler.cpp
	dcc_socket.cpp
	file_transfer.cpp
	file_transfer_manager.cpp
	file_transfer_notifications.cpp
	file_transfer_window.cpp
)

set (MOC_SOURCES
	dcc.h
	dcc_handler.h
	dcc_socket.h
	file_transfer.h
	file_transfer_manager.h
	file_transfer_notifications.h
	file_transfer_window.h
)

set (TRANSLATION_SOURCES
	translations/dcc_de.ts
	translations/dcc_fr.ts
	translations/dcc_it.ts
	translations/dcc_pl.ts
)

set (CONFIGURATION_FILES
	configuration/dcc.ui)

kadu_module_desc (dcc.desc)
kadu_module_configuration(${CONFIGURATION_FILES})
qt_wrap_cpp (dcc MOC_FILES ${MOC_SOURCES})

qt4_add_translation (TRANSLATION_FILES ${TRANSLATION_SOURCES})

install (FILES ${TRANSLATION_FILES}
	DESTINATION "share/kadu/modules/translations")

if (COMMAND cmake_policy)
	cmake_policy(SET CMP0003 NEW)
endif (COMMAND cmake_policy)

add_library (dcc ${dcc} ${SOURCES} ${MOC_FILES})
add_custom_target (dcc-translations DEPENDS ${TRANSLATION_FILES})
add_dependencies (dcc dcc-translations)

install (TARGETS dcc DESTINATION lib/kadu/modules)
