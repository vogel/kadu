set (SYNTAX_FILES
	aqq2_by_maddox84.syntax
	Arvenil.syntax
	c34_white_gradient_compact.syntax
	cheester.syntax
	filozof.syntax
	fuku.syntax
	gg.syntax
	hapi.syntax
	irc.syntax
	kadu.syntax
	skp_by_dam.syntax
	skype_by_dorr.syntax
	ultr.syntax
)

set (ADIUM_STYLES
	Default
	Modern\ Bubbling\ \(Compact\)
	renkooNaked
	Satin
	SimpleStuff
	Stockholm
)

install (FILES ${SYNTAX_FILES}
	DESTINATION ${KADU_DATA_DIR}/syntax/chat
)

install (DIRECTORY ${ADIUM_STYLES} DESTINATION ${KADU_DATA_DIR}/syntax/chat)
