import QtQuick 1.1

Item
{
	id: openChatWith

	signal itemActivated(int index)

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
				id: found
				text: "Found: " + contacts.count + " contacts"
				height: 20
			}

			Text
			{
				id: currentlySelectedText
				anchors.top: found.bottom
				text: "Selected: " + (null != contacts.currentItem ? contacts.currentItem.contact.displayName : "none")
				height: 20
			}

			ContactsGridView
			{
				id: contacts
				anchors.left: parent.left
				anchors.right: parent.right
				anchors.top: currentlySelectedText.bottom
				anchors.bottom: parent.bottom
				clip: true

				onItemActivated: {
					openChatWith.itemActivated(index)
				}
			}
		}
	}
}
