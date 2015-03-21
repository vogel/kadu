import QtQuick 2.3

Item
{
	id: openChatWith

	signal itemActivated(int index)

	property int currentIndex: contacts.currentIndex

	SystemPalette
	{
		id: pallete
	}

	Rectangle
	{
		id: background
		anchors.fill: parent
		clip: true

		Rectangle
		{
			id: padding
			anchors.horizontalCenter: parent.horizontalCenter
			anchors.verticalCenter: parent.verticalCenter
			width: parent.width - 10
			height: parent.height - 10

			ContactsGridView
			{
				id: contacts
				anchors.fill: parent
				clip: true

				onItemActivated: {
					openChatWith.itemActivated(index)
				}
			}
		}
	}
}
