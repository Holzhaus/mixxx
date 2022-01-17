import "." as Skin
import QtQuick 2.12
import "Theme"

Row {
    id: root

    property int unitNumber // required
    property string group: unitNumber === 1 ? "[Microphone]" : "[Microphone" + unitNumber + "]"

    spacing: 5

    Skin.VuMeter {
        id: vuMeter

        group: root.group
        key: "VuMeter"
        width: 4
        height: parent.height
    }

    Rectangle {
        id: gainKnobFrame

        width: 52
        height: width
        color: Theme.knobBackgroundColor
        radius: 5

        Skin.ControlKnob {
            //color: Theme.gainKnobColor

            id: gainKnob

            anchors.centerIn: parent
            width: 48
            height: width
            arcStart: Knob.ArcStart.Minimum
            group: root.group
            key: "pregain"
        }

    }

    Column {
        SectionText {
            width: parent.width
            height: root.height / 2
            text: "MIC " + root.unitNumber
        }

        Skin.ControlButton {
            id: pflButton

            group: root.group
            key: "pfl"
            text: "PFL"
            //activeColor: Theme.pflActiveButtonColor
            toggleable: true
        }

    }

    Skin.EmbeddedBackground {
        id: embedded

        height: parent.height
        width: 56

        Skin.InfoBarButton {
            //activeColor: Theme.deckActiveColor

            id: talkButton

            anchors.left: parent.left
            anchors.right: parent.right
            anchors.top: parent.top
            anchors.bottom: parent.verticalCenter
            group: root.group
            key: "talkover"

            foreground: Skin.EmbeddedText {
                anchors.centerIn: parent
                text: "TALK"
            }

        }

        Skin.InfoBarButton {
            //activeColor: Theme.deckActiveColor

            id: fx1Button

            anchors.left: parent.left
            anchors.right: parent.horizontalCenter
            anchors.top: parent.verticalCenter
            anchors.bottom: parent.bottom
            group: "[EffectRack1_EffectUnit1]"
            key: "group_" + root.group + "_enable"

            foreground: Skin.EmbeddedText {
                anchors.centerIn: parent
                text: "FX1"
            }

        }

        Skin.InfoBarButton {
            //activeColor: Theme.deckActiveColor

            group: "[EffectRack1_EffectUnit2]"
            anchors.left: parent.horizontalCenter
            anchors.right: parent.right
            anchors.top: parent.verticalCenter
            anchors.bottom: parent.bottom
            key: "group_" + root.group + "_enable"

            foreground: Skin.EmbeddedText {
                anchors.centerIn: parent
                text: "FX2"
            }

        }

    }

}
