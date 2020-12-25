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

        function getColumnWidth(columnIndex, columnSpacing, tableWidth) {
            switch (columnIndex) {
                case 0: return 80
                case 1: return 60
                case 2: return 95
                case 3: return tableWidth - columnSpacing * 3 - 235
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
                        id: texttt
                        font.pixelSize: 15
                        anchors.fill: parent
                        anchors.leftMargin: 10
                        verticalAlignment: Text.AlignVCenter
                        x: 4
                        width: parent.width - 4
                        text: tableview.model.headerData(index, Qt.Horizontal)
                    }
                }
            }
        }

        TableView {
            id: tableview
            model: transactionsTableModel
            focus: true

            columnWidthProvider: function(column) {
                return table.getColumnWidth(column, columnSpacing, table.width)
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
                    tableview.model.loadToModel(tableview.selectedRow, viewTransactionDialog.transactionModel)
                    viewTransactionDialog.open()
                }
            }
            Item { Layout.fillWidth: true } // Spacer
            Button {
                id: addButton
                text: "Add"
                font.pointSize: 10
                Layout.preferredWidth: 100
                Layout.preferredHeight: tableheader.height
                Layout.rightMargin: 0

                onClicked: {
                    if (dataCore.numPeople() < 1) {
                        popupMessage("Add at least one person to begin adding transactions")
                    } else {
                        addTransactionDialog.transactionModel.loadDefault()
                        addTransactionDialog.open()
                    }
                }
            }
        }
    }

    // TODO: rename, singular
    TransactionsInputDialog {
        id: addTransactionDialog
        onSavePressed: {
            if (!tableview.model.addFromModel(transactionModel)) {
                console.warn("transaction couldn't be added.")
            } else {
                tableview.forceLayout() // contentHeight won't update without this as it is loaded on demand
            }
        }
    }
    TransactionsInputDialog {
        id: viewTransactionDialog
        onSavePressed: {
            if (!tableview.model.editFromModel(tableview.selectedRow, transactionModel)) {
                console.warn("transaction couldn't be edited.")
            }
        }
        onDeletePressed: {
            if (!tableview.model.removeRows(tableview.selectedRow, 1)) {
                console.warn("transaction couldn't be deleted")
            } else {
                tableview.forceLayout() // contentHeight won't update without this as it is loaded on demand
            }
        }
        onClosed: {
            viewButton.enabled = false
        }
    }
}
