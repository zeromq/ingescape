import QtQuick 2.9

import MasticQuick 1.0


Item {
    id: root

    anchors.fill: parent


    // Behavior
    Component.onCompleted: {
        // Set agent name and definition info
        Mastic.agentName = "Simple-dragNDrop-agent"
        Mastic.definitionName = Mastic.agentName;
        Mastic.definitionDescription = "Definition of " + Mastic.agentName;
        Mastic.definitionVersion = "0.0";
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


        // Color is exposed as a Mastic input named 'rect_color'
        MasticInputBinding on color {
            inputName: "rect_color"
        }

        // X and Y are exposed as Mastic outputs named 'rect_x' and 'rect_x'
        MasticOutputBinding {
            properties: "x, y"
            outputsPrefix: "rect_"
        }
    }
}

