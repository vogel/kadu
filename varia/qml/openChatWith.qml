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

				Column
				{
					anchors.fill: parent
					Contact
					{
						anchors.fill: parent
						displayName: display
					}
				}
			}
		}

		GridView
		{
			id: contactsGrid
			anchors.fill: parent
			cellWidth: 74
			cellHeight: 106
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
