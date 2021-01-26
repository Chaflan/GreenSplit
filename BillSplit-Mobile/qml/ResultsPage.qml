import QtQuick 2.12
import QtQuick.Controls 2.12
import BillSplit.Core 1.0

Page {
    id: page
    Item {
        id: table
        anchors.fill:parent
        anchors.leftMargin: 5
        anchors.rightMargin: 5

        ResultsModel {
            id: resultsModel
            dataCore: dataCoreInstance
        }

        Row {
            id: listheader
            width: listview.width
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
            width: Math.max(listview.model.getMaxLetterCount() * 9, parent.width)

            onWidthChanged: listheader.width = listview.width

            anchors.topMargin: listheader.height + 5
            spacing: 5

            delegate: TextField {
                id: textField
                text: display
                width: listview.width
                implicitHeight: 50
                font.pixelSize: 15
                readOnly: true

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
