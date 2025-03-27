import QtQuick
import QtQuick.Controls

Rectangle {
    property bool isSelected: false
    property string entryName: ""
    property bool isAlreadyImported: false
    signal entryClicked()
    width: parent.width
    height: 36
    color: isSelected ? SplatQmlGlobals.listSelectedHover : "white"
    
    Row {
        anchors.verticalCenter: parent.verticalCenter
        anchors.left: parent.left
        anchors.leftMargin: 16
        spacing: 12
        SplatCheckBox {
            id: checkBox
            state: isAlreadyImported ? "indeterminate" : (isSelected ? "checked" : "unchecked")
            onCheckedChanged: {
                console.log("checkBox.checked", checkBox.checked)
                isSelected = checkBox.checked
            }
        }

        Text {
            text: entryName
            color: isAlreadyImported ? SplatQmlGlobals.textSecondary : "black"
        }
    }

    //MouseArea {
    //    id: mArea
    //    anchors.fill: parent
    //    hoverEnabled: true
    //    onClicked: {
    //        entryClicked()
    //    }
    //}
}