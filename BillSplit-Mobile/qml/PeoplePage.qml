import QtQuick 2.15
import QtQuick.Controls 2.5
import QtQml.Models 2.12
import Qt.labs.qmlmodels 1.0
import QtQuick.Layouts 1.3

// TODO: standardize use of table.model and peoplemodel
// TODO: standardize the use of parent vs id

Page {
    Item {
        id: entiretable
        anchors.fill: parent
        anchors.leftMargin: 5
        anchors.rightMargin: 5

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
                    width: tableview.model.columnWidth(index, tableviewheader.spacing, entiretable.width)
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
                return model.columnWidth(column, columnSpacing, entiretable.width);
            }

            anchors.fill: parent
            anchors.topMargin: tableviewheader.height + 5
            anchors.bottomMargin: tableviewfooter.height + 5
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
                    onFocusChanged: { if(focus) { selectAll() } } // Select all on click

                    onEditingFinished: {
                        // "model.edit = text" has problems
                        if (!tableview.model.setData(row, column, text)) {
                            text = display
                        }
                    }

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
            id: tableviewfooter
            width: tableview.width
            height: 40
            x: -tableview.contentX
            y: -tableview.contentY + tableview.contentHeight + tableviewheader.height + tableview.rowSpacing + 5

            Button {
                id: viewButton
                enabled: false
                text: "View"
                font.pointSize: 10
                Layout.preferredWidth: 100
                Layout.preferredHeight: tableviewheader.height
                Layout.leftMargin: 0

                onClicked: {
                    viewPersonDialog.initials = tableview.model.getData(tableview.selectedRow, "Identifier")
                    viewPersonDialog.name = tableview.model.getData(tableview.selectedRow, "Name")
                    viewPersonDialog.open()
                }
            }
            Item { Layout.fillWidth: true } // Spacer
            Button {
                id: addButton
                text: "Add"
                font.pointSize: 10
                Layout.preferredWidth: 100
                Layout.preferredHeight: tableviewheader.height
                Layout.rightMargin: 0
                onClicked: {
                    addPersonDialog.initials = ""
                    addPersonDialog.name = ""
                    addPersonDialog.open()
                }
            }
        }
    }


    PersonInputDialog {
        id: addPersonDialog
        anchors.centerIn: parent
        onSavePressed: {
            if (!tableview.model.addPerson(addPersonDialog.initials, addPersonDialog.name)) {
                console.warn("person couldn't be added. I=" + initials + " N=" + name)
            } else {
                tableview.forceLayout() // contentHeight won't update without this as it is loaded on demand
            }
        }
    }
    PersonInputDialog {
        id: viewPersonDialog
        anchors.centerIn: parent
        onSavePressed: {
            tableview.model.setData(tableview.selectedRow, "Identifier", viewPersonDialog.initials)
            tableview.model.setData(tableview.selectedRow, "Name", viewPersonDialog.name)
        }
        onDeletePressed: {
            tableview.model.removeRows(tableview.selectedRow, 1)
            tableview.forceLayout() // contentHeight won't update without this as it is loaded on demand
        }
    }

    // TODO: Make a file for this
    // Creat a popup when an error signal is sent from the model
    Connections {
        target: tableview.model
        function onSignalError(pErrorMessage) {
            errorPopup.errorMessage.text = pErrorMessage// + " paaaaaa aaaa aaa aa aaaaa aaa aadding "
            errorPopup.open()
        }
    }
    Popup {
        id: errorPopup
        modal: true
        focus: true
        anchors.centerIn: parent
        width: 300
        height: 200
        closePolicy: Popup.CloseOnEscape | Popup.CloseOnPressOutside

        property alias errorMessage: messageText

        Column {
            anchors.fill: parent
            Text {
                id: messageText
                height: parent.height - circleButton.height
                width: parent.width
                font.pointSize: 15
                anchors.horizontalCenter: parent.horizontalCenter
                wrapMode: Text.WordWrap
                verticalAlignment: Text.AlignVCenter
                horizontalAlignment: Text.AlignHCenter
            }
            RoundButton {
                id: circleButton

                height: 80
                width: 100
                anchors.horizontalCenter: parent.horizontalCenter

                text: "OK"
                font.pointSize: 15

                focus: true
                onClicked: { errorPopup.close() }
                Keys.onEnterPressed: { errorPopup.close() }
                Keys.onReturnPressed: { errorPopup.close() }
            }
        }
    }
}
