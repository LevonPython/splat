import QtQuick
import QtQuick.Controls
import SMSplat 1.0

Rectangle {
    id: root

    required property real w
    required property real h
    required property real r
    required property color backgroundColor
    required property string buttonName
    required property color textColor
    required property string textFont
    required property int fontSize    
    property string iconSourcePath: ""

    width: w
    height: h
    radius: r
    color: backgroundColor

    signal generateClicked()


    Row {
        anchors.centerIn: parent
        spacing: 8
        Image {
            id: icon
            visible: false
            source: iconSourcePath 
        }

        Text {
            id: buttonLabel
            text: buttonName
            color: textColor
            font.family: textFont
            font.pixelSize: fontSize
        }
    }

    MouseArea {
        id: mouseArea
        anchors.fill: parent
        hoverEnabled: true
        onClicked: {
            generateClicked()
        }
    }
  
    states: [
        State {
            name: "enabled";
            PropertyChanges { target: root; color: SplatQmlGlobals.primaryEnabled }
            PropertyChanges { target: buttonLabel; color: "white" }
        },
        State {
            name: "hovered"; when: mouseArea.containsMouse && !mouseArea.pressed
            PropertyChanges { target: root; color: SplatQmlGlobals.primaryHover }
            PropertyChanges { target: buttonLabel; color: "white" }
        },
        State {
            name: "pressed"; when: mouseArea.pressed
            PropertyChanges { target: root; color: SplatQmlGlobals.primaryPressed }
            PropertyChanges { target: buttonLabel; color: "white" }
        },
        State {
            name: "loading"
            PropertyChanges { target: icon; visible: true }
            PropertyChanges { target: root; color: SplatQmlGlobals.disableBackgrnd }
            PropertyChanges { target: buttonLabel; color: SplatQmlGlobals.textDisable }
        },
        State {
            name: "disabled"
            PropertyChanges { target: icon; visible: false }
            PropertyChanges { target: root; color: SplatQmlGlobals.disableBackgrnd }
            PropertyChanges { target: buttonLabel; color: SplatQmlGlobals.textDisable }
        }
    ]
}