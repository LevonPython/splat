import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import SMSplat 1.0

Rectangle {
    id: root

    function setHeight(h) {
        height = h
    }

    function showInputData(input) {
        console.log("input", input.transmitterName)
        nameField.setFieldValue(input.transmitterName)
        dropField.setFieldValue(input.itmCovType)
        tLatitude.setFieldValue(input.transmitterLat)
        tLongitude.setFieldValue(input.transmitterLon)
        tAltitude.setFieldValue(input.transmitterAlt)
        rAltitude.setFieldValue(input.receiverHeight)
        covRad.setFieldValue(input.radius)
        freq.setFieldValue(input.frequency)
        fresZone.setFieldValue(input.fresnelZone)
        angleStart.setFieldValue(input.startAngle)
        angleEnd.setFieldValue(input.endAngle)

    }
    function getFieldValues(input) {
        input.transmitterName = nameField.fieldValue ? nameField.fieldValue : "default"
        input.itmCovType = dropField.fieldValue ? dropField.fieldValue : "default"
        input.transmitterLat = tLatitude.fieldValue ? parseFloat(tLatitude.fieldValue) : 0.0
        input.transmitterLon = tLongitude.fieldValue ? parseFloat(tLongitude.fieldValue) : 0.0
        input.transmitterAlt = tAltitude.fieldValue ? parseFloat(tAltitude.fieldValue) : 0.0
        input.receiverHeight = rAltitude.fieldValue ? parseFloat(rAltitude.fieldValue) : 0.0
        input.radius = covRad.fieldValue ? parseFloat(covRad.fieldValue) : 0.0
        input.frequency = freq.fieldValue ? parseFloat(freq.fieldValue) : 0.0
        input.fresnelZone = fresZone.fieldValue ? parseFloat(fresZone.fieldValue) : 0.0
        input.startAngle = angleStart.fieldValue ? parseFloat(angleStart.fieldValue) : 0.0
        input.endAngle = angleEnd.fieldValue ? parseFloat(angleEnd.fieldValue) : 360.0
        return input
    }

    function reset() {
        nameField.setFieldValue("")
        dropField.setFieldValue("")
        tLatitude.setFieldValue("")
        tLongitude.setFieldValue("")
        tAltitude.setFieldValue("")
        rAltitude.setFieldValue("")
        covRad.setFieldValue("")
        freq.setFieldValue("")
        fresZone.setFieldValue("")
        angleStart.setFieldValue("")
        angleEnd.setFieldValue("")
    }

    function clearData() {
        showInputData("")
    }

    width: parent.width
    height: 460
    color: "transparent"
    property bool expanded: false
    property var inputStruct

    Flickable { 
        id: flickable
        clip: true
        flickableDirection: Flickable.VerticalFlick
        height: root.height
        width: root.width
        contentHeight: textFields.height + button.height 
        contentWidth: root.width
        anchors.top: root.top
        anchors.horizontalCenter: root.horizontalCenter
        
        Column {    
            id: textFields
            spacing: 12
            anchors.centerIn: parent
            
            InputField {
                id: nameField
                title: qsTr("Name")
                regexp: RegularExpressionValidator {
                        regularExpression: RegExp("^[a-zA-Z0-9]{1,20}$") // Allows only letters and numbers with a max length of 20
                    }
            }

            DropdownField {
                id: dropField
                title: qsTr("Node type")
                z: 2
            }

            InputField {
                id: tLatitude
                title: qsTr("Transmitter latitude")
                regexp: RegularExpressionValidator {
                        //should be written properly, missing for now
                        //regularExpression: RegExp("^-?([0-8]?[0-9]|90)(\.\d+)?$") // between -90 and 90
                    }
            }
            
            InputField {
                id: tLongitude
                title: qsTr("Transmitter longitude")
                regexp: RegularExpressionValidator {
                        //regularExpression: RegExp("^-?((1[0-7]?[0-9]|[0-9]?[0-9])(\.\d+)?|180(\.0+)?)$") // between -180 and 180
                    }
            }
            
            InputField {
                id: tAltitude
                title: qsTr("Transmitter altitude (m)")
                regexp: RegularExpressionValidator {
                       //regularExpression: RegExp("^[0-9]+(\.[0-9]+)?$") 
                    }
            }
            
            InputField {
                id: rAltitude
                visible: expanded
                title: qsTr("Receiver altitude (m)")
                regexp: RegularExpressionValidator {
                       //regularExpression: RegExp("^[0-9]+(\.[0-9]+)?$") 
                    }
            }
            
            InputField {
                id: covRad
                visible: expanded
                title: qsTr("Coverage radius")
                regexp: RegularExpressionValidator {
                        //regularExpression: RegExp("^[0-9]+(\.[0-9]+)?$")
                    }
            }

            InputField {
                id: freq
                visible: expanded
                title: qsTr("Frequency")
                // Don't know which checking should be performed here
                /*regexp: RegularExpressionValidator {
                        regularExpression: RegExp("^\d+(\.\d+)?$") // positive integer or decimal
                    }*/

            }

            InputField {
                id: fresZone
                visible: expanded
                title: qsTr("Fresnel zone")
                /*regexp: RegularExpressionValidator {
                        regularExpression: RegExp("^\d+(\.\d+)?$") // positive integer or decimal
                    }*/
            }

            InputField {
                id: angleStart
                visible: expanded
                title: qsTr("View angle start")
                regexp: RegularExpressionValidator {
                        //regularExpression: RegExp("^[0-9]+(\.[0-9]+)?$")
                    }
            }
            
            InputField {
                id: angleEnd
                visible: expanded
                title: qsTr("View angle end")
                regexp: RegularExpressionValidator {
                        //regularExpression: RegExp("^[0-9]+(\.[0-9]+)?$")
                    }
            }
        }
    }
    
    RectangleButton {
        id: button
        width: root.width
        height: 30
        anchors.top: root.bottom
        anchors.horizontalCenter: root.horizontalCenter
        buttonName: expanded ? "SHOW LESS" : "SHOW MORE"
        onBtnClicked: {
            root.expanded = !root.expanded
        }
    }
}