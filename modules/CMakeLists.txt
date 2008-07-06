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

set (STATIC_MODULES ${STATIC_MODULES} CACHE INTERNAL "")

foreach (MODULE ${STATIC_MODULES})
	message (STATUS "Static module: " ${MODULE})
	set (${MODULE} STATIC)
	link_directories (${MODULE})
	add_subdirectory (${MODULE})
endforeach (MODULE)

foreach (MODULE ${DYNAMIC_MODULES})
	message (STATUS "Dynamic module: " ${MODULE})
	set (${MODULE} MODULE)
# 	link_directories (${MODULE})
	add_subdirectory (${MODULE})
endforeach (MODULE)
