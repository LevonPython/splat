import QtQuick
import QtQuick.Controls
import SMSplat 1.0

Rectangle {
    id: root
    required property string buttonName
    signal btnClicked()
    color: SplatQmlGlobals.lightHover

    Text {
        text: buttonName
        font.pixelSize: 13
        color: SplatQmlGlobals.primaryEnabled
        anchors.centerIn: parent
    }

    MouseArea {
        id: mouseArea
        anchors.fill: parent
        hoverEnabled: true
        onClicked: {
            btnClicked()
        }
    }

    states: [
        State {
            name: "enabled"
            PropertyChanges {target: root; color: SplatQmlGlobals.lightHover}
        },
        State {
            name: "hovered"; when: mouseArea.containsMouse && !mouseArea.pressed
            PropertyChanges {target: root; color: SplatQmlGlobals.listSelectedHover}
        },
        State {
            name: "pressed"; when: mouseArea.pressed
            PropertyChanges {target: root; color: SplatQmlGlobals.lightHover}
        }
    ]

}