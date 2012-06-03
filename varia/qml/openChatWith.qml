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

		ContactsGridView
		{
			anchors.fill: parent
		}
	}
}
