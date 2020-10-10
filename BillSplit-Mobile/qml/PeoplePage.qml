import QtQuick 2.15
import QtQuick.Controls 2.5
import QtQml.Models 2.12
import Qt.labs.qmlmodels 1.0
import QtQuick.Layouts 1.3

// TODO: standardize use of table.model and peoplemodel
// TODO: standardize the use of parent vs id

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
                model: peopleModel
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

                        onEditingFinished: {
                            // model.edit = text here does strange things and requires
                            // a tableview.forceLayout() call.  I call it directly instead
                            // This also allows use of the return variable
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
        }

        RowLayout {
            property var buttonDiameter: 80

            Layout.preferredHeight: buttonDiameter
            Layout.fillWidth: true
            Layout.bottomMargin: 20
            Layout.rightMargin: 20
            spacing: 50

            Item { Layout.fillWidth: true } // Spacer
            RoundButton {
                id: viewButton
                enabled: false
                text: "View\nSelected\nPerson"
                font.pointSize: 10
                Layout.preferredWidth: parent.buttonDiameter
                Layout.preferredHeight: parent.buttonDiameter
                onClicked: {
                    // TODO: Overload with row and string
                    viewPersonDialog.initials = peopleModel.data(tableview.selectedRow, peopleModel.getColumnFromRole("Initials"))
                    viewPersonDialog.name = peopleModel.data(tableview.selectedRow, peopleModel.getColumnFromRole("Name"))
                    viewPersonDialog.open()
                }
            }
            RoundButton {
                id: addButton
                text: "Add\nPerson"
                font.pointSize: 10
                Layout.preferredWidth: parent.buttonDiameter
                Layout.preferredHeight: parent.buttonDiameter
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
            console.log("add save")
            if (!peopleModel.addPerson(addPersonDialog.initials, addPersonDialog.name)) {
                console.warn("person couldn't be added. I=" + initials + " N=" + name)
            }
        }
    }
    PersonInputDialog {
        id: viewPersonDialog
        anchors.centerIn: parent
        onSavePressed: {
            console.log("view save")
            peopleModel.setData(tableview.selectedRow, peopleModel.getColumnFromRole("Initials"), viewPersonDialog.initials)
            peopleModel.setData(tableview.selectedRow, peopleModel.getColumnFromRole("Name"), viewPersonDialog.name)
        }
        onDeletePressed: {
            console.log("view delete")
            peopleModel.removeRows(tableview.selectedRow, 1)
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
