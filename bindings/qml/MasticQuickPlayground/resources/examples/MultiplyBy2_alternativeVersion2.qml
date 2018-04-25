import QtQuick 2.0
import MasticQuick 1.0


Item {
    id: root

    // Behavior
    Component.onCompleted: {
        // Set agent name and definition info
        Mastic.agentName = "MultiplyBy2-agent-alternativeVersion1"
        Mastic.definitionName = Mastic.agentName;
        Mastic.definitionDescription = "Definition of " + Mastic.agentName;
        Mastic.definitionVersion = "0.0";

        // Create an input
        Mastic.createInputDouble("operand", 0);

        // Create an output
        Mastic.createOutputDouble("result");
    }

    // Use a Connection to observe a specific input
    Connections {
        target: Mastic.inputs

        onOperandChanged: {
            Mastic.outputs.result = Mastic.inputs.operand * 2;
        }
    }

    // Feedback
    Text {
        anchors {
            fill: parent
            margins: 10
        }

        wrapMode: Text.WordWrap

        font {
            pixelSize: 16
        }

       text: Mastic.inputs.operand + " x 2 = " + Mastic.outputs.result
    }
}
