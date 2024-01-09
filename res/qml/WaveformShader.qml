import Mixxx 1.0 as Mixxx
import QtQuick 2.12

ShaderEffect {
    id: root

    property string group // required
    property var deckPlayer: Mixxx.PlayerManager.getPlayer(group)
    property size framebufferSize: Qt.size(width, height)
    property int waveformLength: root.deckPlayer.waveformLength
    property int textureSize: root.deckPlayer.waveformTextureSize
    property int textureStride: root.deckPlayer.waveformTextureStride
    property real firstVisualIndex: 1
    property real lastVisualIndex: root.deckPlayer.waveformLength / 2
    property color axesColor: "#FFFFFF"
    property color highColor: "#0000FF"
    property color midColor: "#00FF00"
    property color lowColor: "#FF0000"

    property var visualGainAll: Mixxx.Config.getDouble("[Waveform]", "VisualGain_0", 1.0)
    property var visualGainLow: Mixxx.Config.getDouble("[Waveform]", "VisualGain_1", 1.0)
    property var visualGainMid: Mixxx.Config.getDouble("[Waveform]", "VisualGain_2", 1.0)
    property var visualGainHigh: Mixxx.Config.getDouble("[Waveform]", "VisualGain_3", 1.0)

    property real highGain: (filterWaveformEnableControl.value ? (filterHighKillControl.value ? 0 : filterHighControl.value) : 1) * visualGainHigh
    property real midGain: (filterWaveformEnableControl.value ? (filterMidKillControl.value ? 0 : filterMidControl.value) : 1) * visualGainMid
    property real lowGain: (filterWaveformEnableControl.value ? (filterLowKillControl.value ? 0 : filterLowControl.value) : 1) * visualGainLow
    property real allGain: gainControl.value * 2.0 * visualGainAll // See WaveformWidgetRenderer::getGain()

    Component.onCompleted: () => {
        console.warn(`visualGainAll: ${visualGainAll}, visualGainLow: ${visualGainLow}, visualGainMid: ${visualGainMid}, visualGainHigh: ${visualGainHigh}`)
    }
    property Image waveformTexture

    fragmentShader: "qrc:/shaders/rgbsignal_qml.frag.qsb"

    Mixxx.ControlProxy {
        id: gainControl

        group: root.group
        key: "total_gain"
    }

    Mixxx.ControlProxy {
        id: filterWaveformEnableControl

        group: root.group
        key: "filterWaveformEnable"
    }

    Mixxx.ControlProxy {
        id: filterHighControl

        group: root.group
        key: "filterHigh"
    }

    Mixxx.ControlProxy {
        id: filterHighKillControl

        group: root.group
        key: "filterHighKill"
    }

    Mixxx.ControlProxy {
        id: filterMidControl

        group: root.group
        key: "filterMid"
    }

    Mixxx.ControlProxy {
        id: filterMidKillControl

        group: root.group
        key: "filterMidKill"
    }

    Mixxx.ControlProxy {
        id: filterLowControl

        group: root.group
        key: "filterLow"
    }

    Mixxx.ControlProxy {
        id: filterLowKillControl

        group: root.group
        key: "filterLowKill"
    }

    waveformTexture: Image {
        visible: false
        layer.enabled: false
        source: root.deckPlayer.waveformTexture
    }
}
