import QtQuick 2.6
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.0
import QtQuick.Dialogs 1.2
import QtQuick.Window 2.2
import "."

PopupDialog {
    property alias initials: textFieldInitials.text
    property alias name: textFieldName.text

    ColumnLayout {
        id: columnLayout
        anchors.fill: parent

        GroupBox {
            id: groupBox
            Layout.fillWidth: true
            title: qsTr("Initials/ID (3 Char Max)")

            TextField {
                id: textFieldInitials
                anchors.fill: parent
                placeholderText: qsTr("J")
                maximumLength: 3
            }
        }

        GroupBox {
            id: groupBox1
            Layout.fillWidth: true
            title: qsTr("Name (Optional)")

            TextField {
                id: textFieldName
                anchors.fill: parent
                placeholderText: qsTr("Jane Smith")
                maximumLength: 20
            }
        }
    }
}

/*##^##
Designer {
    D{i:0;autoSize:true;height:480;width:640}D{i:2}D{i:4}D{i:6}
}
##^##*/
