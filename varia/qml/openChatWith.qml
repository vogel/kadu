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

		Component
		{
			id: contactsDelegate
			Item
			{
				width: contactsGrid.cellWidth
				height: contactsGrid.cellHeight

				Contact
				{
					anchors.fill: parent
					displayName: display
					avatarPath: avatar
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
}
