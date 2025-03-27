import QtQuick
import QtQuick.Controls
import SMSplat 1.0

CustomPopup {
    id: root
    
    popupType: "delete"
    entryToBeDeleted: "Goris - Ground station"

    Rectangle {
        id: footer
        width: parent.width
        height: 64
        color: "transparent"
        anchors.bottom: parent.bottom
        anchors.horizontalCenter: parent.horizontalCenter

        DecisionButton {
            id: cancel
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
            w: 203
            h: 36
            buttonName: "DELETE FROM DATABASE"
            colorScheme: "whiteNred"
            anchors.verticalCenter: parent.verticalCenter
            anchors.right: remove.left
            anchors.rightMargin: 16
            onButtonClicked: {
                if(SMSplat.splatModel.contains(entryToBeDeleted)) {
                    SMSplat.splatModel.removeByName(entryToBeDeleted)
                }
            }
        }

        DecisionButton {
            id: remove
            w: 197
            h: 36
            buttonName: "REMOVE FROM THE LIST"
            colorScheme: "red"
            anchors.verticalCenter: parent.verticalCenter
            anchors.right: parent.right
            anchors.rightMargin: 24
            onButtonClicked: {
                if(SMSplat.inputModel.contains(entryToBeDeleted)) {
                    SMSplat.inputModel.removeByName(entryToBeDeleted)
                }
            }
        }
    }
}