import QtQuick 2.12
import QtQuick.Controls 2.12
import com.company.core 1.0

ApplicationWindow {
    id: mainApplicationWindow
    visible: true
    width: 800
    height: 600
    title: qsTr("Tabs")

    // View
    SwipeView {
        id: swipeView
        anchors.fill: parent
        currentIndex: tabBar.currentIndex

        PeoplePage {
        }

        TransactionsPage {
        }

//        ResultsPage {
//            onFocusChanged: {
//                resultsModel.updateCalculations()
//            }
//        }
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

    // TODO: Put these in the scopes in which they belong
    // Models
    DataCore {
        id: dataCore
    }
    PeopleTableModel {
        id: peopleTableModel
        data: dataCore
    }
    TransactionsTableModel {
        id: transactionsTableModel
        data: dataCore
    }

    // Creat a popup when an error signal is sent from the models
    Connections {
        target: dataCore
        function onSignalError(errorMessage) { popupMessage(errorMessage) }
    }
    Connections {
        target: peopleTableModel
        function onSignalError(errorMessage) { popupMessage(errorMessage) }
    }
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
                height: parent.height - circleButton.height
                width: parent.width
                font.pointSize: 15
                anchors.horizontalCenter: parent.horizontalCenter
                wrapMode: Text.WordWrap
                verticalAlignment: Text.AlignVCenter
                horizontalAlignment: Text.AlignHCenter
            }
            RoundButton {
                id: circleButton

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
