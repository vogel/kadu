set (SYNTAX_FILES
	Arvenil.syntax
	default.syntax
	ultr.syntax
)

install (FILES ${SYNTAX_FILES}
	DESTINATION ${KADU_DATA_DIR}/syntax/infopanel
)
