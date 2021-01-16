import QtQuick 2.15
import QtQuick.Controls 2.5
import QtQml.Models 2.12
import Qt.labs.qmlmodels 1.0
import QtQuick.Layouts 1.3

Page {
    Item {
        id: table
        anchors.fill: parent
        anchors.leftMargin: 5
        anchors.rightMargin: 5
        anchors.bottomMargin: 5

        function getColumnWidth(columnIndex, columnSpacing, tableWidth) {
            switch (columnIndex) {
                case 0: return 110
                case 1: return tableWidth - columnSpacing - 110
            }
            return 0
        }

        Row {
            id: tableheader
            width: tableview.contentWidth
            height: 40
            x: -tableview.contentX
            z: 1
            spacing: 5

            Repeater {
                model: tableview.model.columnCount()
                Rectangle {
                    width: table.getColumnWidth(index, tableheader.spacing, table.width)
                    height: parent.height
                    color: "green"

                    Text {
                        id: headerText
                        font.pixelSize: 15
                        anchors.fill: parent
                        anchors.leftMargin: 10
                        verticalAlignment: Text.AlignVCenter
                        x: 4
                        width: parent.width - 4
                        text: tableview.model.headerData(index, Qt.Horizontal)
                    }

                    // Hack to get tableview to resize on window resize
                    // columnWidthProvider not getting called properly
                    onWidthChanged: tableview.forceLayout()
                }
            }
        }

        TableView {
            id: tableview
            model: peopleTableModel
            focus: true
            flickableDirection: Flickable.VerticalFlick

            columnWidthProvider: function(column) {
                return table.getColumnWidth(column, columnSpacing, table.width);
            }

            anchors.fill: parent
            anchors.topMargin: tableheader.height + 5
            anchors.bottomMargin: buttonbar.height + 10 + 10
            columnSpacing: 5
            rowSpacing: 5

            ScrollBar.vertical: ScrollBar { }

            property var selectedRow: 0
            property var selectedColumn: 0

            // Enable the view button only when a table cell is selected
            onActiveFocusChanged: {
                viewButton.enabled = tableview.activeFocus || (viewButton.enabled && viewButton.activeFocus)
            }

            delegate: TextField {
                text: display
                implicitHeight: 50
                font.pixelSize: 15
                readOnly: true
                maximumLength: column === 0 ? 3 : 20

                background: Rectangle {
                    id: backgroundRectangle
                    border.color: "lightgray"
                    border.width: 1
                }

                // readOnly currently disables this
                onEditingFinished: {
                    // "model.edit = text" has problems
                    if (!tableview.model.setData(row, column, text)) {
                        text = display
                    }
                }

                onActiveFocusChanged: {
                    if (activeFocus) {
                        tableview.selectedRow = row
                        tableview.selectedColumn = column
                        backgroundRectangle.border.color = "blue"
                        backgroundRectangle.border.width = 2
                    } else {
                        readOnly = true
                        backgroundRectangle.border.color = "lightgray"
                        backgroundRectangle.border.width = 1
                    }
                }
            }
        }
    }

    RowLayout {
        id: buttonbar
        width: tableview.width
        height: 40
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 10
        anchors.left: parent.left
        anchors.leftMargin: 10
        anchors.right: parent.right
        anchors.rightMargin: 10

        Button {
            id: viewButton
            enabled: false
            text: "View"
            font.pointSize: 10
            Layout.preferredWidth: 100
            Layout.preferredHeight: tableheader.height

            onClicked: {
                viewPersonDialog.initials = tableview.model.data(tableview.selectedRow, "Identifier")
                viewPersonDialog.name = tableview.model.data(tableview.selectedRow, "Name")
                viewPersonDialog.open()
            }
        }
        Item { Layout.fillWidth: true } // Fill Spacer
        Button {
            id: addButton
            text: "Add"
            font.pointSize: 10
            Layout.preferredWidth: 100
            Layout.preferredHeight: tableheader.height

            onClicked: {
                addPersonDialog.initials = ""
                addPersonDialog.name = ""
                addPersonDialog.open()
            }
        }
    }

    PersonInputDialog {
        id: addPersonDialog
        onSavePressed: {
            tableview.model.addPerson(addPersonDialog.initials, addPersonDialog.name)
        }
    }
    PersonInputDialog {
        id: viewPersonDialog
        onSavePressed: {
            tableview.model.setData(tableview.selectedRow, "Identifier", viewPersonDialog.initials)
            tableview.model.setData(tableview.selectedRow, "Name", viewPersonDialog.name)
        }
        onDeletePressed: {
            tableview.model.removeRows(tableview.selectedRow, 1)
        }
        onClosed: {
            viewButton.enabled = false
        }
    }
}
