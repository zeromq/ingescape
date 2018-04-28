import QtQuick 2.0
import QtQuick.Extras 1.4
import QtQuick.Controls.Styles 1.4

import IngeScapeQuick 1.0

Rectangle {
    id: root

    anchors.fill: parent

    color: "black"

    // Behavior
    Component.onCompleted: {
        // Set agent name and definition info
        IngeScape.agentName = "Gauges-agent"
        IngeScape.definitionName = IngeScape.agentName;
        IngeScape.definitionDescription = "Definition of " + IngeScape.agentName;
        IngeScape.definitionVersion = "0.0";
    }


    // Circular gauges
    Item {
        id: circularGauges

        anchors {
            left: parent.left
            right: parent.horizontalCenter
            top: parent.top
            bottom: parent.bottom
            margins: 10
        }

        CircularGauge {
            id: circularGauge1

            anchors {
                top: parent.top
                left: parent.left
                right: parent.right
            }

            height: parent.height/2

            minimumValue: 0
            maximumValue: 100

            value: 0

            // Link our "value" property to a IngeScape input named 'absoluteCX'
            IngeScapeInputBinding on value {
                inputName: "absoluteCX"
            }
        }


        CircularGauge {
            anchors {
                left: parent.left
                right: parent.right
                top: circularGauge1.bottom
                bottom: parent.bottom
            }

            minimumValue: -1
            maximumValue: 1

            value: 0

            style: CircularGaugeStyle {
                tickmarkStepSize: 0.25
            }

            // Link our "value" property to a IngeScape input named 'relativeCX'
            IngeScapeInputBinding on value {
                inputName: "relativeCX"
            }
        }
    }


    // Vertical gauges
    Item {
        anchors {
            left: circularGauges.right
            right: parent.right
            top: parent.top
            bottom: parent.bottom
            margins: 10
        }

        Gauge {
            id: verticalGauge1

            anchors {
                top: parent.top
                bottom: parent.bottom
            }

            width: parent.width/2

            minimumValue: 0
            maximumValue: 100

            value: 0

            // Link our "value" property to a IngeScape input named 'absoluteX'
            IngeScapeInputBinding on value {
                inputName: "absoluteX"
            }
        }

        Gauge {
            anchors {
                top: parent.top
                bottom: parent.bottom
                left: verticalGauge1.right
                right: parent.right
            }

            minimumValue: -1
            maximumValue: 1
            tickmarkStepSize: 0.25

            value: 0

            // Link our "value" property to a IngeScape input named 'relativeX'
            IngeScapeInputBinding on value {
                inputName: "relativeX"
            }
        }
    }
}
