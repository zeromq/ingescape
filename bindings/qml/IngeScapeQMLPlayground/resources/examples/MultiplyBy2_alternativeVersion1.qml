import QtQuick 2.0
import IngeScapeQuick 1.0


Item {
    id: root

    // Behavior
    Component.onCompleted: {
        // Set agent name and definition info
        IngeScape.agentName = "MultiplyBy2-agent-alternativeVersion1"
        IngeScape.definitionName = IngeScape.agentName;
        IngeScape.definitionDescription = "Definition of " + IngeScape.agentName;
        IngeScape.definitionVersion = "0.0";

        // Create an input
        IngeScape.createInputDouble("operand", 0);

        // Create an output
        IngeScape.createOutputDouble("result");
    }

    // Use a Connection to observe a specific input
    Connections {
        target: IngeScape.inputs

        onOperandChanged: {
            IngeScape.outputs.result = IngeScape.inputs.operand * 2;
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

       text: IngeScape.inputs.operand + " x 2 = " + IngeScape.outputs.result
    }
}
