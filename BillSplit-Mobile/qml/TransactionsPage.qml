import QtQuick 2.15
import QtQuick.Controls 2.12
import QtQml.Models 2.12
import Qt.labs.qmlmodels 1.0

Page {
    width: 600
    height: 400

    header: Button {
        text: qsTr("Add Transaction")
        onClicked: {
            transactionsInputDialog.open()
        }
    }

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

        delegate: Rectangle {
            implicitHeight: 50
            border.color: "black"

            TextField {
                id: textField
                text: display
                font.pixelSize: 15
                anchors.fill: parent
                width: parent.width

                onAccepted: {
                    //console.log("accepted")
                    //console.log("column=" + column + " row=" + row + " colwidth=" + width)
                    model.edit = text
                }

                onActiveFocusChanged: {
                    //console.log(textField.text + " " + (textField.activeFocus ? "has focus" : "doesn't have focus"))
                    if (!textField.activeFocus) {
                        text = display
                    }
                }
            }
        }
    }

    TransactionsInputDialog {
        id: transactionsInputDialog
        onAccepted: {
            console.log("accepted. PayerName=" + payername + ". Cost=" + cost + ". Description=" + description)
//                bool addTransaction(const QString& payerName,
//                        double cost,
//                        const QString& description,
//                        const QStringList& coveringPids);
//            if (!tableview.model.addTransaction(initials, name)) {
//                console.warn("transaction couldn't be added. I=")// + initials + " N=" + name)
//            }
        }
        onRejected: {
            console.log("rejected")
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
