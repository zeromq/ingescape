import QtQuick 2.9

import IngeScapeQuick 1.0


Item {
    id: root

    anchors {
        fill: parent
        margins: 20
    }



    // Behavior
    Component.onCompleted: {
        // Set agent name and definition info
        IngeScape.agentName = "QmlClock-agent"
        IngeScape.definitionName = IngeScape.agentName;
        IngeScape.definitionDescription = "Definition of " + IngeScape.agentName;
        IngeScape.definitionVersion = "0.0";

        // Create IngeScape inputs
        IngeScape.createInputInt("hours", 10);
        IngeScape.createInputInt("minutes", 10);
        IngeScape.createInputInt("seconds", 30);
    }


    // Bind local properties to IngeScape inputs
    property int hours: IngeScape.inputs.hours
    property int minutes: IngeScape.inputs.minutes
    property int seconds: IngeScape.inputs.seconds


    //----------------
    //
    // Content
    //
    //----------------

    Rectangle {
        id: background

        width: Math.min(parent.width, parent.height)
        height: width
        radius: width/2

        anchors.centerIn: parent

        color: "#D4D4D4"

        Repeater {
            model: 60

            delegate: Item {
                id: tick

                property bool isBigTick: ((modelData % 5) === 0)

                width: Math.max(2, (isBigTick ? background.width/50 : background.width/100))

                anchors {
                    top: parent.top
                    bottom: parent.bottom
                    horizontalCenter: parent.horizontalCenter
                }

                rotation: modelData * 6

                Rectangle {
                    width: parent.width
                    height: width * 3
                    radius: width/2

                    color: (tick.isBigTick) ? "#7F007F" : "#007F7F"

                    smooth: true
                }
            }
        }


        // Hours
        Rectangle {
            anchors {
                bottom: parent.verticalCenter
                horizontalCenter: parent.horizontalCenter
            }

            width: Math.max(2, background.width/25)
            height: background.width/5
            radius: width/2

            color: "#7F007F"
            opacity: 0.8

            smooth: true

            transformOrigin: Item.Bottom

            rotation: 30.0 * (root.hours + (root.minutes/60.0))
        }


        // Minutes
        Rectangle {
            anchors {
                bottom: parent.verticalCenter
                horizontalCenter: parent.horizontalCenter
            }

            width: Math.max(2, background.width/50)
            height: background.width/3
            radius: width/2

            color: "#007F7F"
            opacity: 0.8

            smooth: true

            transformOrigin: Item.Bottom

            rotation: 6.0 * (root.minutes + (root.seconds/60.0))
        }


        // Seconds
        Rectangle {
            anchors {
                bottom: parent.verticalCenter
                horizontalCenter: parent.horizontalCenter
            }

            width: Math.max(2, background.width/100)
            height: background.width/2
            radius: width/2

            color: "orangered"

            smooth: true

            transformOrigin: Item.Bottom

            rotation: root.seconds * 6.0
        }


        Rectangle {
            anchors.centerIn: parent

            width: Math.max(2, background.width/20)
            height: width
            radius: width/2

            color: "black"
            smooth: true
        }
    }
}
