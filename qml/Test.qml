import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import SMSplat 1.0

ApplicationWindow {
    id: root
    title: "SPLAT Manager"
    visible: true
    width: 640
    height: 480 
    background: Rectangle {
        color: "gray"
    }
    
    ColumnLayout {
        anchors.fill: parent
        spacing: 10
        
        RowLayout {
            Button {
                id: createNewButton
                objectName: "createNewButton"
                text: "Create New"
                onClicked: createDialog.open()
            }
            Button {
                id: importButton
                objectName: "importButton"
                text: "Import"
                onClicked: importDialog.open()
            }
            Button {
                text: "Clear List"
                onClicked: SMSplat.clearList()
            }
        }
        
        ListView {
            id: splatList
            objectName: "splatList"
            Layout.fillWidth: true
            Layout.fillHeight: true
            model: SMSplat.splatModel
            
            delegate: ItemDelegate {
                width: ListView.view.width
                contentItem: ColumnLayout {
                    spacing: 5
                    
                    Label {
                        text: name
                        font.bold: true
                    }
                    
                    Image {
                        Layout.preferredWidth: 200
                        Layout.preferredHeight: 200
                        source: image ? "image://memory/" + name : ""
                        visible: image !== null
                    }
                    
                    RowLayout {
                        Button {
                            text: isSavedInDb ? "Update" : "Save"
                            onClicked: SMSplat.save(name)
                        }
                        Button {
                            text: "Delete"
                            onClicked: {
                                confirmDeleteDialog.open()
                            }
                        }
                        Button {
                            text: "Hide/Show"
                            onClicked: SMSplat.toggleVisibility(name)
                        }
                    }
                }
                
                Dialog {
                    id: confirmDeleteDialog
                    title: "Confirm Delete"
                    
                    property bool isInDb: false
                    
                    onOpened: {
                        isInDb = isSavedInDb
                    }
                    
                    ColumnLayout {
                        Label {
                            text: confirmDeleteDialog.isInDb ? 
                                  "Delete from database and list?" : 
                                  "Delete from list?"
                        }
                        
                        RowLayout {
                            Button {
                                text: "Delete from List Only"
                                onClicked: {
                                    SMSplat.deleteItem(name, false)
                                    confirmDeleteDialog.close()
                                }
                            }
                            
                            Button {
                                text: "Delete from Database"
                                visible: confirmDeleteDialog.isInDb
                                onClicked: {
                                    SMSplat.deleteItem(name, true)
                                    confirmDeleteDialog.close()
                                }
                            }
                            
                            Button {
                                text: "Cancel"
                                onClicked: confirmDeleteDialog.close()
                            }
                        }
                    }
                }
            }
        }
        
        BusyIndicator {
            running: SMSplat.busy
            Layout.alignment: Qt.AlignCenter
        }
    }
    
    Dialog {
        id: createDialog
        objectName: "createDialog"
        title: "Create New SPLAT Analysis"
        width: Math.min(root.width * 0.9, 400)
        height: Math.min(root.height * 0.9, 600)
        modal: true
        anchors.centerIn: parent
        
        property var inputInfo: SMSplat.createInputInfo()
        onOpened: {
            inputInfo = SMSplat.createInputInfo()
        }
        
        ColumnLayout {
            anchors.fill: parent
            
            ScrollView {
                Layout.fillWidth: true
                Layout.fillHeight: true
                
                ColumnLayout {
                    width: parent.width
                    
                    Label { text: "Coverage Name:" }
                    TextField {
                        Layout.fillWidth: true
                        placeholderText: "Enter coverage name"
                        onTextChanged: {
                            if (createDialog.inputInfo) {
                                createDialog.inputInfo.transmitterSite = text
                            }
                        }
                    }
                    
                    Label { text: "Coverage Type:" }
                    ComboBox {
                        Layout.fillWidth: true
                        model: ["full", "partial"]
                        currentIndex: 0
                        onCurrentTextChanged: {
                            if (createDialog.inputInfo) {
                                createDialog.inputInfo.itmCovType = currentText
                            }
                        }
                    }
                    
                    Label { text: "Receiver Height:" }
                    SpinBox {
                        Layout.fillWidth: true
                        from: 0
                        to: 1000
                        value: 10
                        onValueChanged: {
                            if (createDialog.inputInfo) {
                                createDialog.inputInfo.receiverHeight = value
                            }
                        }
                    }
                    
                    // Additional fields would go here...
                }
            }
            
            RowLayout {
                Button {
                    text: "Generate"
                    Layout.fillWidth: true
                    onClicked: {
                        if (SMSplat.generate(createDialog.inputInfo)) {
                            createDialog.close()
                        }
                    }
                }
                
                Button {
                    text: "Cancel"
                    Layout.fillWidth: true
                    onClicked: createDialog.close()
                }
            }
        }
        
        onClosed: {
            if (inputInfo) {
                inputInfo.destroy()
                inputInfo = null
            }
        }
    }
    
    Dialog {
        id: importDialog
        title: "Import from Database"
        width: Math.min(root.width * 0.9, 400)
        height: Math.min(root.height * 0.9, 500)
        modal: true
        anchors.centerIn: parent
        
        property var availableSplats: []
        property var selectedSplats: []
        
        Component.onCompleted: {
            availableSplats = SMSplat.getAvailableSplatsFromDb()
        }
        
        ColumnLayout {
            anchors.fill: parent
            
            ListView {
                Layout.fillWidth: true
                Layout.fillHeight: true
                model: importDialog.availableSplats
                
                delegate: CheckDelegate {
                    width: ListView.view.width
                    text: modelData
                    checked: importDialog.selectedSplats.includes(modelData)
                    onCheckedChanged: {
                        if (checked) {
                            if (!importDialog.selectedSplats.includes(modelData)) {
                                importDialog.selectedSplats.push(modelData)
                            }
                        } else {
                            const index = importDialog.selectedSplats.indexOf(modelData)
                            if (index !== -1) {
                                importDialog.selectedSplats.splice(index, 1)
                            }
                        }
                    }
                }
            }
            
            RowLayout {
                Button {
                    text: "Import Selected"
                    Layout.fillWidth: true
                    onClicked: {
                        if (SMSplat.importFromDb(importDialog.selectedSplats)) {
                            importDialog.close()
                        }
                    }
                }
                
                Button {
                    text: "Cancel"
                    Layout.fillWidth: true
                    onClicked: importDialog.close()
                }
            }
        }
        
        onOpened: {
            availableSplats = SMSplat.getAvailableSplatsFromDb()
        }
    }
    
    Connections {
        target: SMSplat
        
        function onLastErrorChanged() {
            if (SMSplat.lastError) {
                errorDialog.text = SMSplat.lastError
                errorDialog.open()
            }
        }
    }
    
    Dialog {
        id: errorDialog
        title: "Error"
        modal: true
        anchors.centerIn: parent
        
        property string text: ""
        
        Label {
            text: errorDialog.text
            wrapMode: Text.WordWrap
        }
        
        standardButtons: Dialog.Ok
    }
    
    function evaluateQml(expression) {
        return eval(expression);
    }
}
