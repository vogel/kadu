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

message (STATUS "Found modules: " ${MODULES})
message (STATUS "Static modules: " ${STATIC_MODULES})
message (STATUS "Dynamic modules: " ${DYNAMIC_MODULES})
message (STATUS "Disabled modules: " ${DISABLED_MODULES})