import QtQuick 1.1

Item
{
	property string displayName : ""
	clip: true

	Text
	{
		anchors.bottom: parent.bottom
		anchors.left: parent.left
		anchors.right: parent.right
		elide: Text.ElideRight
		horizontalAlignment: Text.AlignHLeft
		text: displayName
		width: parent.width
		wrapMode: Text.WordWrap
	}
}
