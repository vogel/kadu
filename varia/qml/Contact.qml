import QtQuick 1.1

Rectangle
{
	property string displayName : ""
	property string avatarPath : ""
	property string statusIconPath : ""

	id: container
	clip: true
	width: 74
	height: 108

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
				border.color: avatarImage.status == Image.Ready ? "transparent" : "#dedede"
				height: width
				width: parent.width

				Image
				{
					id: avatarImage
					anchors
					{
						fill: parent
						margins: 1
					}
					fillMode: Image.PreserveAspectFit
					source: avatarPath
				}

				Image
				{
					id: statusImage
					anchors
					{
						right: parent.right
						bottom: parent.bottom
						margins: 5
					}
					width: 16
					height: 16
					fillMode: Image.PreserveAspectFit
					source: statusIconPath
				}
			}
		}

		Text
		{
			anchors.horizontalCenter: parent.horizontalCenter
			clip: true
			horizontalAlignment: Text.AlignHCenter
			text: displayName
			width: parent.width
			height: parent.height - top - 5
			wrapMode: Text.WordWrap
		}
	}

	MouseArea
	{
		id: mouseArea
		anchors.fill: parent
		hoverEnabled: true
	}

	SystemPalette
	{
		id: pallete
	}

	states:
	[
		State
		{
			name: "normal"
			when: !mouseArea.containsMouse
			PropertyChanges { target: container; color: "transparent" }
		},

		State
		{
			name: "hover"
			when: mouseArea.containsMouse
			PropertyChanges { target: container; color: Qt.lighter(pallete.highlight) }
		}
	]

	transitions:
	[
		Transition
		{
			from: "normal"; to: "hover"; reversible: true
			ColorAnimation
			{
				duration: 300
			}
		}
	]

}
