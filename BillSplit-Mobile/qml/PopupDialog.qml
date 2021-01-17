import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12

//-----------------------------------------------------------------------
// Custom Dialog
//
// Bug fix: QML has a bug for me where it will not load dialogs on android.
// A missing file [put bug link and error msg in here] causes the loading to
// freeze the app.  To solve the problem I customized a popup.
//-----------------------------------------------------------------------
Popup {
    y: origin.y + 10
    x: origin.x + 10
    width: mainApplicationWindow.width - 20
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
            id: contentItem
            Layout.fillHeight: true
            Layout.fillWidth: true

            // QML does not automatically do this caulcation which throws off layout sizing
            implicitHeight: {
                var iheight = 0
                for (var i = 0; i < contentItem.children.length; i++) {
                    iheight += contentItem.children[i].implicitHeight
                }
                return iheight
            }
            implicitWidth: {
                var iwidth = 0
                for (var i = 0; i < contentItem.children.length; i++) {
                    iwidth += contentItem.children[i].implicitWidth
                }
                return iwidth
            }
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
