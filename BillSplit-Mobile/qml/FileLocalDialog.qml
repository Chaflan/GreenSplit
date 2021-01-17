import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12

//-----------------------------------------------------------------------
// Dialog for loading and saving local memory files
//-----------------------------------------------------------------------
Popup {
    y: origin.y + 10
    x: origin.x + 10
    width: mainApplicationWindow.width - 20
    modal: true

    function setOpenMode() {
        buttonAccept.text = "Open"
        textFieldFileName.visible = false
        resultLabel.visible = false
        privates.activeColor = privates.openColor
        privates.listTextColor = privates.openColor
    }
    function setSaveMode() {
        buttonAccept.text = "Save"
        textFieldFileName.visible = true
        resultLabel.visible = true
        privates.activeColor = privates.saveColor
        privates.listTextColor = "black"
    }

    property alias fileName: textFieldFileName.text

    signal acceptPressed    // User pressed save
    signal cancelPressed    // User pressed cancel
    signal closeNoPress     // Popup closed, but user did not press a button to do so

    QtObject {
        id: privates

        property int pressResult: 0
        property color activeColor
        property color listTextColor: "black"

        readonly property color saveColor: "darkred"
        readonly property color openColor: "darkgreen"
    }

    ColumnLayout {
        id: columnLayout
        anchors.fill: parent

        ListView {
            id: listView
            Layout.fillHeight: true
            Layout.fillWidth: true
            spacing: 5

            // QML does not automatically do this caulcation which throws off layout sizing
            implicitHeight: {
                var iheight = 0
                for (var i = 0; i < contentItem.children.length; i++) {
                    iheight += contentItem.children[i].implicitHeight
                }
                return iheight
            }

            delegate: TextField {
                id: listViewText
                text: modelData

                anchors.left: parent.left
                anchors.right: parent.right
                horizontalAlignment: TextInput.AlignHCenter
                font.pixelSize: 15
                readOnly: true
                color: privates.listTextColor

                background: Rectangle {
                    id: listViewTextBackground
                    border.color: "lightgray"
                    border.width: 1
                }

                onActiveFocusChanged: {
                    if (activeFocus) {
                        listViewTextBackground.border.color = "blue"
                        listViewTextBackground.border.width = 2
                        textFieldFileName.text = text
                    } else {
                        listViewTextBackground.border.color = "lightgray"
                        listViewTextBackground.border.width = 1
                    }
                }
            }
        }

        Item { implicitHeight: 40 } // Spacer

        Label {
            id: resultLabel
            text: "Resulting Save File (Can Be Edited):"
            Layout.fillWidth: true
            horizontalAlignment: TextInput.AlignHCenter
            font.pixelSize: 15
            color: privates.activeColor
        }

        TextField {
            id: textFieldFileName
            Layout.fillWidth: true
            horizontalAlignment: TextInput.AlignHCenter
            font.pixelSize: 15
            color: privates.activeColor
        }

        RowLayout {
            id: buttonRow
            Layout.fillWidth: true
            spacing: 5

            Layout.leftMargin: 5
            Button {
                id: buttonAccept
                text: qsTr("Accept")
                enabled: textFieldFileName.text !== ""
                contentItem: Text {
                    id: acceptContentItemText
                    text: buttonAccept.text
                    color: textFieldFileName.text === "" ? "gray" : privates.activeColor
                    font.pixelSize: 15
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                }
                onClicked: { privates.pressResult = 2; close() }
            }
            Item { Layout.fillWidth: true } // Spacer
            Button {
                id: buttonCancel
                text: qsTr("Cancel")
                contentItem: Text {
                    id: cancelContentItemText
                    text: buttonCancel.text
                    color: privates.activeColor
                    font.pixelSize: 15
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                }
                onClicked: { privates.pressResult = 3; close() }
            }
            Layout.rightMargin: 5
        }
    }

    onOpened: {
        listView.model = dataCoreInstance.getLocalSaveFiles()
        textFieldFileName.clear()
    }

    onClosed: {
        if (privates.pressResult === 2) {
            acceptPressed()
        } else if (privates.pressResult === 3) {
            cancelPressed()
        } else {
            closeNoPress()
        }
    }
}
