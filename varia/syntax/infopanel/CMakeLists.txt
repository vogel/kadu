set (SYNTAX_FILES
	Arvenil.syntax
	"Old Default.syntax"
	ultr.syntax
)

install (FILES ${SYNTAX_FILES}
	DESTINATION "${KADU_DATADIR}/syntax/infopanel"
)
