import QtQuick
import QtQuick.Controls
import SMSplat 1.0

ListView {
    id: coverageListView
    required property var inputmenu
    
    width: parent.width
    height: contentHeight
    model: SMSplat.splatModel
    
    property string selectedEntry: ""
    property var selectedInput: null
    
    delegate: ListEntry {
        required property var name
        required property var isSavedInDb
        
        entryName: name
        isInDB: {
            return isSavedInDb
        }
        
        isSelected: coverageListView.selectedEntry === name
        
        onEntryClicked: {
            coverageListView.selectedEntry = name
            selectedInput = SMSplat.inputModel.getByNameQml(entryName)
            inputmenu.showInputData(selectedInput)
        }
    }
}