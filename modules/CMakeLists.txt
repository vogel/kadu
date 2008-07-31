If (WIN32)
	set (STATIC_MODULES "docking;qt4_docking")
	set (DYNAMIC_MODULES "account_management;autoaway;dcc;default_sms;hints;history;notify;sms;window_notify;advanced_userlist;autoresponder;sound;config_wizard;ext_sound;echo;exec_notify;desktop_docking")
	set (DISABLED_MODULES "encryption;voice")
else (WIN32)

exec_program (./get-all.sh ${CMAKE_CURRENT_SOURCE_DIR}
	ARGS ${ENABLE_AUTDOWNLOAD}
	OUTPUT_VARIABLE MODULES)

exec_program (./get-list.sh ${CMAKE_CURRENT_SOURCE_DIR}
	ARGS "y" ${MODULES}
	OUTPUT_VARIABLE STATIC_MODULES)

exec_program (./get-list.sh ${CMAKE_CURRENT_SOURCE_DIR}
	ARGS "m" ${MODULES}
	OUTPUT_VARIABLE DYNAMIC_MODULES)

exec_program (./get-list.sh ${CMAKE_CURRENT_SOURCE_DIR}
	ARGS "n" ${MODULES}
	OUTPUT_VARIABLE DISABLED_MODULES)

exec_program (./create-static-modules.sh ${CMAKE_CURRENT_SOURCE_DIR}
	ARGS ${STATIC_MODULES}
	OUTPUT_VARIABLE NULL_VAR
)

find_program (AWK NAMES awk gawk)

exec_program (./check-deps.sh ${CMAKE_CURRENT_SOURCE_DIR}
	ARGS ${AWK} ${STATIC_MODULES} - ${DYNAMIC_MODULES}
	OUTPUT_VARIABLE DEPS_MESSAGE
	RETURN_VALUE DEPS_STATUS
)

if (DEPS_STATUS EQUAL 1)
	message (FATAL_ERROR ${DEPS_MESSAGE})
endif (DEPS_STATUS EQUAL 1)

endif (WIN32)

set (STATIC_MODULES ${STATIC_MODULES} CACHE INTERNAL "")

foreach (MODULE ${STATIC_MODULES})
	message (STATUS "Static module: " ${MODULE})
	set (${MODULE} STATIC)
	link_directories (${MODULE})
	add_subdirectory (${MODULE})
endforeach (MODULE)

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
