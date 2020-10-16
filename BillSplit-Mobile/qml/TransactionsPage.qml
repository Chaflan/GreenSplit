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
                model: transactionsModel
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
                text: "View\nSelected\nTransaction"
                font.pointSize: 10
                Layout.preferredWidth: parent.buttonDiameter
                Layout.preferredHeight: parent.buttonDiameter
                onClicked: {
                    transactionsModel.loadToModel(tableview.selectedRow, viewTransactionDialog.transactionModel)
                    //transactionsModel.loadToModel(tableview.selectedRow, transactionModel)
                    viewTransactionDialog.open()
                }
            }
            RoundButton {
                id: addButton
                text: "Add\nTransaction"
                font.pointSize: 10
                Layout.preferredWidth: parent.buttonDiameter
                Layout.preferredHeight: parent.buttonDiameter
                onClicked: {
                    viewTransactionDialog.transactionModel.clear()
                    //transactionModel.clear()
                    addTransactionDialog.open()
                }
            }
        }
    }

    // TODO: rename, singular
    TransactionsInputDialog {
        id: addTransactionDialog
        anchors.centerIn: parent
        onSavePressed: {
            if (!tableview.model.addFromModel(transactionModel)) {
                console.warn("transaction couldn't be added.")
            }
        }
    }
    TransactionsInputDialog {
        id: viewTransactionDialog
        anchors.centerIn: parent
        onSavePressed: {
            console.log("view save")
            if (!tableview.model.editFromModel(tableview.selectedRow, transactionModel)) {
                console.warn("transaction couldn't be edited.")
            }

        }
        onDeletePressed: {
            console.log("view delete")
            if (!tableview.model.removeRows(tableview.selectedRow, 1)) {
                console.warn("transaction couldn't be deleted")
            }
        }
    }



    // Creat a popup when an error signal is sent from the model
//    Connections {
//        target: tableview.model
//        function onSignalError(pErrorMessage) {
//            errorPopup.errorMessage.text = pErrorMessage// + " paaaaaa aaaa aaa aa aaaaa aaa aadding "
//            errorPopup.open()
//        }
//    }
//    Popup {
//        id: errorPopup
//        modal: true
//        focus: true
//        anchors.centerIn: parent
//        width: 300
//        height: 200
//        closePolicy: Popup.CloseOnEscape | Popup.CloseOnPressOutside

//        property alias errorMessage: messageText

//        Column {
//            anchors.fill: parent
//            Text {
//                id: messageText
//                height: parent.height - circleButton.height
//                width: parent.width
//                font.pointSize: 15
//                anchors.horizontalCenter: parent.horizontalCenter
//                wrapMode: Text.WordWrap
//                verticalAlignment: Text.AlignVCenter
//                horizontalAlignment: Text.AlignHCenter
//            }
//            RoundButton {
//                id: circleButton

//                height: 80
//                width: 100
//                anchors.horizontalCenter: parent.horizontalCenter

//                text: "OK"
//                font.pointSize: 15

//                focus: true
//                onClicked: errorPopup.close()
//                Keys.onEnterPressed:  errorPopup.close()
//                Keys.onReturnPressed: errorPopup.close()
//            }
//        }
//    }
}
