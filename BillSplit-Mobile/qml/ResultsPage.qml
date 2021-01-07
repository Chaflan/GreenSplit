import QtQuick 2.15
import QtQuick.Controls 2.12
import QtQml.Models 2.12
import Qt.labs.qmlmodels 1.0
import QtQuick.Layouts 1.3

Page {
    id: page
    Item {
        id: table
        anchors.fill:parent
        anchors.leftMargin: 5
        anchors.rightMargin: 5

        Row {
            id: tableheader
            //width: Math.max(maxLetterCount * 12, parent.width)
            anchors.left: parent.left
            anchors.right: parent.right
            height: 40
            x: -listview.contentX
            z: 1
            spacing: 5

            Rectangle {
                width: parent.width
                anchors.top: parent.top
                anchors.bottom: parent.bottom
                color: "green"

                Text {
                    id: texttt
                    font.pixelSize: 15
                    anchors.fill: parent
                    anchors.leftMargin: 10
                    verticalAlignment: Text.AlignVCenter
                    x: 4
                    width: parent.width - 4
                    text: "Results"
                }
            }
        }

        ListView {
            id: listview
            model: resultsModel
            focus: true
            flickableDirection: Flickable.HorizontalAndVerticalFlick

            anchors.top: parent.top
            anchors.bottom: parent.bottom
            width: Math.max(listview.model.maxLetterCount * 12, parent.width)
            contentWidth: Math.max(listview.model.maxLetterCount * 12, parent.width)


            //anchors.fill: parent
            anchors.topMargin: tableheader.height + 5
            spacing: 5

            delegate: TextField {
                id: textField
                text: display
                implicitHeight: 50
                font.pixelSize: 15
                readOnly: true

                // Binding these causes strange errors
                // when names are updated
                Component.onCompleted: {
                    anchors.left = parent.left
                    anchors.right = parent.right
                }

                background: Rectangle {
                    id: backgroundRectangle
                    border.color: "lightgray"
                    border.width: 1
                }

                onActiveFocusChanged: {
                    if (activeFocus) {
                        backgroundRectangle.border.color = "blue"
                        backgroundRectangle.border.width = 2
                    } else {
                        backgroundRectangle.border.color = "lightgray"
                        backgroundRectangle.border.width = 1
                    }
                }
            }
        }
    }
}
