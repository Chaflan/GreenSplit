import QtQuick 2.15
import QtQuick.Controls 2.5
import QtQuick.Layouts 1.3

//-----------------------------------------------------------------------
// Custom Dialog
//-----------------------------------------------------------------------

Popup {
    modal: true

    default property alias content: contentItem.data
    property alias deleteEnabled: buttonDelete.enabled

    signal deletePressed    // User pressed delete
    signal savePressed      // User pressed save
    signal cancelPressed    // User pressed cancel
    signal closeNoPress     // Popup closed, but user did not press a button to do so

    QtObject {
        id: privateProperties
        property int pressResult: 0
    }

    ColumnLayout {
        id: columnLayout
        anchors.fill: parent
        spacing: 20

        Item {
            Component.onCompleted: console.log("contentItem.IH=" + contentItem.implicitHeight + " IW=" + contentItem.implicitBackgroundWidth)
            id: contentItem
            Layout.fillHeight: true
            Layout.fillWidth: true

//                implicitHeight: columnLayout2.implicitHeight
//                implicitWidth: columnLayout2.implicitWidth

//                property alias initials: textFieldInitials.text
//                property alias name: textFieldName.text

//                ColumnLayout {
//                    id: columnLayout2
//                    anchors.fill: parent
//            //        Layout.fillHeight: true
//            //        Layout.fillWidth: true

//                    GroupBox {
//                        id: groupBox
//                        Layout.fillWidth: true

//                        title: qsTr("Initials/ID (3 Char Max)")

//                        TextField {
//                            id: textFieldInitials
//                            anchors.fill: parent
//                            placeholderText: qsTr("J")
//                        }
//                    }

//                    GroupBox {
//                        id: groupBox1
//                        Layout.fillWidth: true

//                        title: qsTr("Name (Optional)")

//                        TextField {
//                            id: textFieldName
//                            anchors.fill: parent
//                            placeholderText: qsTr("Jane Smith")
//                        }
//                    }
//                }
        }

        RowLayout {
            id: buttonRow
            Layout.fillWidth: true
            spacing: 5

            Layout.leftMargin: 5
            Button {
                id: buttonDelete
                text: qsTr("Delete")
                onClicked: { privateProperties.pressResult = 1; close() }
            }
            Item { Layout.fillWidth: true } // Spacer
            Button {
                id: buttonSave
                text: qsTr("Save")
                onClicked: { privateProperties.pressResult = 2; close() }
            }
            Item { Layout.fillWidth: true } // Spacer
            Button {
                id: buttonCancel
                text: qsTr("Cancel")
                onClicked: { privateProperties.pressResult = 3; close() }
            }
            Layout.rightMargin: 5
        }
    }

    onClosed: {
        console.log("onClosed columnLayout.height" + columnLayout.height)

        if (privateProperties.pressResult == 1) {
            deletePressed()
        } else if (privateProperties.pressResult === 2) {
            savePressed()
        } else if (privateProperties.pressResult === 3) {
            cancelPressed()
        } else {
            closeNoPress()
        }
    }
}
