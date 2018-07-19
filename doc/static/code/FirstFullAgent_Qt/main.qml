import QtQuick 2.9
import QtQuick.Window 2.2
import QtQuick.Controls 2.2

import FIRSTFULLAGENT 1.0

Window {
    visible: true
    width: 640
    height: 480
    title: qsTr("Hello World")

    Item {
        id: mainItem
        anchors.fill: parent

        Rectangle
        {
            id:inputContainer
            width: childrenRect.width
            height:childrenRect.height
            anchors.centerIn: parent
            border.color: "#ddd"


            TextEdit {
                id:input
                width: 300
                padding: 5
                wrapMode: TextEdit.Wrap
                selectByKeyboard: true
                selectByMouse: true

                property string placeholderText: "Enter your command..."

                Text {
                    text: input.placeholderText
                    color: "#aaa"
                    visible: !input.text
                    padding: 5
                }
            }
        }

        Button
        {
            anchors
            {
                top:inputContainer.bottom
                horizontalCenter:inputContainer.horizontalCenter
                topMargin:10
            }

            text: "Send command"
            onClicked: {
                MainC.sendMessage(input.text);
            }
        }

    }
}
