import QtQuick
import QtQuick.Controls
import SMSplat 1.0

CustomPopup {
    id: root 
    popupType: "unsaved"
    listOfUnsaved: "Goris - Ground station, Goris - Ground station, Goris - Ground station, Goris - Ground station"

    Rectangle {
        id: footer
        width: parent.width
        height: 64
        color: "transparent"
        anchors.bottom: parent.bottom
        anchors.horizontalCenter: parent.horizontalCenter

        DecisionButton {
            w: 88
            h: 36
            buttonName: "CANCEL" 
            colorScheme: "whiteNgray"
            anchors.verticalCenter: parent.verticalCenter
            anchors.left: parent.left
            anchors.leftMargin: 24
            onButtonClicked: {
                root.close()
            }
        }

        DecisionButton {
            w: 149
            h: 36
            buttonName: "SAVE AND CLOSE"
            colorScheme: "whiteNblue"
            anchors.verticalCenter: parent.verticalCenter
            anchors.left: parent.left
            anchors.leftMargin: 335
        }

        DecisionButton {
            w: 76
            h: 36
            buttonName: "CLOSE"
            colorScheme: "blue"
            anchors.verticalCenter: parent.verticalCenter
            anchors.right: parent.right
            anchors.rightMargin: 24
        }
    }
}