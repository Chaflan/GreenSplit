import QtQuick 2.12
import QtQuick.Controls 2.12

ApplicationWindow {
    id: mainApplicationWindow
    visible: true
    width: 640
    height: 480
    title: qsTr("Tabs")

//    onActiveChanged: {
//        //if (Qt.platform.os === "windows") {
//            if (active) {
//                // ...
//                if (aboutDialog.visible) {
//                    aboutDialog.requestActivate()
//                }
//                // ...
//            }
//        //}
//    }

    SwipeView {
        id: swipeView
        anchors.fill: parent
        currentIndex: tabBar.currentIndex

        PeoplePage {
        }

        TransactionsPage {
        }

        ResultsPage {
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
