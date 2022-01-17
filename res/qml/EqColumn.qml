import "." as Skin
import Mixxx.Controls 0.1 as MixxxControls
import QtQuick 2.12
import QtQuick.Shapes 1.12
import "Theme"

Column {
    id: root

    property string group // required

    spacing: 4

    EqKnob {
        //knob.color: Theme.eqHighColor

        statusKey: "button_parameter3"
        knob.group: "[EqualizerRack1_" + root.group + "_Effect1]"
        knob.key: "parameter3"
    }

    EqKnob {
        //knob.color: Theme.eqMidColor

        statusKey: "button_parameter2"
        knob.group: "[EqualizerRack1_" + root.group + "_Effect1]"
        knob.key: "parameter2"
    }

    EqKnob {
        //knob.color: Theme.eqLowColor

        knob.group: "[EqualizerRack1_" + root.group + "_Effect1]"
        knob.key: "parameter1"
        statusKey: "button_parameter1"
    }

    EqKnob {
        //knob.color: Theme.eqFxColor

        knob.group: "[QuickEffectRack1_" + root.group + "]"
        knob.key: "super1"
        statusGroup: "[QuickEffectRack1_" + root.group + "_Effect1]"
        statusKey: "enabled"
        knob.arcStyle: ShapePath.DashLine
        knob.arcStylePattern: [2, 2]
    }

    Skin.OrientationToggleButton {
        //color: Theme.crossfaderOrientationColor

        group: root.group
        key: "orientation"
    }

}
