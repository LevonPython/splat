import QtQuick
import QtQuick.Controls
import SMSplat 1.0

Rectangle {
    id: root
    
    signal buttonClicked()

    required property string buttonName
    required property real w
    required property real h
    required property string colorScheme 

    onColorSchemeChanged: {
        initColors()
    }
    // this can be whiteNblue, whiteNgray, whiteNred, red and blue
    
    width: w
    height: h 
    radius: height/2
    border.width: colorScheme == "red" || colorScheme == "blue" ? 0 : 1
    border.color: initBorderColor()

    MouseArea {
        id: area
        anchors.fill: parent
        hoverEnabled: true
        onClicked: {
            buttonClicked()
        }
    }

    Text {
        text: buttonName
        anchors.centerIn: parent
        color: initTextColor()
        font.pixelSize: 14
    }

    property color enabledColor
    property color hoveredColor
    property color pressedColor

    function initColors() {
        if(colorScheme == "whiteNgray") {
            enabledColor = "white"
            hoveredColor = SplatQmlGlobals.hover
            pressedColor = SplatQmlGlobals.pressed
            return
        } 
        if(colorScheme == "whiteNblue") {
            enabledColor = "white"
            hoveredColor = SplatQmlGlobals.lightHover
            pressedColor = SplatQmlGlobals.listSelectedHover
            return
        } 
        if(colorScheme == "whiteNred") {
            enabledColor = "white"
            hoveredColor = SplatQmlGlobals.lightRedHovered
            pressedColor = SplatQmlGlobals.lightRedPressed
            return
        } 
        if(colorScheme == "blue") {
            enabledColor = SplatQmlGlobals.primaryEnabled
            hoveredColor = SplatQmlGlobals.primaryHover
            pressedColor = SplatQmlGlobals.primaryPressed
            return
        } 
        if(colorScheme == "red") {
            enabledColor = SplatQmlGlobals.red
            hoveredColor = SplatQmlGlobals.red
            pressedColor = SplatQmlGlobals.darkRed
            return
        } 
    } 

    function initBorderColor() {
        if(colorScheme == "whiteNgray") {
            return SplatQmlGlobals.outlineBorder
        } else if(colorScheme == "whiteNblue") {
            return SplatQmlGlobals.primaryEnabled
        } else if(colorScheme == "whiteNred") {
            return SplatQmlGlobals.red
        } else {
            return enabledColor
        }
    }

    function initTextColor() {
        if(colorScheme == "whiteNgray") {
            return SplatQmlGlobals.textSecondary
        } else if(colorScheme == "whiteNblue") {
            return SplatQmlGlobals.primaryEnabled
        } else if(colorScheme == "whiteNred") {
            return SplatQmlGlobals.red
        } else {
            return "white"
        }
    }

    states: [
        State {
            name: "enabled"
            when: !area.containsMouse && !area.containsPress
            PropertyChanges {
                target: root
                color: enabledColor
            } 
        },
        State {
            name: "hovered"
            when: area.containsMouse && !area.containsPress
            PropertyChanges {
                target: root
                color: hoveredColor
            } 
        },
        State {
            name: "pressed"
            when: area.containsPress
            PropertyChanges {
                target: root
                color: pressedColor
            } 
        }
    ]
}