set (SYNTAX_FILES
	abaddon.syntax
	Arvenil.syntax
	ciuciu.syntax
	default.syntax
	dorr.syntax
	maz.syntax
	ronk.syntax
	ultr.syntax
)

install (FILES ${SYNTAX_FILES}
	DESTINATION ${KADU_DATA_DIR}/syntax/infopanel
)
