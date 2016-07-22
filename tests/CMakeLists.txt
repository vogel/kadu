option (ENABLE_TESTS "Enable tests" ON)

if (ENABLE_TESTS)
	add_custom_target (all_tests ALL DEPENDS kadu ${COMPILE_PLUGINS} ${UNIT_TEST_TARGETS})
	add_custom_command (
		TARGET all_tests
		COMMENT "Run tests"
		POST_BUILD COMMAND ctest ARGS --output-on-failure
		WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
	)
endif (ENABLE_TESTS)
