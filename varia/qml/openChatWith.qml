import QtQuick 1.1

Item
{
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

			Text
			{
				id: text
				text: "Found: " + contacts.count + " contacts"
				height: 20
			}

			ContactsGridView
			{
				id: contacts
				anchors.left: parent.left
				anchors.right: parent.right
				anchors.top: text.bottom
				anchors.bottom: parent.bottom
				clip: true
			}
		}
	}
}
