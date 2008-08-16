If (WIN32)
	set (STATIC_MODULES "docking;qt4_docking")
	set (DYNAMIC_MODULES "account_management;autoaway;dcc;default_sms;hints;history;notify;sms;window_notify;advanced_userlist;autoresponder;sound;config_wizard;ext_sound;echo;exec_notify;desktop_docking;dshow_sound;voice")
	set (DISABLED_MODULES "encryption")
else (WIN32)

	exec_program (${CMAKE_CURRENT_SOURCE_DIR}/get-all.sh ${CMAKE_CURRENT_SOURCE_DIR}
		ARGS ${ENABLE_AUTDOWNLOAD}
		OUTPUT_VARIABLE MODULES)

	exec_program (${CMAKE_CURRENT_SOURCE_DIR}/get-list.sh ${CMAKE_CURRENT_SOURCE_DIR}
		ARGS "y" ${MODULES}
		OUTPUT_VARIABLE STATIC_MODULES)

	exec_program (${CMAKE_CURRENT_SOURCE_DIR}/get-list.sh ${CMAKE_CURRENT_SOURCE_DIR}
		ARGS "m" ${MODULES}
		OUTPUT_VARIABLE DYNAMIC_MODULES)

	exec_program (${CMAKE_CURRENT_SOURCE_DIR}/get-list.sh ${CMAKE_CURRENT_SOURCE_DIR}
		ARGS "n" ${MODULES}
		OUTPUT_VARIABLE DISABLED_MODULES)
#autodownload:
	if (ENABLE_AUTDOWNLOAD)
		foreach (MODULE ${STATIC_MODULES} ${DYNAMIC_MODULES})
			if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/${MODULE}.web)
				if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/${MODULE}/.autodownloaded OR NOT EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/${MODULE}/spec)
					exec_program (${CMAKE_SOURCE_DIR}/varia/scripts/autodownload ${CMAKE_CURRENT_SOURCE_DIR}
						ARGS module ${MODULE}
						RETURN_VALUE DOWNLOAD_STATUS
					)
					if (DOWNLOAD_STATUS EQUAL 1)
						message (FATAL_ERROR ${MODULE} " download failed!")
					endif (DOWNLOAD_STATUS EQUAL 1)
				endif(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/${MODULE}/.autodownloaded OR NOT EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/${MODULE}/spec)
			endif(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/${MODULE}.web)
		endforeach (MODULE)
	endif (ENABLE_AUTDOWNLOAD)

	exec_program (${CMAKE_CURRENT_SOURCE_DIR}/check-deps.sh ${CMAKE_CURRENT_SOURCE_DIR}
		ARGS ${AWK} ${STATIC_MODULES} - ${DYNAMIC_MODULES}
		OUTPUT_VARIABLE DEPS_MESSAGE
		RETURN_VALUE DEPS_STATUS
	)	

	if (DEPS_STATUS EQUAL 1)
		message (FATAL_ERROR ${DEPS_MESSAGE})
	endif (DEPS_STATUS EQUAL 1)

	exec_program (${CMAKE_CURRENT_SOURCE_DIR}/create-static-modules.sh ${CMAKE_CURRENT_BINARY_DIR}
		ARGS ${STATIC_MODULES}
		OUTPUT_VARIABLE NULL_VAR
	)

	find_program (AWK NAMES awk gawk)
endif (WIN32)

if (NOT STATIC_MODULES STREQUAL "\n")
	set (STATIC_MODULES ${STATIC_MODULES} CACHE INTERNAL "")

	foreach (MODULE ${STATIC_MODULES})
		message (STATUS "Static module: " ${MODULE})
		set (${MODULE} STATIC)
		link_directories (${MODULE})
		add_subdirectory (${MODULE})
	endforeach (MODULE)
endif (NOT STATIC_MODULES STREQUAL "\n")

if (NOT DYNAMIC_MODULES STREQUAL "\n")
	foreach (MODULE ${DYNAMIC_MODULES})
		message (STATUS "Dynamic module: " ${MODULE})
		if (WIN32)
			set (${MODULE} SHARED)
		else (WIN32)
			set (${MODULE} MODULE)
		endif (WIN32)
	# 	link_directories (${MODULE})
		add_subdirectory (${MODULE})
	endforeach (MODULE)
endif (NOT DYNAMIC_MODULES STREQUAL "\n")
