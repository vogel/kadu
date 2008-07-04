set (SYNTAX_FILES
	cheester.syntax
	filozof.syntax
	fuku.syntax
	hapi.syntax
	irc.syntax
	kadu.syntax
)

install (FILES ${SYNTAX_FILES}
	DESTINATION "share/kadu/syntax/chat")
