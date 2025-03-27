import QtQuick
import QtQuick.Controls
import SMSplat 1.0
import QtQuick.Effects

Rectangle {
    id: root
    //required property var legend
    //required property var inputMenu
    visible: false
    width: 148
    height: 144
    radius: 12
    color: "transparent"
    property string selectedEntry: ""

    function openPopup() {
        root.visible = true
        popup.open()
    }
    /*Rectangle {
        width: root.width
        height: root.height
        anchors.centerIn: parent
        color: "transparent"
        MultiEffect {
            id: shadow
            source: parent
            shadowEnabled: true
            shadowBlur: 2.0
            shadowVerticalOffset: 2
            shadowOpacity: 0.1
            shadowColor: "black"
            anchors.fill: parent
        }
    }*/
    Popup {
        id: popup
        anchors.centerIn: parent    
        width: root.width
        height: root.height
        dim: false
        padding: 0

        
        background: Rectangle {
            width: parent.width
            height: parent.height
            radius: 12
            color: "white"
            border.width: 1 // needs to be a shadow, this is a temporary thing
            border.color: SplatQmlGlobals.outlineBorder
        }

        onClosed: {
            root.visible = false
        }

        Column {
            anchors.centerIn: parent
            spacing: 0
            
            ActionsMenuEntry {
                w: popup.width
                iconPath: "qrc:/img/duplicate.svg"
                entryText: "Duplicate"
                textColor: SplatQmlGlobals.black
                onEntryClicked: {
                    console.log("Duplicate ", selectedEntry)
                    if (selectedEntry !== "") {
                        if(SMSplat.splatModel.contains(selectedEntry) && SMSplat.inputModel.contains(selectedEntry)) {
                            SMSplat.duplicateSplatGenInfo(selectedEntry)
                            SMSplat.duplicateSplatInputInfo(selectedEntry)
                        }
                        else {
                            console.log("Cannot duplicate ", selectedEntry)
                        }
                    }
                }
            }
            
            ActionsMenuEntry {
                w: popup.width
                iconPath: "qrc:/img/eye.svg"
                entryText: "Hide legend"
                textColor: SplatQmlGlobals.black
                /*onEntryClicked: {
                    legend.hide()
                }*/
            }

            ActionsMenuEntry {
                w: popup.width
                iconPath: "qrc:/img/trashbin.svg"
                entryText: "Delete"
                textColor: SplatQmlGlobals.darkRed
                onEntryClicked: {
                    if (selectedEntry !== "") {
                        if(SMSplat.splatModel.contains(selectedEntry) && SMSplat.inputModel.contains(selectedEntry)) {
                            SMSplat.inputModel.removeByName(selectedEntry)
                            SMSplat.splatModel.removeByName(selectedEntry)
                        }
                        else {
                            console.log("Cannot delete saved splat")
                            //TODO: show a popup to the user that the splat is saved in the database and cannot be deleted
                        }
                    }
                }
            }
        }    
    }

}