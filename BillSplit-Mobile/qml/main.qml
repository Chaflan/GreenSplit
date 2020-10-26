import QtQuick 2.12
import QtQuick.Controls 2.12
import com.company.core 1.0

ApplicationWindow {
    id: mainApplicationWindow
    visible: true
    width: 800
    height: 600
    title: qsTr("Tabs")

    //onWidthChanged: console.log("titties")

    // TODO: Private?
    DataCore {
        id: dataCore
    }

    PeopleTableModel {
        id: peopleTableModel
        data: dataCore
    }

    SwipeView {
        id: swipeView
        anchors.fill: parent
        currentIndex: tabBar.currentIndex

        PeoplePage {
        }

//        TransactionsPage {
//        }

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
}
