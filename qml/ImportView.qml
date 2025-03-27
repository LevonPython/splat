import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import SMSplat 1.0

    Rectangle {
        id: importView
        width: parent.width
        height: parent.height
        color: "white"
        property var dbList: []
        property var selectedEntries_: []

        Component.onCompleted: {
            dbList = SMSplat.getAvailableSplatsFromDb()
        }

        Image {
            id: uploadIcon
            width: 20
            height: width
            source: "qrc:/img/upload_gray.svg"
            anchors.top: parent.top
            anchors.topMargin: 16
            anchors.left: parent.left
            anchors.leftMargin: 16
        }

        Text {
            text: qsTr("Import coverages")
            font.pixelSize: 14
            color: SplatQmlGlobals.textSecondary
            anchors.verticalCenter: uploadIcon.verticalCenter
            anchors.left: uploadIcon.right
            anchors.leftMargin: 8
        }

        Image {
            id: closeIcon
            width: 20
            height: width
            source: "qrc:/img/close.svg"
            anchors.verticalCenter: uploadIcon.verticalCenter
            anchors.right: parent.right
            anchors.rightMargin: 16

            MouseArea {
                anchors.fill: parent
                onClicked: {
                    closeImportView()
                } 
            }
        }

        Rectangle {
            id: div_top
            width: parent.width
            height: 1
            color: SplatQmlGlobals.divider
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.top: closeIcon.bottom
            anchors.topMargin: 16
        }

        ImportListView {
            id: importListView
            selectedEntries: selectedEntries_
            anchors.top: div_top.bottom
            anchors.horizontalCenter: parent.horizontalCenter
            visible: dbList.length > 0
        }

        Text {
            id: emptyDB
            text: qsTr("No coverages found in database")
            color: "black"
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.verticalCenter: parent.verticalCenter
            visible: dbList.length === 0
        }

        Rectangle {
            id: div_bottom
            width: parent.width
            height: 1
            color: SplatQmlGlobals.divider
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.bottom: parent.bottom
            anchors.bottomMargin: importButton.height + 32
        }

        GenerateButton {
            id: importButton
            w: 130
            h: 36
            r: h/2
            anchors.bottom: parent.bottom
            anchors.bottomMargin: 16
            anchors.horizontalCenter: parent.horizontalCenter
            backgroundColor: SplatQmlGlobals.primaryEnabled
            buttonName: qsTr("IMPORT")
            textColor: "white"
            textFont: SplatQmlGlobals.mainFont
            fontSize: 14
            onGenerateClicked: {
                SMSplat.importFromDb(selectedEntries_)
                closeImportView()
            }
        }
    }