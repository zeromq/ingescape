import QtQuick 2.0


BorderImage {
    id: root


    //--------------------
    //
    // Content
    //
    //--------------------

    // Flag indicating if our animation is running
    property bool wheelRunning: false

    // Source (image) of our wheel
    property alias wheelSource: wheel.source




    source: "qrc:/qml/images/background.png"


    border { left: 270; top: 140; right: 250; bottom: 140 }



    //--------------------
    //
    // Behavior
    //
    //--------------------

    onWheelRunningChanged: {
        wheelAnimation.running = wheelRunning;
    }


    //--------------------
    //
    // Content
    //
    //--------------------


    Image {
        id: pole

        anchors {
            horizontalCenter: parent.horizontalCenter
            bottom: parent.bottom
        }

        source: "qrc:/qml/images/pole.png"
    }

    Image {
        id: wheel
rotation: 10
        anchors {
            horizontalCenter: pole.horizontalCenter
            bottom: pole.top
            bottomMargin: -height/2
        }

        source: "qrc:/qml/images/pinwheel.png"
    }



    SequentialAnimation {
        id: wheelAnimation

        running: false

        loops: Animation.Infinite

        NumberAnimation {
            target: wheel
            property: "rotation"

            duration: 1000.0 * (to - from)/360.0

            from: wheel.rotation
            to: 360
        }

        ScriptAction {
            script: {
                var isRunning = wheelAnimation.running;

                wheel.rotation = 0;
                wheelAnimation.stop();
                if (isRunning)
                {
                    wheelAnimation.start();
                }
            }
        }
    }
}



