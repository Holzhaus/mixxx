import Mixxx.Controls 0.1 as MixxxControls
import QtQuick 2.12
import QtQuick.Controls 2.12
import "Theme"

MixxxControls.Knob {
    id: root

    property color color // required

    implicitWidth: 56
    implicitHeight: implicitWidth
    arc: true
    arcRadius: 20
    arcColor: root.color
    arcWidth: 2
    angle: 117

    Popup {
        id: popup

        visible: root.pressed
        width: root.width
        x: -width - 5
        y: root.height / 2 - height / 2

        Text {
            text: (control.parameter * 100).toFixed(0) + "%"
            font.family: Theme.fontFamily
            font.bold: true
            font.pixelSize: Theme.textFontPixelSize
            color: Theme.deckTextColor
            anchors.centerIn: parent
        }

        background: BorderImage {
            id: backgroundImage

            anchors.fill: parent
            horizontalTileMode: BorderImage.Stretch
            verticalTileMode: BorderImage.Stretch
            source: "images/button.svg"

            border {
                top: 10
                left: 20
                right: 20
                bottom: 10
            }

        }

    }

    background: Image {
        anchors.left: parent.left
        anchors.top: parent.top
        anchors.right: parent.right
        source: "images/knob.svg"
    }

    foreground: Item {
        anchors.fill: parent

        Rectangle {
            anchors.horizontalCenter: parent.horizontalCenter
            width: root.width / 30
            height: root.width / 7
            y: parent.height / 4
            color: root.color
        }

    }

}
