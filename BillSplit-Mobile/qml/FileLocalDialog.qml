// TODO: What is necessary here?  What is the .

import QtQuick 2.6
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.0
import QtQuick.Dialogs 1.2
import QtQuick.Window 2.2
import QtQuick.Controls.Styles 1.4
import "."

Popup {
    y: origin.y + 10
    x: origin.x + 10
    width: mainApplicationWindow.width - 20
    modal: true

    function setOpenMode() {
        buttonAccept.text = "Open"
        textFieldFileName.visible = false
        resultLabel.visible = false
        acceptContentItemText.color = "black"
        cancelContentItemText.color = "black"
    }
    function setSaveMode() {
        buttonAccept.text = "Save"
        textFieldFileName.visible = true
        resultLabel.visible = true
        acceptContentItemText.color = "darkgreen"
        cancelContentItemText.color = "darkgreen"
    }

    property alias fileName: textFieldFileName.text

    signal acceptPressed    // User pressed save
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

        ListView {
            id: listView
            Layout.fillHeight: true
            Layout.fillWidth: true
            spacing: 5

            // Bug fix: QML does not automatically do this caulcation which throws off layout sizing
            implicitHeight: {
                var iheight = 0
                for (var i = 0; i < contentItem.children.length; i++) {
                    iheight += contentItem.children[i].implicitHeight
                }
                return iheight
            }

            delegate: TextField {
                id: textField
                text: modelData

                anchors.left: parent.left
                anchors.right: parent.right
                horizontalAlignment: TextInput.AlignHCenter
                font.pixelSize: 15
                readOnly: true

                background: Rectangle {
                    id: backgroundRectangle
                    border.color: "black"
                    border.width: 1
                }

                onActiveFocusChanged: {
                    if (activeFocus) {
                        backgroundRectangle.border.color = "blue"
                        backgroundRectangle.border.width = 2
                        textFieldFileName.text = text
                    } else {
                        backgroundRectangle.border.color = "black"
                        backgroundRectangle.border.width = 1
                    }
                }
            }
        }

        Label {
            id: resultLabel
            text: "Resulting Save File (Can Be Edited):"
            Layout.fillWidth: true
            horizontalAlignment: TextInput.AlignHCenter
            font.pixelSize: 15
            color: "darkgreen"
        }

        TextField {
            id: textFieldFileName
            Layout.fillWidth: true
            horizontalAlignment: TextInput.AlignHCenter
            font.pixelSize: 15
            color: "darkgreen"
        }

        RowLayout {
            id: buttonRow
            Layout.fillWidth: true
            spacing: 5

            Layout.leftMargin: 5
            Button {
                id: buttonAccept
                text: qsTr("Accept")
                contentItem: Text {
                    id: acceptContentItemText
                    text: buttonAccept.text
                    font.pixelSize: 15
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                }
                onClicked: { privateProperties.pressResult = 2; close() }
            }
            Item { Layout.fillWidth: true } // Spacer
            Button {
                id: buttonCancel
                text: qsTr("Cancel")
                contentItem: Text {
                    id: cancelContentItemText
                    text: buttonCancel.text
                    font.pixelSize: 15
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                }
                onClicked: { privateProperties.pressResult = 3; close() }
            }
            Layout.rightMargin: 5
        }
    }

    onOpened: {
        listView.model = dataCore.getLocalSaveFiles()
    }

    onClosed: {
        if (privateProperties.pressResult === 2) {
            acceptPressed()
        } else if (privateProperties.pressResult === 3) {
            cancelPressed()
        } else {
            closeNoPress()
        }
    }
}
