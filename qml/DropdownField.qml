import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Effects
import SMSplat 1.0

Item {
    id: root
    required property string title
    property bool enabled: false
    property var fieldValue: ""
    width: 218
    height: 58
    
    function setFieldValue(option) {
        nodeType.text = option
        fieldValue = option
    }

    Text {
        text: qsTr(title)
        font.pixelSize: 12
        color: SplatQmlGlobals.textSecondary
        anchors.top: parent.top
        anchors.left: parent.left
    }

    Rectangle {
        id: background
        width: parent.width
        height: 40
        color: "white"
        border.width: root.enabled ? 2 : 1
        border.color: root.enabled ? SplatQmlGlobals.primaryEnabled : SplatQmlGlobals.outlineBorder
        radius: 12 
        anchors.bottom: parent.bottom
        anchors.horizontalCenter: parent.horizontalCenter
        MouseArea {
            anchors.fill: parent
            onClicked: {
                root.enabled = !root.enabled
            }
        }

        Image {
            id: triangle
            source: root.enabled ? "qrc:/img/arrow_up.svg" : "qrc:/img/arrow_down.svg"
            anchors.right: parent.right
            anchors.rightMargin: 12
            anchors.verticalCenter: parent.verticalCenter
        }

        Text {
            id: nodeType
            color: SplatQmlGlobals.black
            anchors.verticalCenter: parent.verticalCenter
            anchors.left: parent.left
            anchors.leftMargin: 16
            text: ""
        }
    }

    Rectangle {
        width: 218
        height: 144
        color: "white"
        radius: 12
        visible: root.enabled
        anchors.top: root.bottom
        anchors.horizontalCenter: root.horizontalCenter

        MultiEffect {
            id: shd
            source: parent
            shadowEnabled: true
            shadowBlur: 2.0
            shadowVerticalOffset: 2
            shadowOpacity: 0.1
            shadowColor: "black"
            anchors.fill: parent
        }
        
        ColumnLayout {
            anchors.centerIn: parent
            spacing: 0
            Rectangle {
                width: root.width
                height: 40
                color: area1.containsPress ? SplatQmlGlobals.listSelectedHover: area1.containsMouse ? SplatQmlGlobals.lightHover : "transparent"
                Text {
                    id: option1
                    text: qsTr("Ground station")
                    color: SplatQmlGlobals.black
                    anchors.verticalCenter: parent.verticalCenter
                    anchors.left: parent.left
                    anchors.leftMargin: 16
                }
                MouseArea {
                    id: area1
                    anchors.fill: parent
                    hoverEnabled: true
                    onClicked: {
                        setFieldValue(option1.text)
                        root.enabled = false
                    }
                }
            }
            Rectangle {
                width: root.width
                height: 40
                color: area2.containsPress ? SplatQmlGlobals.listSelectedHover : area2.containsMouse ? SplatQmlGlobals.lightHover : "transparent"
                Text {
                    id: option2
                    text: qsTr("Relay")
                    color: SplatQmlGlobals.black
                    anchors.verticalCenter: parent.verticalCenter
                    anchors.left: parent.left
                    anchors.leftMargin: 16
                }
                MouseArea {
                    id: area2
                    anchors.fill: parent
                    hoverEnabled: true
                    onClicked: {
                        setFieldValue(option2.text)
                        root.enabled = false
                    }
                }
            }
            Rectangle {
                width: root.width
                height: 40
                color: area3.containsPress ? SplatQmlGlobals.listSelectedHover : area3.containsMouse ? SplatQmlGlobals.lightHover : "transparent"
                Text {
                    id: option3
                    text: qsTr("UAV")
                    color: SplatQmlGlobals.black
                    anchors.verticalCenter: parent.verticalCenter
                    anchors.left: parent.left
                    anchors.leftMargin: 16
                }
                MouseArea {
                    id: area3
                    anchors.fill: parent
                    hoverEnabled: true
                    onClicked: {
                        setFieldValue(option3.text)
                        root.enabled = false
                    }
                }
            }
        }
    }
}