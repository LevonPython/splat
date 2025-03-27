import QtQuick
import QtQuick.Controls
import SMSplat 1.0

Popup {
    id: root
    required property string popupType 
    // this can be delete, unsaved, replace
    property string title: if(popupType === "delete") {
        return qsTr("Delete coverage area?")
    } else if(popupType === "unsaved") {
        return qsTr("Unsaved coverage")
    } else if(popupType === "replace") {
        return qsTr(entryToBeReplaced)
    }
    property string entryToBeDeleted
    property string entryToBeReplaced
    property string listOfUnsaved

    function validateProperties() {
        if (popupType === "delete") {
            if (!entryToBeDeleted) {
                console.error("entryToBeDeleted property is required when type is 'delete'")
            }
        } else if (popupType === "replace") {
            if (!entryToBeReplaced) {
                console.error("entryToBeReplaced property is required when type is 'replace'")
            }
        } else if (popupType === "unsaved") {
            if (!listOfUnsaved) {
                console.error("listOfUnsaved property is required when type is 'unsaved'")
            }
        } else {
            console.error("Invalid 'popupType' provided:", popupType)
        }
    }    

    width: 600
    height: 2*titleRec.height + 2 + messageRec.height
    anchors.centerIn: parent
    dim: true
    padding: 0

    background: Rectangle {
        width: root.width
        height: root.height
        radius: 12
        color: "white"
    }

    Rectangle {
        id: titleRec
        width: root.width
        height: 64
        color: "transparent"

        Image {
            id: icon
            source: "qrc:/img/warning.svg"
            visible: popupType != "delete"
            anchors.verticalCenter: parent.verticalCenter
            anchors.left: parent.left
            anchors.leftMargin: 24
        }

        Text {
            id: popupTitle
            text: title
            font.pixelSize: 20
            font.weight: Font.Bold 
            color: SplatQmlGlobals.black
            anchors.verticalCenter: parent.verticalCenter
            anchors.left: icon.visible ? icon.right : parent.left
            anchors.leftMargin: icon.visible ? 12 : 24
        }

        Image {
            width: 20
            height: width
            source: "qrc:/img/close.svg"
            anchors.verticalCenter: parent.verticalCenter
            anchors.right: parent.right
            anchors.rightMargin: 20

            MouseArea {
                anchors.fill: parent
                onClicked: {
                    root.close()
                }
            } 
        }
    }

    HorizontalDivider {
        anchors.horizontalCenter: root.horizontalCenter
        anchors.top: titleRec.bottom
    }


    Rectangle {
        id: messageRec
        width: parent.width
        height: 2*14 + message.height
        color: "transparent"
        anchors.centerIn: parent

        Text {
            id: message
            font.pixelSize: 14
            font.family: SplatQmlGlobals.mainFont
            width: parent.width - 48
            anchors.centerIn: parent
            textFormat: Text.RichText
            wrapMode: Text.Wrap
            elide: Text.ElideRight
            text: getPopupText()

            function getPopupText() {
                if (root.popupType === "delete") {
                    return qsTr("Are you sure you want to delete <b>%1</b> coverage area?").arg(entryToBeDeleted);
                } else if (root.popupType === "replace") {
                    return qsTr("Coverage with <b>%1</b> name already exists. Do you want to replace it?").arg(entryToBeReplaced);
                } else if (root.popupType === "unsaved") {
                    return qsTr("<b>%1</b> not saved").arg(listOfUnsaved);
                } else {
                    return "tralala";
                }
            }
        }
    }

    HorizontalDivider {
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 64
    }
}