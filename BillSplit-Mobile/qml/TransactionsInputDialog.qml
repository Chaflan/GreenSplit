import QtQuick 2.6
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.0
import QtQuick.Dialogs 1.2
import QtQuick.Window 2.2
import "."

PopupDialog {
    id: root
    width: 400

    property alias payername: comboBoxPayer.currentText
    property alias cost: spinBoxCost.text
    property alias description: textFieldDescription.text

    // TODO: Hack.  Seems like we could do better here
    // Maybe a model or a property for this sort of thing?
    function getNameList() {
        return peopleModel.getSelectedPeople(listView.checks)
    }
    signal updatedChecks()
    function setNameList(nameList) {
        var selection = peopleModel.getSelectionFromPeople(nameList)
        if (selection.length !== peopleModel.rowCount()) {
            console.log("oh noooo")
        }
        for (var i = 0; i < peopleModel.rowCount(); i++) {
            listView.checks[i] = selection[i];
        }
        //printChecks()
        //listView.forceLayout()
        root.updatedChecks()
    }

    // TODO: Also hacky and janky
//    function getPayerId() {
//        console.log("currentValue " + comboBoxPayer.currentValue)
//        console.log("currentText " + comboBoxPayer.currentText)
//        console.log("payername: comboBoxPayer.currentText " + payername)
//        console.log("currentIndex " + comboBoxPayer.currentIndex)
//        return transactionsModel.getPidFromInitials(payername)
//    }
    function setPayerId(initialsString) {
        comboBoxPayer.currentIndex = 0;
        for (var i = 0; i < comboBoxPayer.model.length; i++) {
            if (comboBoxPayer.model[i] === initialsString) {
                comboBoxPayer.currentIndex = i;
                break;
            }
        }
    }

    function printChecks() {
        console.log("checks=")
        for (var i = 0; i < listView.checks.length; i++) {
            console.log("\t" + listView.checks[i])
        }
    }

    ColumnLayout {
        id: column
        anchors.fill: parent
        spacing: 5
        Component.onCompleted: console.log("column implicit height=" + column.implicitHeight)

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

            TextField {
                id: spinBoxCost
                anchors.fill: parent
                onFocusChanged: { if(focus) { selectAll() } } // Select all on click
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
                onFocusChanged: { if(focus) { selectAll() } } // Select all on click
            }
        }

        GroupBox {
            id: groupBoxCovering
            height: 100
            Layout.fillWidth: true
            Layout.fillHeight: true
            title: qsTr("Covering Whom")
            Component.onCompleted: console.log("groupBoxCovering implicit height=" + groupBoxCovering.implicitHeight)

            GridView {
                id: listView
                anchors.fill: parent
                model: peopleModel.allPeople

                // TODO: Temp hack to be able to see contents
                // What you really need here is a scrollbar and some kind of default
                implicitHeight: 100

                property bool defaultCheckState: true
                property var checks: []
                property bool checksCompleted: false
                Component.onCompleted: {
                    for (var i = 0; i < peopleModel.rowCount(); i++) {
                        checks[i] = defaultCheckState
                    }
                    checksCompleted = true
                    console.log("listView implicit height=" + listView.implicitHeight)
                    console.log("listView height=" + listView.implicitHeight)
                    printChecks()
                }

                delegate: CheckDelegate {
                    id: checkDelegate
                    text: modelData
                    font.pointSize: 9

                    onCheckStateChanged: {
                        listView.checks[index] = checked
                    }
                    Component.onCompleted: {
                        if (!listView.checksCompleted) {
                            checked = listView.defaultCheckState
                        } else {
                            checked = listView.checks[index]
                        }
                    }
                    Connections {
                        target: root
                        function onUpdatedChecks() {
                            checked = listView.checks[index]
                        }
                    }
                }
            }
        }
    }
}


