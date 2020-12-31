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
                    width: tableview.model.columnWidth(index, tableheader.spacing, table.width)
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

            columnWidthProvider: function(column) {
                return model.columnWidth(column, columnSpacing, table.width);
            }

            anchors.fill: parent
            anchors.topMargin: tableheader.height + 5
            anchors.bottomMargin: tablefooter.height + 5
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
                maximumLength: column === 0 ? 3 : 30

                background: Rectangle {
                    id: backgroundRectangle
                    border.color: "lightgray"
                    border.width: 1
                }

                onEditingFinished: {
                    // "model.edit = text" has problems
                    if (!tableview.model.setData(row, column, text)) {
                        text = display
                    }
                }

                onFocusChanged: { if(focus) { selectAll() } } // Select all on click

                onActiveFocusChanged: {
                    if (activeFocus) {
                        tableview.selectedRow = row
                        tableview.selectedColumn = column
                        backgroundRectangle.border.color = "blue"
                        backgroundRectangle.border.width = 2
                    } else {
                        backgroundRectangle.border.color = "lightgray"
                        backgroundRectangle.border.width = 1
                    }
                }
            }
        }

        RowLayout {
            id: tablefooter
            width: tableview.width
            height: 40
            x: -tableview.contentX
            y: -tableview.contentY + tableview.contentHeight + tableheader.height + tableview.rowSpacing + 5

            Button {
                id: viewButton
                enabled: false
                text: "View"
                font.pointSize: 10
                Layout.preferredWidth: 100
                Layout.preferredHeight: tableheader.height
                Layout.leftMargin: 0

                onClicked: {
                    viewPersonDialog.initials = tableview.model.getData(tableview.selectedRow, "Identifier")
                    viewPersonDialog.name = tableview.model.getData(tableview.selectedRow, "Name")
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
                Layout.rightMargin: 0

                onClicked: {
                    if (tableview.model.addDefaultPerson()) {
                        // Adjust table to scroll down with the adds
                        var heightDifference = tableview.contentHeight - tableview.height
                        if (heightDifference > 0) {
                            tableview.contentY += 55
                        } else {
                            heightDifference += 55
                            if (heightDifference > 0) {
                                tableview.contentY += heightDifference
                            }
                        }
                    }
                }
            }
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
