import QtQuick 2.9
import QtQuick.Controls 1.4

import IngeScapeQuick 1.0


Item {
    id: root

    anchors.fill: parent


    Component.onCompleted: {
        // Set agent name and definition info
        IngeScape.agentName = "MyQmlAgent"
        IngeScape.definitionName = IngeScape.agentName;
        IngeScape.definitionDescription = "Definition of " + IngeScape.agentName;
        IngeScape.definitionVersion = "0.0";

        // Create a new output
        IngeScape.createOutputImpulsion("onClicked")
    }


    //----------------
    // Content
    //----------------

    Button {
        id: myButton

        anchors.centerIn: parent

        text: qsTr("Send impulsion")

        onClicked: {
            IngeScape.writeOutputAsImpulsion("onClicked");
        }
    }
}
