import Mixxx 1.0 as Mixxx
import QtQuick 2.15
import QtQuick.Shapes 1.4
import QtQuick.Window 2.15

import QtQuick.Controls 2.15

import "Theme"

Item {
    id: root

    property color color: Theme.waveformMarkerDefault

    property real markerHeight: root.height
    property color labelColor: Theme.waveformMarkerLabel
    property real radiusSize: 2
    property string cueLabel: qsTr("CUE")

    FontMetrics {
        id: fontMetrics
        font.family: Theme.fontFamily
        font.pixelSize: 12
        font.weight: Font.Normal
    }

    property rect contentRect: fontMetrics.tightBoundingRect(cueLabel)

    Shape {
        ShapePath {
            strokeWidth: 0
            strokeColor: 'transparent'
            fillColor: color
            strokeStyle: ShapePath.SolidLine
            startX: -1; startY: 0

            PathArc {
                x: 2
                y: radiusSize /2
                radiusX: radiusSize /2; radiusY: radiusSize /2
                direction: PathArc.Clockwise
            }
            PathLine { x: 4 - radiusSize + contentRect.width; y: radiusSize /2 }
            PathArc {
                x: 4 + contentRect.width
                y: radiusSize * 1.5
                radiusX: radiusSize; radiusY: radiusSize
                direction: PathArc.Clockwise
            }
            PathLine { x: 4 + contentRect.width; y: 16 - radiusSize /2 }
            PathArc {
                x: 4 - radiusSize + contentRect.width
                y: 16 + radiusSize /2
                radiusX: radiusSize; radiusY: radiusSize
                direction: PathArc.Clockwise
            }
            PathLine { x: 4; y: 16 + radiusSize /2 }
            PathLine { x: 2; y: 16 + radiusSize /2 }
            PathLine { x: 2; y: markerHeight }
            PathLine { x: -1; y: markerHeight }
            PathLine { x: -1; y: 0 }
        }
    }
    Shape {
        ShapePath {
            fillColor: labelColor
            strokeColor: labelColor
            PathText {
                x: 2
                y: 4
                font.family: Theme.fontFamily
                font.pixelSize: 12
                font.weight: Font.Normal
                text: cueLabel
            }
        }
    }
}
