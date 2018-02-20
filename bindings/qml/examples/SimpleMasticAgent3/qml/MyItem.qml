import QtQuick 2.0


BorderImage {
    id: root


    //--------------------
    //
    // Content
    //
    //--------------------

    // Flag indicating if our animation is running
    property alias wheelRunning: wheelAnimation.running

    // Source (image) of our wheel
    property alias wheelSource: wheel.source




    source: "qrc:/qml/images/background.png"


    border { left: 270; top: 140; right: 250; bottom: 140 }



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

        anchors {
            horizontalCenter: pole.horizontalCenter
            bottom: pole.top
            bottomMargin: -height/2
        }

        source: "qrc:/qml/images/pinwheel.png"
    }

    NumberAnimation {
        id: wheelAnimation

        target: wheel
        property: "rotation"

        duration: 1000
        running: false

        loops: Animation.Infinite

        from: 0
        to: 360
    }
}



