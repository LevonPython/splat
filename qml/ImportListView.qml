import QtQuick
import QtQuick.Controls
import SMSplat 1.0

ListView {
    id: importListView
    
    width: parent.width
    height: contentHeight
    model: SMSplat.getAvailableSplatsFromDb()
    
    property var selectedEntries: []
    delegate: ImportListEntry {
        
        entryName: modelData
        
        isSelected: selectedEntries_.includes(modelData)
        isAlreadyImported: SMSplat.splatModel.contains(modelData)
        
        onIsSelectedChanged : {
            if (isSelected) {
                selectedEntries_.push(modelData)
            } else {
                selectedEntries.splice(selectedEntries.indexOf(modelData), 1)
            }
        }
    }
}