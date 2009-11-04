set (SYNTAX_FILES
	cheester.syntax
	filozof.syntax
	fuku.syntax
	hapi.syntax
	irc.syntax
	kadu.syntax
)

set (ADIUM_STYLES
	Default
)

install (FILES ${SYNTAX_FILES}
	DESTINATION ${KADU_DATA_DIR}/syntax/chat)

install (DIRECTORY ${ADIUM_STYLES} DESTINATION ${KADU_DATA_DIR}/syntax/chat)
