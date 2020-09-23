import QtQuick 2.15
import QtQuick.Controls 2.12
import QtQml.Models 2.12
import Qt.labs.qmlmodels 1.0

Page {
    width: 600
    height: 400

    header: Button {
        text: qsTr("Add Person")
        onClicked: {
            personInputDialog.open()
        }
    }



    // TODO: Headers (QT hasn't implemented these yet)

    // TODO: Resize columns based on data in them
    // TODO: Center the table, not smashed up into the upper right
    // TODO: Selectable rows

//    ItemSelectionModel {
//        id: ism
//        model: peopleModel
//        onSelectionChanged: console.log("selectionchanged")
//        onCurrentIndexChanged: console.log("currentindexchanged")
//        onCurrentColumnChanged: console.log("currentindexchanged")
//        onCurrentRowChanged: console.log("currentindexchanged")
////                onCurrentIndexChanged: console.log("currentindexchanged")
////                onCurrentIndexChanged: console.log("currentindexchanged")
////                onCurrentIndexChanged: console.log("currentindexchanged")
////                onCurrentIndexChanged: console.log("currentindexchanged")
//    }


    QtObject {
        id: theme
        property font headerFont: Qt.font({
//            family: 'Encode Sans',
//            weight: Font.Black,
//            italic: false,
            pointSize: 15
        })
    }

    Row {
        id: tableviewheader
        width: tableview.contentWidth
        height: 40
        x: -tableview.contentX
        z: 1
        spacing: 5

        Repeater {
            model: tableview.model.columnCount()
            Rectangle {
                width: tableview.model.columnWidth(index, Qt.font({ pointSize: 15 }))

                //width: 200
                height: parent.height
                color: "green"

                Text {
                    id: texttt
                    font: theme.headerFont
                    anchors.fill: parent
                    //anchors.verticalCenter: parent.VerticalCenter
                    x: 4
                    width: parent.width - 4
                    text: tableview.model.headerData(index, Qt.Horizontal)
                }
            }
        }
    }

    TableView {
        id: tableview
        model: peopleModel
        focus: true
        columnWidthProvider: function(column) { return Math.min(600, model.columnWidth(column, theme.headerFont)) }

        anchors.fill: parent
        anchors.topMargin: tableviewheader.height
        columnSpacing: 5
        rowSpacing: 5

//        delegate: DelegateChooser {
//            id: chooser
//            role: "type"
//            DelegateChoice { roleValue: "InitialsRole"; Text { text: display } }
//            DelegateChoice { roleValue: "NameRole"; TextField { text: display } }
//        }

        delegate: Rectangle {
            //implicitWidth: tableview.model.columnWidth(index)
            implicitHeight: 50
            border.color: "black"

//            Text {
//                id: text
//                text: model.display
//            }

            TextField {
                text: display
                font: theme.headerFont
                anchors.fill: parent
                width: parent.width

                onAccepted: {
                    console.log("column=" + column + " row=" + row + " colwidth=" + width)
                }
                //onDataChanged: console.log("datachanged")
            }
        }

//        MouseArea {
//            anchors.fill: parent
//            onClicked: {
//                tableview.forceLayout()
//            }
//        }
    }

    PersonInputDialog {
        id: personInputDialog
        onAccepted: {
            console.log("accepted")
            if (!peopleModel.addPerson(initials, name)) {
                console.warn("person couldn't be added. I=" + initials + " N=" + name)
            }
        }
        onRejected: {
            console.log("rejected")
        }
    }
}
