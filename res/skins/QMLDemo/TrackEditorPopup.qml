import QtQuick 2.12
import QtQuick.Controls 2.12
import "Theme"

Popup {
    id: root

    property var deckPlayer // required

    Grid {
        columns: 2
        flow: Grid.TopToBottom
        spacing: 5

        Repeater {
            model: ["Title", "Artist", "Album", "Album Artist", "Genre", "Composer", "Grouping", "Year", "Comment"]

            Text {
                text: modelData + ":"
                font.family: Theme.fontFamily
                font.pixelSize: Theme.textFontPixelSize
                color: Theme.deckTextColor
                width: 100
                height: 30
            }

        }

        Repeater {
            model: ["title", "artist", "album", "albumArtist", "genre", "composer", "grouping", "year", "comment"]

            TextField {
                text: root.deckPlayer[modelData]
                onEditingFinished: {
                    root.deckPlayer[modelData] = text;
                }
                font.family: Theme.fontFamily
                font.pixelSize: Theme.textFontPixelSize
                color: Theme.deckTextColor
                width: 200
                height: 30

                background: Rectangle {
                    anchors.fill: parent
                    radius: 5
                    color: Theme.deckBackgroundColor
                }

            }

        }

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
