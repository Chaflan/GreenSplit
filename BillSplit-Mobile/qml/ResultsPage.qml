import QtQuick 2.15
import QtQuick.Controls 2.12
import QtQml.Models 2.12
import Qt.labs.qmlmodels 1.0
import QtQuick.Layouts 1.3

Page {
    Item {
        id: table
        anchors.fill:parent
        anchors.leftMargin: 5
        anchors.rightMargin: 5


        Row {
            id: tableheader
            width: listview.contentWidth
            height: 40
            x: -listview.contentX
            z: 1
            spacing: 5

            Rectangle {
                width: 200
                // TODO: Fix these.  Width needs to be tied to something, but god knows what
                //width: tableview.model.columnWidth(index, tableheader.spacing, table.width)
                height: parent.height
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

            anchors.fill: parent
            anchors.topMargin: tableheader.height + 5

            // TODO: WHY>?!?!
            spacing: -5

            delegate: Rectangle {
                id: delegate
                implicitHeight: 50
                border.color: "black"

                // For some dumb reason only TextField shows a border, not Text
                TextField {
                    id: textField
                    text: display
                    font.pixelSize: 15
                    readOnly: true
                }
            }
        }
    }
}
