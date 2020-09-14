import QtQuick 2.6
import QtQuick.Controls 2.5
import QtQuick.Controls 1.4 as OldControls

Page {
    width: 600
    height: 400

    header: Button {
        text: qsTr("Add Person")
        onClicked: {
            personInputDialog.open()
        }
    }

    OldControls.TableView {
        id: urMomsPussy
        model: peopleModel
        anchors.fill: parent

        // This might have somethign to do with the roles function that was mentioned in the book
        OldControls.TableViewColumn {title: "initials"; role: peopleModel ? "initials" : ""; width: 70 }
        OldControls.TableViewColumn {title: "name"; role: peopleModel ? "name" : "";  width: 70   }
    }

    PersonInputDialog {
        id: personInputDialog
        onAccepted: {
            peopleModel.addPerson(initials.text, name.text)
        }
    }
}
