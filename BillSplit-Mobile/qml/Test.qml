import QtQuick 2.0
import QtQuick.Controls 2.15

Item {
    Column {
        id: row
        x: 0
        anchors.right: parent.right
        anchors.top: parent.top
        anchors.bottom: parent.bottom
        anchors.rightMargin: 0
        anchors.bottomMargin: 0
        anchors.topMargin: 0

        Rectangle {
            id: rectangle
            y: 73
            height: 110
            color: "#ffffff"
            anchors.left: parent.left
            anchors.right: parent.right

            Label {
                id: label
                text: qsTr("Label")
            }

            TextField {
                id: textField
                placeholderText: qsTr("Text Field")
            }

        }

        Rectangle {
            id: rectangle1
            x: 138
            y: 232
            width: 200
            height: 200
            color: "#ffffff"
            anchors.left: parent.left
            anchors.right: parent.right

            Label {
                id: label1
                text: qsTr("Name")
                font.pointSize: 14
            }

            TextField {
                id: textField1
                placeholderText: qsTr("Text Field")
            }

        }

        Rectangle {
            id: rectangle2
            x: 440
            y: 100
            width: 200
            height: 200
            color: "#ffffff"
            anchors.left: parent.left
            anchors.right: parent.right
        }
    }


}

/*##^##
Designer {
    D{i:0;autoSize:true;height:480;width:640}D{i:2}
}
##^##*/
