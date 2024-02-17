import Mixxx 1.0 as Mixxx
import "." as Skin
import QtQuick 2.12
import QtQuick.Layouts 1.12

RowLayout {
    spacing: 0

    Mixxx.ControlProxy {
        id: numSamplersControl

        group: "[App]"
        key: "num_samplers"
    }

    Repeater {
        model: Math.trunc(Mixxx.MathUtils.clamp(numSamplersControl.value, 4, 8));

        Skin.Sampler {
            required property int index

            Layout.fillWidth: true
            group: "[Sampler" + (index + 1) + "]"
        }
    }

    Component.onCompleted: if (numSamplersControl.value < 8) {
        numSamplersControl.value = 8;
    }
}
