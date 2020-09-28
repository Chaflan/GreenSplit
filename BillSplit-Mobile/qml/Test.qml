import QtQuick 2.0
import QtQuick.Controls 2.15

Item {
        width: 300
        height: 200

        Column {
            anchors.fill: parent
            Text {
                id: messageText
                height: parent.height - circleButton.height
                width: parent.width
                font.pointSize: 15
                anchors.horizontalCenter: parent.horizontalCenter
            }
            RoundButton {
                id: circleButton
                height: 80
                width: 100
                text: "OK"
                font.pointSize: 15
                anchors.horizontalCenter: parent.horizontalCenter
                onClicked: errorPopup.close()
            }
        }

}


