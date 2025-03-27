import QtQuick
import QtQuick.Controls
import SMSplat 1.0

Rectangle {
    id: root
    required property int w
    required property string iconPath
    required property string entryText
    required property color textColor

    signal entryClicked()

    width: w
    height: 40
    color: "transparent"

    Image {
        id: icon
        source: iconPath
        anchors.verticalCenter: parent.verticalCenter
        anchors.left: parent.left
        anchors.leftMargin: 16
    }

    Text {
        text: entryText
        color: textColor
        anchors.verticalCenter: parent.verticalCenter
        anchors.left: icon.right
        anchors.leftMargin: 12
    }

    MouseArea {
        id: area
        anchors.fill: parent
        hoverEnabled: true
        onClicked: {
            entryClicked()
        }
    }

    states: [
        State {
            name: "hovered"
            when: area.containsMouse && !area.containsPress
            PropertyChanges {
                target: root
                color: SplatQmlGlobals.hover
            }
        },
        State {
            name: "pressed"
            when: area.containsPress
            PropertyChanges {
                target: root
                color: SplatQmlGlobals.pressed
            }
        }
    ]

}