import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12
import com.company.core 1.0

ApplicationWindow {
    id: mainApplicationWindow
    visible: true
    width: 800
    height: 600
    title: qsTr("Tabs")

    Item {
        id: origin
        x: menuBarRow.x
        y: menuBarRow.y
    }

    header: RowLayout {
        id: menuBarRow
        height: 50

        Button {
            id: burgerButton
            icon.source: "../icons/ic_menu_grey600_36dp.png"
            implicitWidth: 50
            Layout.leftMargin: 5
            onClicked: menu.open()

            Menu {
                id: menu

                MenuItem {
                    text: "New"
                    onClicked: {
                        dataCoreInstance.clear();
                        saveMenuItem.enabled = false;
                        saveMenuItem.text = "Save"
                    }
                }
                MenuItem {
                    text: "Open (Local)..."
                    onClicked: {
                        fileLocalDialogOpen.open()
                    }
                }
                MenuItem {
                    id: saveMenuItem

                    enabled: false
                    property string activeFileName

                    text: "Save"
                    onClicked: {
                        dataCoreInstance.jsonWrite(activeFileName)
                    }
                }
                MenuItem {
                    text: "Save As (Local)..."
                    onClicked: {
                        fileLocalDialogSave.open()
                    }
                }
                MenuItem {
                    text: "Exit"
                    onClicked: {
                        Qt.quit()
                    }
                }
            }
        }

        Item { Layout.fillWidth: true } // Spacer

        Button {
            id: kebabButton
            icon.source: "../icons/ic_dots_vertical_grey600_36dp.png"
            implicitWidth: 50
            Layout.rightMargin: 5
            enabled: false
        }
    }

    FileLocalDialog {
        id: fileLocalDialogSave
        Component.onCompleted: setSaveMode()
        onAcceptPressed: {
            if (dataCoreInstance.jsonWrite(fileLocalDialogSave.fileName)) {
                saveMenuItem.enabled = true
                saveMenuItem.activeFileName = fileLocalDialogSave.fileName
                saveMenuItem.text = "Save (" + saveMenuItem.activeFileName + ")"
            }
        }
    }
    FileLocalDialog {
        id: fileLocalDialogOpen
        Component.onCompleted: setOpenMode()
        onAcceptPressed: {
            if (dataCoreInstance.jsonRead(fileLocalDialogOpen.fileName)) {
                saveMenuItem.enabled = true
                saveMenuItem.activeFileName = fileLocalDialogOpen.fileName
                saveMenuItem.text = "Save (" + saveMenuItem.activeFileName + ")"
            }
        }
    }

    SwipeView {
        id: swipeView
        anchors.fill: parent
        currentIndex: tabBar.currentIndex
        interactive: false

        // This bit of foolishness is necessary because I sometimes see pages showing up in
        // adjacent pages based on scroll status.
        onCurrentIndexChanged: {
            peoplepage.visible = false
            transactionspage.visible = false
            resultspage.visible = false

            switch (currentIndex) {
                case 0: peoplepage.visible = true; break
                case 1: transactionspage.visible = true; break
                case 2: resultspage.visible = true; break
            }
        }

        PeoplePage {
            id: peoplepage
        }
        TransactionsPage {
            id: transactionspage
        }
        ResultsPage {
            id: resultspage
        }
    }

    footer: TabBar {
        id: tabBar
        currentIndex: swipeView.currentIndex

        TabButton {
            text: qsTr("People")
        }
        TabButton {
            text: qsTr("Transactions")
        }
        TabButton {
            text: qsTr("Results")
        }
    }

    DataCore {
        id: dataCoreInstance
    }
    Connections {
        target: dataCoreInstance
        function onSignalError(errorMessage) { popupMessage(errorMessage) }
    }

    // Creat an error popup with the passed text.  Used with error signals from models.
    function popupMessage(messageText) {
        errorPopup.errorMessage.text = messageText// + " paaaaaa aaaa aaa aa aaaaa aaa aadding "
        errorPopup.open()
    }
    Popup {
        id: errorPopup
        modal: true
        focus: true
        anchors.centerIn: parent
        width: 300
        height: 200
        closePolicy: Popup.CloseOnEscape | Popup.CloseOnPressOutside

        property alias errorMessage: messageText

        Column {
            anchors.fill: parent
            Text {
                id: messageText
                height: parent.height - popupOkButton.height
                width: parent.width
                font.pointSize: 15
                anchors.horizontalCenter: parent.horizontalCenter
                wrapMode: Text.WordWrap
                verticalAlignment: Text.AlignVCenter
                horizontalAlignment: Text.AlignHCenter
            }
            Button {
                id: popupOkButton

                height: 80
                width: 100
                anchors.horizontalCenter: parent.horizontalCenter

                text: "OK"
                font.pointSize: 15

                focus: true
                onClicked: { errorPopup.close() }
                Keys.onEnterPressed: { errorPopup.close() }
                Keys.onReturnPressed: { errorPopup.close() }
            }
        }
    }
}
