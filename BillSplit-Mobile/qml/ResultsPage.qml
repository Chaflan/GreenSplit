import QtQuick 2.15
import QtQuick.Controls 2.12
import QtQml.Models 2.12
import Qt.labs.qmlmodels 1.0
import QtQuick.Layouts 1.3

Page {
    Item {
        anchors.fill: parent

        ListView {
            id: listview
            model: resultsModel
            focus: true

            width: parent.width
            height: parent.height

            delegate: Text {
                text: display
            }

//            delegate: Rectangle {
//                border.color: "black"

//                TextField {
//                    id: textField
//                    text: display
//                    font.pixelSize: 15
//                    readOnly: true
//                }
//            }
        }
    }
}
