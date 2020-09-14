import QtQuick 2.6
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.0
import QtQuick.Dialogs 1.2
import QtQuick.Window 2.2
import "."

Dialog {

    // See if you can remove this, i don't see a reason for it other to expose
    // values if that is necessary
    property alias initials: textInputInitials
    property alias name: textInputName
    title: "Initials"
    
    standardButtons: StandardButton.Ok | StandardButton.Cancel
    
    Rectangle {
        id: rectangle
        color: "#ffffff"
        anchors.fill: parent
    }
//    onVisibleChanged: {
//        //editTextItem.focus = true
//        //editTextItem.selectAll()
//    }
//    onButtonClicked: {
//        Qt.inputMethod.hide();
//    }

//    Rectangle {
//        id: initialsInputRect
//        implicitWidth: parent.width
//        implicitHeight: 100

//        //Label: qsTr("Initials:")

//        TextInput {
//            id: textInputInitials
//            inputMethodHints: Qt.ImhPreferUppercase
//            text: qsTr("kh")
//            color: Style.text
//        }
//    }

    Rectangle {
        id: nameInputRect
        implicitWidth: parent.width
        implicitHeight: 100

        ColumnLayout {
            Text {
                id: labelItem
                //text: label
                //color: Style.text
            }

            TextInput {
                id: textInputInitials
                inputMethodHints: Qt.ImhPreferUppercase
                text: qsTr("kh")
                color: Style.text
            }

            TextInput {
                id: textInputName
                text: qsTr("aaaaaaaaaa")
                color: Style.text
            }
        }
    }
}
