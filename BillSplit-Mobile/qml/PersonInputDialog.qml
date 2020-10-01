import QtQuick 2.6
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.0
import QtQuick.Dialogs 1.2
import QtQuick.Window 2.2
import "."

Dialog {
    id: root
    width: 400
    title: "Add Person"
    standardButtons: StandardButton.NoButton

    property alias initials: textFieldInitials.text
    property alias name: textFieldName.text
    property alias buttonDelete: buttonDelete

    // TODO: Change the layout, its better
    Column {
        id: column
        anchors.fill: parent
        spacing: 20

        GroupBox {
            id: groupBox
            anchors.left: parent.left
            anchors.right: parent.right

            title: qsTr("Initials/ID (3 Char Max)")

            TextField {
                id: textFieldInitials
                anchors.fill: parent
                placeholderText: qsTr("J")
            }
        }

        GroupBox {
            id: groupBox1
            anchors.left: parent.left
            anchors.right: parent.right
            title: qsTr("Name (Optional)")

            TextField {
                id: textFieldName
                anchors.fill: parent
                placeholderText: qsTr("Jane Smith")
            }
        }

        Row {
            id: row
            spacing: 50

            Button {
                id: buttonDelete
                text: qsTr("Delete")
                onClicked: reject()
            }

            Button {
                id: buttonSave
                text: qsTr("Save")
                onClicked: accept()
            }

            Button {
                id: buttonCancel
                text: qsTr("Cancel")
                onClicked: close()
            }
        }
    }

}

//Dialog {

//    // See if you can remove this, i don't see a reason for it other to expose
//    // values if that is necessary
//    property alias initials: textInputInitials
//    property alias name: textInputName
//    title: "Initials"
    
//    standardButtons: StandardButton.Ok | StandardButton.Cancel | StandardButton.Delete
    
//    Rectangle {
//        id: rectangle
//        color: "#ffffff"
//        anchors.fill: parent
//    }
////    onVisibleChanged: {
////        //editTextItem.focus = true
////        //editTextItem.selectAll()
////    }
////    onButtonClicked: {
////        Qt.inputMethod.hide();
////    }

////    Rectangle {
////        id: initialsInputRect
////        implicitWidth: parent.width
////        implicitHeight: 100

////        //Label: qsTr("Initials:")

////        TextInput {
////            id: textInputInitials
////            inputMethodHints: Qt.ImhPreferUppercase
////            text: qsTr("kh")
////            color: Style.text
////        }
////    }

//    Rectangle {
//        id: nameInputRect
//        implicitWidth: parent.width
//        implicitHeight: 100

//        ColumnLayout {
//            Text {
//                id: labelItem
//                //text: label
//                //color: Style.text
//            }

//            TextInput {
//                id: textInputInitials
//                inputMethodHints: Qt.ImhPreferUppercase
//                text: qsTr("kh")
//                color: Style.text
//            }

//            TextInput {
//                id: textInputName
//                text: qsTr("aaaaaaaaaa")
//                color: Style.text
//            }
//        }
//    }
//}

/*##^##
Designer {
    D{i:0;autoSize:true;height:480;width:640}D{i:2}D{i:4}D{i:6}
}
##^##*/
