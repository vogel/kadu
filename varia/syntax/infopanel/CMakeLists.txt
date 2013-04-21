set (SYNTAX_FILES
	Arvenil.syntax
	"Old Default.syntax"
	ultr.syntax
)

install (FILES ${SYNTAX_FILES}
	DESTINATION "${KADU_INSTALL_DATA_DIR}/syntax/infopanel"
)
