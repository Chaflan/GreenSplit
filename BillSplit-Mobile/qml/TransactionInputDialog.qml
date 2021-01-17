import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12
import com.company.core 1.0

PopupDialog {
    height: 500

    property alias transactionModel: transactionModel

    TransactionModel {
        id: transactionModel
        dataCore: dataCoreInstance
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
                inputMethodHints: Qt.ImhDigitsOnly
                onFocusChanged: { if(focus) { selectAll() } } // Select all on click
                text: transactionModel.costStr
                onDisplayTextChanged: transactionModel.costStr = text
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
                maximumLength: 120
                onDisplayTextChanged: transactionModel.description = text
            }
        }

        GroupBox {
            id: groupBoxCovering
            Layout.fillWidth: true
            Layout.fillHeight: true
            title: qsTr("Covering Whom")

            GridView {
                id: listView
                anchors.fill: parent
                model: transactionModel.coveringList
                clip: true
                cellWidth: 90
                cellHeight: 50

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


