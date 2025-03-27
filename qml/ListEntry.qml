import QtQuick
import QtQuick.Controls

Rectangle {
    required property string entryName
    required property bool isInDB
    property bool isSelected: false
    signal entryClicked()
    width: parent.width
    height: 36
    color: isSelected ? SplatQmlGlobals.listSelectedHover : "white"
    
    Row {
        anchors.verticalCenter: parent.verticalCenter
        anchors.left: parent.left
        anchors.leftMargin: 16
        spacing: 12
        Image {
            source: isInDB ? "qrc:/img/save.svg" : "qrc:/img/pencil.svg"
        }

        Text {
            text: entryName
        }
    }

    MouseArea {
        id: mArea
        anchors.fill: parent
        hoverEnabled: true
        onClicked: {
            entryClicked()
        }
    }
}