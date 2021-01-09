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
            width: tableview.contentWidth
            height: 40
            x: -tableview.contentX
            z: 1
            spacing: 5

            // QML is very painful sometimes.  Calling forceLayout doesnt work so we have to
            // do this very basic action of resizing column headers with cells manually.  It's unacceptable.
            function recalculateColumnWidths() {
                for (var i = 0; i < tableheaderrepeater.count; i++) {
                    tableheaderrepeater.itemAt(i).width = tableview.model.columnWidth(i, tableheader.spacing, table.width);
                }
            }

            Repeater {
                id: tableheaderrepeater
                model: tableview.model.columnCount()

                Rectangle {
                    width: tableview.columnWidthProvider(index)
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
            flickableDirection: Flickable.HorizontalAndVerticalFlick
            anchors.fill: parent
            anchors.topMargin: tableheader.height + 5
            anchors.bottomMargin: buttonbar.height + 10 + 10
            columnSpacing: 5
            rowSpacing: 5
            ScrollBar.vertical: ScrollBar { }
            columnWidthProvider: function(column) {
                return tableview.model.columnWidth(column, columnSpacing, table.width)
            }

            property var selectedRow: 0
            property var selectedColumn: 0

            // Enable the view button only when a table cell is selected
            onActiveFocusChanged: {
                viewButton.enabled = tableview.activeFocus || (viewButton.enabled && viewButton.activeFocus)
            }

            onWidthChanged: tableview.forceLayout() // Changes won't show otherwise

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

    Connections {
        target: tableview.model
        function onColumnWidthsChanged() {
            tableheader.recalculateColumnWidths()
        }
    }

    TransactionInputDialog {
        id: addTransactionDialog
        onSavePressed: {
            if (tableview.model.addFromModel(transactionModel)) {
                tableview.forceLayout() // needed for contentHeight and col widths
            }
        }
    }
    TransactionInputDialog {
        id: viewTransactionDialog
        onSavePressed: {
            if (tableview.model.editFromModel(tableview.selectedRow, transactionModel)) {
                tableview.forceLayout() // needed for col widths
            }
        }
        onDeletePressed: {
            if (tableview.model.removeRows(tableview.selectedRow, 1)) {
                tableview.forceLayout() // needed for contentHeight and col widths
            }
        }
        onClosed: {
            viewButton.enabled = false
        }
    }
}
