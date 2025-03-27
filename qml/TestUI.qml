import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import SMSplat 1.0

ApplicationWindow {

    id: root
    title: "SPLAT Manager"
    visible: true
    width: 1920
    height: 1080
    background: Rectangle {
        color: "gray"
    }

    SplatUI {
        anchors {
            left: parent.left
            top: parent.top
        }
    } 
}