import QtQuick 2.12
import QtQuick.Controls 2.12

ApplicationWindow {
    id: mainApplicationWindow
    visible: true
    width: 800
    height: 600
    title: qsTr("Tabs")

    SwipeView {
        id: swipeView
        anchors.fill: parent
        currentIndex: tabBar.currentIndex

        PeoplePage {
        }

        TransactionsPage {
        }

        ResultsPage {
            onFocusChanged: {
                resultsModel.updateCalculations()
            }
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
}
