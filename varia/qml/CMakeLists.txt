set (QML_FILES
	Contact.qml
	openChatWith.qml
)

install (FILES ${QML_FILES}
	DESTINATION "${KADU_DATADIR}/qml"
)
