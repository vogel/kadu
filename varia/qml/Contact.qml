import QtQuick 1.1

Item
{
	property string displayName : ""
	property string avatarPath : ""
	clip: true
	width: 74
	height: 148

	Column
	{
		id: content
		anchors.horizontalCenter: parent.horizontalCenter
		anchors.verticalCenter: parent.verticalCenter
		spacing: 2
		width: parent.width - 10
		height: parent.height - 10

		Item
		{
			id: avatarContainer
			width: parent.width
			height: parent.width

			Rectangle
			{
				anchors.horizontalCenter: parent.horizontalCenter
				anchors.verticalCenter: parent.verticalCenter
				border.color: "#dedede"
				height: width
				width: parent.width

				Image
				{
					id: avatarImage
					anchors.horizontalCenter: parent.horizontalCenter
					anchors.verticalCenter: parent.verticalCenter
					clip: true
					fillMode: Image.PreserveAspectFit
					source: avatarPath
					height: parent.height - 2
					width: parent.width - 2
				}
			}
		}

		Text
		{
			anchors.horizontalCenter: parent.horizontalCenter
			clip: true
			horizontalAlignment: Text.AlignHCenter
			text: displayName
			width: parent.width - 10
			height: parent.height - top - 5
			wrapMode: Text.WordWrap
		}
	}
}
