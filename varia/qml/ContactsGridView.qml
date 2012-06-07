import QtQuick 1.1

Item
{
	id: component

	signal itemActivated(int index)

	property int count: contactsGrid.count
	property Item currentItem: contactsGrid.currentItem
	property int currentIndex: contactsGrid.currentIndex

	SystemPalette
	{
		id: pallete
	}

	Component
	{
		id: contactsDelegate
		Item
		{
			width: contactsGrid.cellWidth
			height: contactsGrid.cellHeight

			property Contact contact: contact

			Contact
			{
				id: contact
				anchors.fill: parent
				displayName: display
				avatarPath: avatar
				statusIconPath: statusIcon
				textColor: index == contactsGrid.currentIndex ? pallete.highlightedText : pallete.text
			}

			MouseArea
			{
				anchors.fill: parent
				onClicked: {
					contactsGrid.currentIndex = index;
				}
				onDoubleClicked: {
					component.itemActivated(index)
				}
			}
		}
	}

	GridView
	{
		id: contactsGrid
		anchors.fill: parent
		cellWidth: 222
		cellHeight: 74
		focus: true
		highlight: Rectangle
		{
			color: pallete.highlight
		}
		highlightFollowsCurrentItem: true
		model: buddies
		delegate: contactsDelegate
	}
}
