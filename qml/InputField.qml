import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import SMSplat 1.0

Item {
    id: root
    property var regexp
    property string fieldValue
    required property string title

    function setFieldValue(value) {
        txtfield.text = value;
    }

    width: 218
    height: errorMessage.visible ? 58 + errorMessage.height + 4 : 58
    
    Text {
        id: fieldTitle
        text: qsTr(title)
        font.pixelSize: 12
        color: SplatQmlGlobals.textSecondary
        anchors.top: parent.top
        anchors.left: parent.left
    }

    TextField {
        id: txtfield
        width: parent.width
        height: 40      
        validator: regexp ? regexp : null
        hoverEnabled: true
        font.pixelSize: 16
        background: Rectangle {
            id: background
            anchors.fill: parent
            color: "white"
            border.color: SplatQmlGlobals.outlineBorder
            radius: 12 
        }
        onTextChanged: {
            fieldValue = txtfield.text;
        }
        anchors.top: fieldTitle.bottom
        anchors.topMargin: 8
        anchors.horizontalCenter: parent.horizontalCenter
    }

    Text {
        id: errorMessage
        visible: false
        text: qsTr("This field is required")
        font.pixelSize: 12
        anchors.top: txtfield.bottom
        anchors.topMargin: 4
        anchors.left: txtfield.left
    }

    states: [
        State {
            name: "enabled"  
            PropertyChanges {target: background; border.color: SplatQmlGlobals.outlineBorder; border.width: 1} 
            PropertyChanges {target: errorMessage; visible: false} 
        },
        State {
            name: "hovered"; when: txtfield.hovered && !txtfield.focus
            PropertyChanges {target: background; border.color: SplatQmlGlobals.black; border.width:1} 
            PropertyChanges {target: errorMessage; visible: false}
        },
        State {
            name: "active"; when: txtfield.focus
            PropertyChanges {target: background; border.color: SplatQmlGlobals.primaryEnabled; border.width: 2} 
            PropertyChanges {target: errorMessage; visible: false}
        },
        State {
            name: "error"
            PropertyChanges {target: background; border.color: SplatQmlGlobals.primaryEnabled; border.width: 2} 
            PropertyChanges {target: fieldTitle; color: SplatQmlGlobals.red}
            PropertyChanges {target: errorMessage; visible: true; color: SplatQmlGlobals.red}
        }        
    ]
}