import QtQuick
import QtQuick.Controls

Image {
    id: root
    property bool indeterminateState: false
    property bool checked: false
    width: 20
    height: width
    state: "unchecked"
    source: "qrc:/img/uncheck.svg"

    states: [
        State {
            name: "checked"
            PropertyChanges {
                target: root
                source: "qrc:/img/check.svg"
            }
        },
        
        State {
            name: "indeterminate"
            PropertyChanges {
                target: root
                source: "qrc:/img/indeterminate.svg"
            }
        },
        
        State {
            name: "unchecked"
            PropertyChanges {
                target: root
                source: "qrc:/img/uncheck.svg"
            }
        }
    ]

    MouseArea {
        anchors.fill: parent
        onClicked: {
            if (indeterminateState) {
                root.state = "indeterminate"
            } else {
                if (root.state == "checked") {
                    root.state = "unchecked"
                    checked = false
                } else if (root.state == "unchecked") {
                    root.state = "checked"
                    checked = true
                }
            }
            
        }
    }
}