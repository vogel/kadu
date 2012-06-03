import QtQuick 1.1

Item
{
	property int count: contactsGrid.count
	property Item currentItem: contactsGrid.currentItem

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
			}

			MouseArea
			{
				anchors.fill: parent
				onClicked: {
					contactsGrid.currentIndex = index;
				}
			}
		}
	}

	GridView
	{
		id: contactsGrid
		anchors.fill: parent
		cellWidth: 74
		cellHeight: 108
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
