import QtQuick 2.3

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
					contactsGrid.focus = true;
				}
				onDoubleClicked: {
					component.itemActivated(index)
					contactsGrid.focus = true;
				}
			}
		}
	}

	GridView
	{
		objectName: "mainWidget"
		id: contactsGrid
		anchors.fill: parent
		cellWidth: 222
		cellHeight: 74
		currentIndex: 0
		focus: true
		keyNavigationWraps: true
		highlight: Rectangle
		{
			color: pallete.highlight
		}
		highlightFollowsCurrentItem: true
		model: buddies
		delegate: contactsDelegate
	}
}
