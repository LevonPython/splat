import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import SMSplat 1.0

Rectangle {
    id: mainMenu
    width: 250
    height: 1016
    color: "white"
    anchors.verticalCenter: parent.verticalCenter
    property string selectedEntry_: covList.selectedEntry ? covList.selectedEntry : ""

    x: -width
    onVisibleChanged: {
        if(visible) {
            anim.restart()
        }
    }
    
    NumberAnimation on x {
        id: anim
        from: -width // Start off-screen to the left
        to: 0 // Position in the center horizontally
        duration: 800 // Increased duration for slower animation (3 seconds)
        easing.type: Easing.InOutQuad // S
    }

    Rectangle {
        id: header
        width: parent.width
        height: 41
        color: "transparent"

        Image {
            width: 20
            height: width
            source: "qrc:/img/signal.svg"
            anchors.verticalCenter: parent.verticalCenter
            anchors.left: parent.left
            anchors.leftMargin: 16
        }

        Text {
            text: qsTr("Coverage calculation")
            font.pixelSize: 14
            color: SplatQmlGlobals.textSecondary
            anchors.verticalCenter: parent.verticalCenter
            anchors.left: parent.left
            anchors.leftMargin: 48
        }

        Image {
            width: 20
            height: width
            source: "qrc:/img/close.svg"
            anchors.verticalCenter: parent.verticalCenter
            anchors.right: parent.right
            anchors.rightMargin: 16

            MouseArea {
                anchors.fill: parent
                onClicked: {
                    mainMenu.visible = false
                }
            } 
        }

        Rectangle {
            width: parent.width
            height: 1
            color: SplatQmlGlobals.divider
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.bottom: parent.bottom
        }
    }

    Rectangle {
        id: newCoverage
        width: parent.width
        height: 36
        color: mArea1.containsPress ? SplatQmlGlobals.listSelectedHover : mArea1.containsMouse ? SplatQmlGlobals.lightHover : "transparent"
        anchors.top: header.bottom
        anchors.topMargin: 16
        anchors.horizontalCenter: parent.horizontalCenter

        Image {
            source: "qrc:/img/plus.svg"
            anchors.verticalCenter: parent.verticalCenter
            anchors.left: parent.left
            anchors.leftMargin: 16
        }

        Text {
            text: qsTr("New coverage")
            anchors.verticalCenter: parent.verticalCenter
            anchors.left: parent.left
            anchors.leftMargin: 44
        }

        MouseArea {
            id: mArea1
            hoverEnabled: true
            anchors.fill: parent
            onClicked: {
                covList.selectedEntry = ""
                inputMenu.reset()
            }
        }

    }
    
    Rectangle {
        id: importFromDB
        width: parent.width
        height: 36
        color: mArea2.containsPress ? SplatQmlGlobals.listSelectedHover : mArea2.containsMouse ? SplatQmlGlobals.lightHover : "transparent"
        anchors.top: newCoverage.bottom
        anchors.horizontalCenter: parent.horizontalCenter
        
        Image {
            source: "qrc:/img/upload.svg"
            anchors.verticalCenter: parent.verticalCenter
            anchors.left: parent.left
            anchors.leftMargin: 16
        }

        Text {
            text: qsTr("Import from database")
            anchors.verticalCenter: parent.verticalCenter
            anchors.left: parent.left
            anchors.leftMargin: 44
        }

        MouseArea {
            id: mArea2
            anchors.fill: parent
            hoverEnabled: true
            onClicked: {
                importViewLoader.active = true
                importViewLoader.item.visible = true
            }
        }
    }
    
    CoverageList {
        id: covList
        inputmenu: inputMenu
        anchors.top: importFromDB.bottom
        anchors.topMargin: 16
        anchors.horizontalCenter: parent.horizontalCenter
    }
    
    Rectangle {
        id: div1
        width: parent.width
        height: 1
        color: SplatQmlGlobals.divider
        anchors.top: covList.bottom
        anchors.topMargin: 16
        anchors.horizontalCenter: parent.horizontalCenter
    }

    InputMenu {
        id: inputMenu
        anchors.top: div1.bottom
        anchors.horizontalCenter: parent.horizontalCenter
    }


    Rectangle {
        id: div
        width: parent.width
        height: 1
        color: SplatQmlGlobals.divider
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.bottom: parent.bottom
        anchors.bottomMargin: footer.height
    }

    Rectangle {
        id: footer
        width: parent.width
        height: 69
        color: "transparent"
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.bottom: parent.bottom
        Row {
            spacing: 8
            anchors.centerIn: parent
            GenerateButton {
                w: 130
                h: 36
                r: h/2
                backgroundColor: SplatQmlGlobals.primaryEnabled
                buttonName: qsTr("GENERATE")
                textColor: "white"
                textFont: SplatQmlGlobals.mainFont
                fontSize: 14
                onGenerateClicked: {
                    inputMenu.inputStruct = inputMenu.getFieldValues(SMSplat.createInputInfo())
                    SMSplat.generate(inputMenu.inputStruct)
                }
            }

            CustomRoundButton {
                id: saveButton
                w: 36
                iconPath: "qrc:/img/save.svg" 
                onRoundButtonClicked: {
                    SMSplat.save(covList.selectedEntry)
                }
            }
            
            CustomRoundButton {
                id: threeDots
                w: 36
                iconPath: "qrc:/img/threedots.svg"

                onRoundButtonClicked: {
                    actionsMenu.openPopup()
                }
            }
        }
    }
    
    ActionsMenu {
        id: actionsMenu
        anchors.right: parent.right
        anchors.rightMargin: 12
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 16
        selectedEntry: selectedEntry_
    } 

    Loader {
        id: importViewLoader
        active: false
        anchors.fill: parent
        sourceComponent: Component {
            ImportView {
                anchors.top: parent.top
                visible: false
                
                function closeImportView() {
                    visible = false
                    importViewLoader.active = false
                }
                
                Component.onCompleted: {
                    if (typeof closeIcon !== "undefined") {
                        closeIcon.clicked.connect(closeImportView)
                    }
                }
            }
        }
    }
}