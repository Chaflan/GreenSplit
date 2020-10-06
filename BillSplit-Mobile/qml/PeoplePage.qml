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
//                            console.log("editingfinished")
                            //model.edit = text
//                            text = display
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
                    viewPersonDialog.open()
                    // TODO: Helper functions that just take row and string if you keep this (same with setdata)
//                    viewPersonDialog.initials = peopleModel.data(tableview.selectedRow, peopleModel.getColumnFromRole("Initials"))
//                    viewPersonDialog.name = peopleModel.data(tableview.selectedRow, peopleModel.getColumnFromRole("Name"))
//                    viewPersonDialog.open()
                }
            }
            RoundButton {
                id: addButton
                text: "Add\nPerson"
                font.pointSize: 10
                Layout.preferredWidth: parent.buttonDiameter
                Layout.preferredHeight: parent.buttonDiameter
                onClicked: {
                    addPersonDialog.open()
//                    addPersonDialog.initials = ""
//                    addPersonDialog.name = ""
//                    addPersonDialog.open()
                }
            }
        }
    }

    //PopupDialog {
    PersonInputDialog {
        id: addPersonDialog
        onSavePressed: {
            console.log("save pressed")
        }
        onDeletePressed: {
            console.log("delete pressed")
        }
        onCancelPressed: {
            console.log("cancel pressed")
        }
    }
    //PopupDialog {
    PersonInputDialog {
        id: viewPersonDialog
        onSavePressed: {
            console.log("save pressed")
        }
        onDeletePressed: {
            console.log("delete pressed")
        }
        onCancelPressed: {
            console.log("cancel pressed")
        }
    }

    // TODO: Explain this hack and why you need it
//    property var addPersonDialog
//    property var viewPersonDialog
//    Component.onCompleted: {
//        var componenttt = Qt.createComponent("PersonInputDialog.qml")
//        if(componenttt.status === Component.Ready) {
//            addPersonDialog = componenttt.createObject(mainApplicationWindow)
//            addPersonDialog.deleteButtonEnabled = false
//            addPersonDialog.onAccepted {
//                console.log("testing")
//            }

//            viewPersonDialog = componenttt.createObject(mainApplicationWindow)
//        } else {
//            console.error(componenttt.errorString())
//        }
//    }

//    PersonInputDialog {
//        id: viewPersonDialog
//        //parent: mainApplicationWindow
//        onAccepted: {
//            console.log("accepted view")
//            peopleModel.setData(tableview.selectedRow, peopleModel.getColumnFromRole("Initials"))
//            tableview.forceLayout()
//            close()
//        }
//        onRejected: {
//            // TODO: Can you do this?  Try to overwrite your selection system with this
//            //peopleModel.selection.select(tablemodel.currentRow)
//            //myXML.removeRow(tablemodel.currentRow)
//            //tablemodel.currentRow++;

//            console.log("rejected view. row=" + tableview.selectedRow)
//            peopleModel.removeRows(tableview.selectedRow, 1)
//            tableview.forceLayout()
//            close()
//        }
//    }

//    PersonInputDialog {
//        id: addPersonDialog
//        //parent: mainApplicationWindow
//        buttonDelete.enabled: false
//        onAccepted: {
//              //addPersonDialog.visible = false
//              //addPersonDialog.destroy()
//    //                        console.log("accepted add")
//    //                        if (!peopleModel.addPerson(addPersonDialog.initials, addPersonDialog.name)) {
//    //                            console.warn("person couldn't be added. I=" + initials + " N=" + name)
//    //                        }
//    //                        tableview.forceLayout()
//        }
//    }


//    PersonInputDialog {
//        id: viewPersonDialog
//        //parent: mainApplicationWindow
//        onAccepted: {
//            console.log("accepted view")
//            peopleModel.setData(tableview.selectedRow, peopleModel.getColumnFromRole("Initials"))
//            tableview.forceLayout()
//            close()
//        }
//        onRejected: {
//            // TODO: Can you do this?  Try to overwrite your selection system with this
//            //peopleModel.selection.select(tablemodel.currentRow)
//            //myXML.removeRow(tablemodel.currentRow)
//            //tablemodel.currentRow++;

//            console.log("rejected view. row=" + tableview.selectedRow)
//            peopleModel.removeRows(tableview.selectedRow, 1)
//            tableview.forceLayout()
//            close()
//        }
//    }

//    PersonInputDialog {
//        id: addPersonDialog
//        //parent: mainApplicationWindow
//        buttonDelete.enabled: false
//        onAccepted: {
//    //                        console.log("accepted add")
//    //                        if (!peopleModel.addPerson(addPersonDialog.initials, addPersonDialog.name)) {
//    //                            console.warn("person couldn't be added. I=" + initials + " N=" + name)
//    //                        }
//    //                        tableview.forceLayout()
//        }
//    }

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
                onClicked: {
                    errorPopup.close()
                    //tableview.forceLayout()
                }
                Keys.onEnterPressed:  {
                    errorPopup.close()
                    //tableview.forceLayout()
                }
                Keys.onReturnPressed: {
                    errorPopup.close()
                    //tableview.forceLayout()
                }
            }
        }
    }
}
