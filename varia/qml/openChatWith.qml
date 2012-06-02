import QtQuick 1.1

Item
{

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
				color: "lightsteelblue"
				radius: 5
			}
			model: buddies
			delegate: contactsDelegate
		}
	}
}
