import QtQuick 2.9
import QtQuick.Controls 2.2

Page {
    width: 600
    height: 400

    title: qsTr("Page 1")

    Label {
        id: label
        text: qsTr("You are on Page 1.")
        anchors.centerIn: parent
    }
}
