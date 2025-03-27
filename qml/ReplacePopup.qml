import QtQuick
import QtQuick.Controls
import SMSplat 1.0

CustomPopup {
    id: root
     
    popupType: "replace"
    entryToBeReplaced: "Goris - Ground station"

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
            w: 91
            h: 36
            buttonName: "RENAME"
            colorScheme: "whiteNblue"
            anchors.verticalCenter: parent.verticalCenter
            anchors.left: parent.left
            anchors.leftMargin: 347
        }

        DecisionButton {
            w: 95
            h: 36
            buttonName: "REPLACE"
            colorScheme: "blue"
            anchors.verticalCenter: parent.verticalCenter
            anchors.right: parent.right
            anchors.rightMargin: 24
        }
    }
}