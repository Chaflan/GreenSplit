import QtQuick 2.6
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.0
import QtQuick.Dialogs 1.2
import QtQuick.Window 2.2
import com.company.core 1.0
import "."

PopupDialog {
    id: root
    width: 400

    property alias transactionModel: transactionModel

    TransactionModel {
        id: transactionModel
        Component.onCompleted: initialize(dataCore)
    }

    ColumnLayout {
        id: column
        anchors.fill: parent
        spacing: 5

        GroupBox {
            id: groupBoxPayer
            Layout.fillWidth: true
            title: qsTr("Who Paid")

            ComboBox {
                id: comboBoxPayer
                anchors.fill: parent
                model: transactionModel.allPeople
                currentIndex: transactionModel.payerIndex
                onCurrentIndexChanged: transactionModel.payerIndex = currentIndex
            }
        }

        GroupBox {
            id: groupBoxCost
            Layout.fillWidth: true
            title: qsTr("How Much")

            TextField {
                id: spinBoxCost
                anchors.fill: parent
                onFocusChanged: { if(focus) { selectAll() } } // Select all on click
                text: transactionModel.cost
                onDisplayTextChanged: transactionModel.cost = text
            }
        }

        GroupBox {
            id: groupBoxDescription
            Layout.fillWidth: true
            title: qsTr("For What (Optional Description)")

            TextField {
                id: textFieldDescription
                anchors.fill: parent
                onFocusChanged: { if(focus) { selectAll() } } // Select all on click
                text: transactionModel.description
                onDisplayTextChanged: transactionModel.description = text
            }
        }

        GroupBox {
            id: groupBoxCovering
            height: 100
            Layout.fillWidth: true
            Layout.fillHeight: true
            title: qsTr("Covering Whom")

            GridView {
                id: listView
                anchors.fill: parent
                model: transactionModel.coveringList

                // TODO: Temp hack to be able to see contents
                // What you really need here is a scrollbar and some kind of default
                implicitHeight: 100

                delegate: CheckDelegate {
                    id: checkDelegate
                    text: modelData.name
                    font.pointSize: 9
                    checked: modelData.checkStatus
                    onCheckStateChanged: modelData.checkStatus = checked
                }
            }
        }
    }
}


