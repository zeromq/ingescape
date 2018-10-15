import QtQuick 2.9
import IngeScapeQuick 1.0


Item {
    id: root

    anchors {
        fill: parent
        margins: 20
    }


    // Properties
    property real joystickX: 0
    property real joystickY: 0

    // Expose our joystickX property as a IngeScape output named 'x'
    IngeScapeOutputBinding on joystickX {
        outputName: "x"
    }

    // Expose our joystickY property as a IngeScape output named 'y'
    IngeScapeOutputBinding on joystickY {
        outputName: "y"
    }


    // Behavior
    Component.onCompleted: {
        // Set agent name and definition info
        IngeScape.agentName = "QmlVirtualJoystick-agent"
        IngeScape.definitionName = IngeScape.agentName;
        IngeScape.definitionDescription = "Definition of " + IngeScape.agentName;
        IngeScape.definitionVersion = "0.0";
    }


    //----------------
    //
    // Content
    //
    //----------------

    ParallelAnimation {
        id: returnToCenterAnimation

        NumberAnimation {
            target: root;
            property: "joystickX";
            to: 0;
            easing.type: Easing.OutSine
        }

        NumberAnimation {
            target: root;
            property: "joystickY";
            to: 0;
            easing.type: Easing.OutSine
        }
    }


    Text {
        font {
            pixelSize: 12
            bold: true
        }

        text: qsTr("X = %1\nY = %2").arg(root.joystickX.toLocaleString(Qt.locale(), 'f', 2)).arg(root.joystickY.toLocaleString(Qt.locale(), 'f', 2))
    }


    Rectangle {
        id: background

        width: Math.min(parent.width, parent.height)
        height: width
        radius: width/2

        anchors.centerIn: parent

        color: "#D4D4D4"

        border {
            width: 2
            color: "#6A6A6A"
        }

        Rectangle {
            id: center

            width: background.width/5
            height: width
            radius: width/2

            anchors.centerIn: parent

            color: "transparent"

            border {
                width: 1
                color: "#6A6A6A"
            }
        }


        Rectangle {
            id: thumb

            width: center.width
            height: width
            radius: width/2

            anchors {
                centerIn: parent
                horizontalCenterOffset: root.joystickX * (background.width/2 - thumb.width/2)
                verticalCenterOffset: -root.joystickY * (background.width/2 - thumb.width/2)
            }

            color: "#990088CC"

            border {
                width: 2
                color: "black"
            }
        }

        MouseArea {
            id: joystickMouseArea

            anchors.fill: parent

            function computeJoystickPosition(mouseX, mouseY)
            {
                var maxDistance = Math.max((background.width/2 - thumb.width/2), 1);
                var dx = mouseX - width/2;
                var dy = mouseY - height/2;

                if ((dx * dx + dy * dy) < (maxDistance * maxDistance))
                {
                    // inside bounds
                    root.joystickX = dx/maxDistance;
                    root.joystickY = -dy/maxDistance;
                }
                else
                {
                    // outside bounds
                    var angle = Math.atan2(dy, dx);
                    root.joystickX = Math.cos(angle);
                    root.joystickY = -Math.sin(angle);
                }
            }

            onPressed: {
                returnToCenterAnimation.stop();

                computeJoystickPosition(mouseX, mouseY);
            }

            onReleased: {
                returnToCenterAnimation.restart();
            }

            onPositionChanged: {
                computeJoystickPosition(mouseX, mouseY);
            }


            // Export signals as outputs
            IngeScapeOutputBinding {
                target: joystickMouseArea

                signalHandlers: "onPressed, onReleased"

                outputsPrefix: "joystick_"
            }
        }
    }
}
