import QtQuick 2.15
import QtQuick.Controls 2.12
import QtQml.Models 2.12
import Qt.labs.qmlmodels 1.0
import QtQuick.Layouts 1.3

Page {
    ColumnLayout {
        anchors.fill: parent
        spacing: 80
        Item {
            Layout.fillWidth: true
            Layout.fillHeight: true

            Row {
                id: tableviewheader
                width: tableview.contentWidth
                height: 40
                x: -tableview.contentX
                z: 1
                spacing: 5

                Repeater {
                    model: tableview.model.columnCount()
                    Rectangle {
                        width: tableview.model.columnWidth(index, font)
                        height: parent.height
                        color: "green"

                        Text {
                            id: texttt
                            font.pixelSize: 15
                            anchors.fill: parent
                            anchors.leftMargin: 10
                            verticalAlignment: Text.AlignVCenter
                            //anchors.verticalCenter: parent.verticalCenter
                            x: 4
                            width: parent.width - 4
                            text: tableview.model.headerData(index, Qt.Horizontal)
                        }
                    }
                }
            }

            TableView {
                id: tableview
                model: resultsModel
                focus: true

                columnWidthProvider: function(column, font) { return Math.min(600, model.columnWidth(column, font)) }
                anchors.fill: parent
                anchors.topMargin: tableviewheader.height
                columnSpacing: 5
                rowSpacing: 5

                property var selectedRow: 0
                property var selectedColumn: 0

                // Enable the view button only when a table cell is selected
                onActiveFocusChanged: {
                    viewButton.enabled = tableview.activeFocus || (viewButton.enabled && viewButton.activeFocus)
                }

                delegate: Rectangle {
                    implicitHeight: 50
                    border.color: "black"

                    TextField {
                        id: textField
                        text: display
                        font.pixelSize: 15
                        anchors.fill: parent
                        width: parent.width
                        readOnly: true

                        onActiveFocusChanged: {
                            if (textField.activeFocus) {
                                tableview.selectedRow = row
                                tableview.selectedColumn = column
                            }
                        }
                    }
                }
            }
        }
    }
}
