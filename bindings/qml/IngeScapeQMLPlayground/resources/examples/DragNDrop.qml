import QtQuick 2.9

import IngeScapeQuick 1.0


Item {
    id: root

    anchors.fill: parent


    // Behavior
    Component.onCompleted: {
        // Set agent name and definition info
        IngeScape.agentName = "Simple-dragNDrop-agent"
        IngeScape.definitionName = IngeScape.agentName;
        IngeScape.definitionDescription = "Definition of " + IngeScape.agentName;
        IngeScape.definitionVersion = "0.0";
    }


    //----------------
    //
    // Content
    //
    //----------------

    // Drag-n-droppable rectangle
    Rectangle {
        id: myRectangle

        width: 100
        height: width
        radius: 10

        color: "lightsteelblue"

        border.width: 1

        Text {
            anchors.fill: parent

            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter

            text: qsTr("Drag me\n\nX = %1\nY = %2").arg(myRectangle.x.toLocaleString(Qt.locale(), 'f', 2)).arg(myRectangle.y.toLocaleString(Qt.locale(), 'f', 2))

            color: "black"

            wrapMode: Text.WordWrap

            font {
                pixelSize: 14
            }
        }

        MouseArea {
            anchors.fill: parent
            drag.target: myRectangle
            smooth: false
        }


        // Color is exposed as a IngeScape input named 'rect_color'
        IngeScapeInputBinding on color {
            inputName: "rect_color"
        }

        // X and Y are exposed as IngeScape outputs named 'rect_x' and 'rect_x'
        IngeScapeOutputBinding {
            target: myRectangle
            properties: "x, y"
            outputsPrefix: "rect_"
        }
    }
}

