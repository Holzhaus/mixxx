import "." as Skin
import Mixxx 0.1 as Mixxx
import Mixxx.Controls 0.1 as MixxxControls
import QtQuick 2.12
import "Theme"

Rectangle {
    id: root

    property alias knob: knob
    property string statusGroup: root.knob.group // required
    property string statusKey // required
    property alias label: labelText.text

    color: Theme.knobBackgroundColor
    width: 56
    height: 56
    radius: 5

    Text {
        id: labelText

        anchors.bottom: parent.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        horizontalAlignment: Text.AlignHCenter
        elide: Text.ElideRight
        font.family: Theme.fontFamily
        font.bold: true
        font.pixelSize: Theme.knobFontPixelSize
        color: Theme.buttonNormalColor
    }

    Skin.ControlKnob {
        id: knob

        anchors.centerIn: root
        width: 48
        height: 48
    }

    Mixxx.ControlProxy {
        id: statusControl

        group: root.statusGroup
        key: root.statusKey
    }

    Rectangle {
        id: statusButton

        anchors.left: root.left
        anchors.bottom: root.bottom
        anchors.leftMargin: 4
        anchors.bottomMargin: 4
        width: 8
        height: width
        radius: width / 2
        border.width: 1
        border.color: Theme.buttonNormalColor
        color: statusControl.value ? knob.color : "transparent"

        TapHandler {
            onTapped: statusControl.value = !statusControl.value
        }

    }

}
