import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import SMSplat 1.0

Rectangle {
    
    id: root
    required property real w
    required property string iconPath
    signal roundButtonClicked()
    width: w
    height: width
    radius: width/2
    border.width: 1
    border.color: SplatQmlGlobals.primaryEnabled
    
    Image {
        id: icon
        source: iconPath
        anchors.centerIn: parent
    }

    MouseArea {
        id: mouseArea
        hoverEnabled: true
        anchors.fill: parent
        onClicked: {
            roundButtonClicked()
        }
    }

    states: [
        State {
            name: "enabled";
            PropertyChanges {target: root; color: "white"; border.color: SplatQmlGlobals.primaryEnabled}
            PropertyChanges {target: icon; color: SplatQmlGlobals.primaryEnabled}
        },
        State {
            name: "hovered"; when: mouseArea.containsMouse && !mouseArea.pressed
            PropertyChanges {target: root; color: SplatQmlGlobals.lightHover}
        },
        State {
            name: "pressed"; when: mouseArea.pressed
            PropertyChanges {target: root; color: SplatQmlGlobals.listSelectedHover}
        },
        State {
            name: "disabled";
            PropertyChanges {target: root; color: "white"; border.color: SplatQmlGlobals.disableBackgrnd}
            PropertyChanges {target: icon; color: SplatQmlGlobals.textDisable}
        }
    ]
}