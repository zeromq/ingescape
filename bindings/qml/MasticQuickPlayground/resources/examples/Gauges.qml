import QtQuick 2.0
import QtQuick.Extras 1.4
import QtQuick.Controls.Styles 1.4

import MasticQuick 1.0

Rectangle {
    id: root

    anchors.fill: parent

    color: "black"

    // Behavior
    Component.onCompleted: {
        // Set agent name and definition info
        Mastic.agentName = "Gauges-agent"
        Mastic.definitionName = Mastic.agentName;
        Mastic.definitionDescription = "Definition of " + Mastic.agentName;
        Mastic.definitionVersion = "0.0";
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

            // Link our "value" property to a Mastic input named 'absoluteCX'
            MasticInputBinding on value {
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

            style: CircularGaugeStyle {
                tickmarkStepSize: 0.25
            }

            // Link our "value" property to a Mastic input named 'relativeCX'
            MasticInputBinding on value {
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

            // Link our "value" property to a Mastic input named 'absoluteX'
            MasticInputBinding on value {
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

            // Link our "value" property to a Mastic input named 'relativeX'
            MasticInputBinding on value {
                inputName: "relativeX"
            }
        }
    }
}
