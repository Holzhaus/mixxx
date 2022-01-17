import "." as Skin
import QtQuick 2.12
import QtQuick.Controls 2.12
import "Theme"

Skin.Button {
    //activeColor: hotcue.color
    //highlight: hotcue.isSet

    id: root

    property int hotcueNumber // required
    property string group // required

    text: hotcueNumber
    width: playButton.height
    height: playButton.height

    Hotcue {
        id: hotcue

        group: root.group
        hotcueNumber: root.hotcueNumber
        activate: root.down
        onIsSetChanged: {
            if (!isSet)
                popup.close();

        }
    }

    HotcuePopup {
        id: popup

        hotcue: hotcue
    }

    MouseArea {
        id: mousearea

        anchors.fill: parent
        acceptedButtons: Qt.RightButton
        onClicked: (mouse) => {
            if (hotcue.isSet) {
                popup.x = mouse.x;
                popup.y = mouse.y;
                popup.open();
            }
        }
    }

}
