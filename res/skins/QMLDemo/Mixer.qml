import QtQuick 2.12
import QtQuick.Controls 2.12

Row {
    id: mixer

    required property string leftDeckGroup
    required property string rightDeckGroup
    property bool show4decks: false

    EqColumn {
        group: root.leftDeckGroup
    }

    MixerColumn {
        width: 56
        height: parent.height
        group: root.leftDeckGroup
    }

    MixerColumn {
        width: 56
        height: parent.height
        group: root.rightDeckGroup
    }

    EqColumn {
        width: 56
        height: parent.height
        group: root.rightDeckGroup
    }

}