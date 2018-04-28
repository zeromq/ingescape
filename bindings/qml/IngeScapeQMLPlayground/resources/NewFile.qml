import QtQuick 2.9
import QtQuick.Controls 1.4

import MasticQuick 1.0


Item {
    id: root

    anchors.fill: parent


    Component.onCompleted: {
        // Set agent name and definition info
        Mastic.agentName = "MyQmlAgent"
        Mastic.definitionName = Mastic.agentName;
        Mastic.definitionDescription = "Definition of " + Mastic.agentName;
        Mastic.definitionVersion = "0.0";

        // Create a new output
        Mastic.createOutputImpulsion("onClicked")
    }


    //----------------
    // Content
    //----------------

    Button {
        id: myButton

        anchors.centerIn: parent

        text: qsTr("Send impulsion")

        onClicked: {
            Mastic.writeOutputAsImpulsion("onClicked");
        }
    }
}
