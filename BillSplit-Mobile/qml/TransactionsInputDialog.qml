import QtQuick 2.6
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.0
import QtQuick.Dialogs 1.2
import QtQuick.Window 2.2
import "."

Dialog {
    id: root
    width: 400
    title: "Add Transaction"
    standardButtons: StandardButton.NoButton

    property alias payername: comboBoxPayer.currentText
    property alias cost: spinBoxCost.realValue
    property alias description: textFieldDescription.text

    // TODO: Bit of a hack no?  Seems like we could do better here
    // Maybe a model or a property for this sort of thing?
    function getNameList() {
        return peopleModel.getSelectedPeople(listView.checks)
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
                model: peopleModel.allPeople
            }
        }
        GroupBox {
            id: groupBoxCost
            Layout.fillWidth: true
            title: qsTr("How Much")

            // TODO: Break out into its own item DoubleSpinBox
            SpinBox {
                id: spinBoxCost
                from: 0
                value: 0
                to: 100 * 100
                stepSize: 100
                anchors.fill: parent
                editable: true

                property int decimals: 2
                property real realValue: value / 100

                validator: DoubleValidator {
                    bottom: Math.min(spinBoxCost.from, spinBoxCost.to)
                    top:  Math.max(spinBoxCost.from, spinBoxCost.to)
                }

                textFromValue: function(value, locale) {
                    return Number(value / 100).toLocaleString(locale, 'f', spinBoxCost.decimals)
                }

                valueFromText: function(text, locale) {
                    return Number.fromLocaleString(locale, text) * 100
                }
            }
        }
        GroupBox {
            id: groupBoxDescription
            Layout.fillWidth: true
            title: qsTr("For What (Optional Description)")

            TextField {
                id: textFieldDescription
                anchors.fill: parent
                placeholderText: qsTr("Text Field")
            }
        }
        GroupBox {
            id: groupBoxCovering
            height: 100
            Layout.fillWidth: true
            Layout.fillHeight: true
            title: qsTr("Covering Whom")

            // TODO: Grid view of some kind?
            ListView {
                id: listView
                anchors.fill: parent
                model: peopleModel.allPeople

                property bool defaultCheckState: true
                property var checks: []
                Component.onCompleted: {
                    for (var i = 0; i < peopleModel.rowCount(); i++) {
                        checks[i] = defaultCheckState
                    }
                }

                delegate: CheckDelegate {
                    text: modelData
                    checked: listView.defaultCheckState
                    onCheckStateChanged: {
                        listView.checks[index] = checked
                    }
                }
            }
        }

        RowLayout {
            id: row
            spacing: 50
            Layout.fillWidth: true

            Button {
                id: buttonDelete
                text: qsTr("Delete")
                Layout.alignment: Qt.AlignLeft
                onClicked: reject()
            }
            Item{ Layout.fillWidth: true }  // Spacer
            Button {
                id: buttonSave
                text: qsTr("Save")
                Layout.alignment: Qt.AlignHCenter
                onClicked: accept()
            }
            Item{ Layout.fillWidth: true }  // Spacer
            Button {
                id: buttonCancel
                text: qsTr("Cancel")
                Layout.alignment: Qt.AlignRight
                onClicked: close()
            }
        }
    }
}


