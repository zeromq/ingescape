/*
 *	MASTIC Editor
 *
 *  Copyright © 2017 Ingenuity i/o. All rights reserved.
 *
 *	See license terms for the rights and conditions
 *	defined by copyright holders.
 *
 *
 *	Contributors:
 *      Alexandre Lemort   <lemort@ingenuity.io>
 *
 */

import QtQuick 2.8
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4

import I2Quick 1.0

import MASTIC 1.0



Item {
    id: rootItem

    anchors.fill: parent


    //--------------------------------
    //
    // Properties
    //
    //--------------------------------

    // Controller associated to our view
    property var controller : null;


    // Minimum scale factor
    readonly property real minimumScale: 0.25

    // Maximum scale factor
    readonly property real maximumScale: 4


    //--------------------------------
    //
    // Functions
    //
    //--------------------------------




    //--------------------------------
    //
    // Content
    //
    //--------------------------------

    Item {
        id: content

        anchors.fill: parent



        //
        // Seamless background (bitmap version)
        //
        // Pros:
        //  - work with any tile pattern
        //  - lightweight (mostly GPU)
        //
        // Cons:
        //  - hard to perform a good looking zoom (scaling artifacts)
        //
        /*
        Image {
            id: seamlessBackgroundBitmap

            width: content.width + sourceSize.width
            height: content.height + sourceSize.height

            horizontalAlignment: Image.AlignLeft
            verticalAlignment: Image.AlignTop
            fillMode: Image.Tile

            // Source must be a tileable pattern
            source: "qrc:/resources/images/agentsmapping-background-tile.jpg"

            x: ((workspace.x % sourceSize.width) + sourceSize.width) % sourceSize.width - sourceSize.width
            y: ((workspace.y % sourceSize.height) + sourceSize.height) % sourceSize.height - sourceSize.height
        }
        */


        //
        // Seamless background (vector version)
        //
        // Pros:
        //  - pan and zoom
        //
        // Cons:
        //  - grid pattern only (ah-hoc look)
        //  - heavier processing load (scale only because items are created/deleted)
        //
        Rectangle {
            id: seamlessBackgroundVector

            anchors.fill: parent

            color: MasticTheme.agentsMappingBackgroundColor

            SeamlessGrid {
                anchors.fill: parent

                offsetX: workspace.x
                offsetY: workspace.y

                cellSize: 150 * workspace.scale
                numberOfSubDivisions: 5

                cellStroke: MasticTheme.agentsMappingGridLineColor
                subCellStroke: MasticTheme.agentsMappingGridSublineColor
            }
        }




        //
        // Workspace background interaction: pan & zoom
        //
        PinchArea {
            anchors.fill: parent

            pinch {
                target: workspace

                minimumScale: rootItem.minimumScale
                maximumScale: rootItem.maximumScale
            }

            MouseArea {
                anchors.fill: parent

                drag.target: workspace

                scrollGestureEnabled: false

                onWheel: {
                    wheel.accepted = true;

                    //TODO: zoom at (x, y)
                    if (wheel.angleDelta.y >= 0)
                    {
                        workspace.scale = Math.max(rootItem.minimumScale, workspace.scale/1.2);
                    }
                    else
                    {
                        workspace.scale = Math.min(rootItem.maximumScale, workspace.scale * 1.2);
                    }
                }
            }



            //-----------------------------------------------
            //
            // Workspace: nodes and links will be added here
            //
            //-----------------------------------------------
            Item {
                id: workspace

                transformOrigin: Item.TopLeft

                width: parent.width
                height: parent.height





                //------------------------------------------------
                //
                // Pseudo content to test pan & zoom, DnD, links
                //
                //------------------------------------------------


                // Maximum Z-index
                property int maxZ: 0


                // Slots
                property color inletColor: "#0CB8FF"
                property color outletColor: "#ff9933"

                // Node
                property color nodeColor: "#17191F"
                property color nodeCollapsedColor: "darkkhaki"
                property color nodeBorderColor: "#939CAA"
                property color nodeSelectedBorderColor: "#ffffff"


                //----------------------------------------
                // TODO: replace with a Repeater
                //       model: list of LinkVM

                //
                // Link between [item1, nodeOut] and [item3, nodein1]
                //
                Link {
                    id: link1

                    firstPoint: Qt.point(item1.x + item1NodeOut.x + item1NodeOut.width/2, item1.y + item1NodeOut.y + item1NodeOut.height/2)

                    secondPoint: Qt.point(item3.x + item3NodeIn1.x + item3NodeIn1.width/2, item3.y + item3NodeIn1.y + item3NodeIn1.height/2)

                    onClicked: console.log("Click on link1")
                }


                //
                // Link between [item2, nodeOut] and [item3, nodein2]
                //
                Link {
                    id: link2

                    firstPoint: Qt.point(item2.x + item2NodeOut.x + item2NodeOut.width/2, item2.y + item2NodeOut.y + item2NodeOut.height/2)

                    secondPoint: Qt.point(item3.x + item3NodeIn2.x + item3NodeIn2.width/2, item3.y + item3NodeIn2.y + item3NodeIn2.height/2)

                    onClicked: console.log("Click on link2")
               }


                //
                // Link between [item2, nodeOut] and [item4, nodeIn]
                //
                Link {
                    id: link3

                    firstPoint: Qt.point(item2.x + item2NodeOut.x + item2NodeOut.width/2, item2.y + item2NodeOut.y + item2NodeOut.height/2)

                    secondPoint: Qt.point(item4.x + item4NodeIn.x + item4NodeIn.width/2, item4.y + item4NodeIn.y + item4NodeIn.height/2)

                     onClicked: console.log("Click on link3")
                }


                //
                // Link between [item3, nodeOut1] and [item5, nodeIn1]
                //
                Link {
                    id: link4

                    firstPoint: Qt.point(item3.x + item3NodeOut1.x + item3NodeOut1.width/2, item3.y + item3NodeOut1.y + item3NodeOut1.height/2)

                    secondPoint: Qt.point(item5.x + item5NodeIn1.x + item5NodeIn1.width/2, item5.y + item5NodeIn1.y + item5NodeIn1.height/2)

                     onClicked: console.log("Click on link4")
                }


                //
                // Link between [item3, nodeOut2] and [item5, nodeIn2]
                //
                Link {
                    id: link5

                    firstPoint: Qt.point(item3.x + item3NodeOut2.x + item3NodeOut2.width/2, item3.y + item3NodeOut2.y + item3NodeOut2.height/2)

                    secondPoint: Qt.point(item5.x + item5NodeIn2.x + item5NodeIn2.width/2, item5.y + item5NodeIn2.y + item5NodeIn2.height/2)

                     onClicked: console.log("Click on link5")
                }


                //
                // Link between [item4, nodeOut] and [item5, nodeIn2]
                //
                Link {
                    id: link6

                    firstPoint: Qt.point(item4.x + item4NodeOut.x + item4NodeOut.width/2, item4.y + item4NodeOut.y + item4NodeOut.height/2)

                    secondPoint: Qt.point(item5.x + item5NodeIn2.x + item5NodeIn2.width/2, item5.y + item5NodeIn2.y + item5NodeIn2.height/2)

                     onClicked: console.log("Click on link6")
               }

                //----------------------------------------


                //----------------------------------------
                // TODO: replace with a Repeater
                //       model: list of AgentVM

                Repeater {
                    model : controller? controller.agentInMappingVMList : 0;


                    AgentNodeView {
                        id: agent
                        agentVM : model.QtObject

                        MouseArea {
                            id: mouseArea

                            anchors.fill: parent

                            drag.target: parent

                            onPressed: {
                                parent.z = workspace.maxZ++;
                            }
                        }
                    }
                }


                //
                // Item 1
                //
                Rectangle {
                    id: item1

                    x: 50
                    y: 50

                    width: 150
                    height: 70

                    color: workspace.nodeColor

                    radius: 8

                    border {
                        width: 2
                        color: (mouseArea1.pressed) ? workspace.nodeSelectedBorderColor : workspace.nodeBorderColor
                    }

                    Rectangle {
                        id: item1NodeOut

                        anchors {
                            horizontalCenter: parent.right
                            verticalCenter: parent.verticalCenter
                        }

                        width: 20
                        height: width
                        radius: width/2

                        border {
                            color: workspace.outletColor
                            width: 2
                        }

                        color: workspace.nodeColor

                        Rectangle {
                            anchors.centerIn: parent

                            width: parent.width - 8
                            height: width
                            radius: width/2

                            color: workspace.outletColor
                        }
                    }


                    MouseArea {
                        id: mouseArea1

                        anchors.fill: parent

                        drag.target: parent

                        onPressed: {
                            parent.z = workspace.maxZ++;
                        }
                    }
                }


                //
                // Item 2
                //
                Rectangle {
                    id: item2

                    x: 50
                    y: 300

                    width: 150
                    height: 70

                    color: workspace.nodeColor

                    radius: 8

                    border {
                        width: 2
                        color: mouseArea2.pressed ? workspace.nodeSelectedBorderColor : workspace.nodeBorderColor
                    }

                    Rectangle {
                        id: item2NodeOut

                        anchors {
                            horizontalCenter: parent.right
                            verticalCenter: parent.verticalCenter
                        }

                        width: 20
                        height: width
                        radius: width/2

                        border {
                            color: workspace.outletColor
                            width: 2
                        }

                        color: workspace.nodeColor

                        Rectangle {
                            anchors.centerIn: parent

                            width: parent.width - 8
                            height: width
                            radius: width/2

                            color: workspace.outletColor
                        }
                    }


                    MouseArea {
                        id: mouseArea2

                        anchors.fill: parent

                        drag.target: parent

                        onPressed: {
                            parent.z = workspace.maxZ++;
                        }
                    }
                }


                //
                // Item 3
                //
                Rectangle {
                    id: item3

                    // Flag indicating if our item is closed or not
                    property bool isClosed: false

                    x: 400
                    y: 150

                    width: 150
                    height: (isClosed) ? 75 : 300


                    color: (isClosed) ? workspace.nodeCollapsedColor : workspace.nodeColor

                    radius: 8

                    border {
                        width: 2
                        color: mouseArea3.pressed ? workspace.nodeSelectedBorderColor : workspace.nodeBorderColor
                    }


                    Behavior on height {
                        NumberAnimation {}
                    }

                    Rectangle {
                        id: item3NodeIn1

                        anchors {
                            horizontalCenter: parent.left
                        }

                        y: (item3.isClosed) ? (parent.height/2 - height/2) : (parent.height/3 - height/2)

                        width: 20
                        height: width
                        radius: width/2

                        border {
                            color: workspace.inletColor
                            width: 2
                        }

                        color: workspace.nodeColor

                        Rectangle {
                            anchors.centerIn: parent

                            width: parent.width - 8
                            height: width
                            radius: width/2

                            color: workspace.inletColor
                        }
                    }


                    Rectangle {
                        id: item3NodeIn2

                        anchors {
                            horizontalCenter: parent.left
                        }

                        y: (item3.isClosed) ? (parent.height/2 - height/2) : (parent.height * 2/3 - height/2)

                        width: 20
                        height: width
                        radius: width/2

                        border {
                            color: workspace.inletColor
                            width: 2
                        }

                        color: workspace.nodeColor

                        Rectangle {
                            anchors.centerIn: parent

                            width: parent.width - 8
                            height: width
                            radius: width/2

                            color: workspace.inletColor
                        }
                    }


                    Rectangle {
                        id: item3NodeOut1

                        anchors {
                            horizontalCenter: parent.right
                        }

                        y: (item3.isClosed) ? (parent.height/2 - height/2) : (parent.height/3 - height/2)

                        width: 20
                        height: width
                        radius: width/2

                        border {
                            color: workspace.outletColor
                            width: 2
                        }

                        color: workspace.nodeColor

                        Rectangle {
                            anchors.centerIn: parent

                            width: parent.width - 8
                            height: width
                            radius: width/2

                            color: workspace.outletColor
                        }
                    }


                    Rectangle {
                        id: item3NodeOut2

                        anchors {
                            horizontalCenter: parent.right
                        }

                        y: (item3.isClosed) ? (parent.height/2 - height/2) : (parent.height * 2/3 - height/2)

                        width: 20
                        height: width
                        radius: width/2

                        border {
                            color: workspace.outletColor
                            width: 2
                        }

                        color: workspace.nodeColor

                        Rectangle {
                            anchors.centerIn: parent

                            width: parent.width - 8
                            height: width
                            radius: width/2

                            color: workspace.outletColor
                        }
                    }


                    MouseArea {
                        id: mouseArea3

                        anchors.fill: parent

                        drag.target: parent

                        onPressed: {
                            parent.z = workspace.maxZ++;
                        }

                        onDoubleClicked: {
                            item3.isClosed = !item3.isClosed;
                        }
                    }
                }


                //
                // Item 4
                //
                Rectangle {
                    id: item4

                    x: 400
                    y: 550

                    width: 150
                    height: 70


                    color: workspace.nodeColor

                    radius: 8

                    border {
                        width: 2
                        color: mouseArea4.pressed ? workspace.nodeSelectedBorderColor : workspace.nodeBorderColor
                    }


                    Rectangle {
                        id: item4NodeIn

                        anchors {
                            verticalCenter: parent.verticalCenter
                            horizontalCenter: parent.left
                        }

                        width: 20
                        height: width
                        radius: width/2

                        border {
                            color: workspace.inletColor
                            width: 2
                        }

                        color: workspace.nodeColor

                        Rectangle {
                            anchors.centerIn: parent

                            width: parent.width - 8
                            height: width
                            radius: width/2

                            color: workspace.inletColor
                        }
                    }

                    Rectangle {
                        id: item4NodeOut

                        anchors {
                            horizontalCenter: parent.right
                            verticalCenter: parent.verticalCenter
                        }

                        width: 20
                        height: width
                        radius: width/2

                        border {
                            color: workspace.outletColor
                            width: 2
                        }

                        color: workspace.nodeColor

                        Rectangle {
                            anchors.centerIn: parent

                            width: parent.width - 8
                            height: width
                            radius: width/2

                            color: workspace.outletColor
                        }
                    }


                    MouseArea {
                        id: mouseArea4

                        anchors.fill: parent

                        drag.target: parent

                        onPressed: {
                            parent.z = workspace.maxZ++;
                        }
                    }
                }


                //
                // Item 5
                //
                Rectangle {
                    id: item5

                    // Flag indicating if our item is closed or not
                    property bool isClosed: false


                    x: 700
                    y: 250

                    width: 150
                    height: (isClosed) ? 75 : 150

                    color: (isClosed) ? workspace.nodeCollapsedColor : workspace.nodeColor

                    radius: 8

                    border {
                        width: 2
                        color: mouseArea5.pressed ? workspace.nodeSelectedBorderColor : workspace.nodeBorderColor
                    }


                    Behavior on height {
                        NumberAnimation {}
                    }

                    Rectangle {
                        id: item5NodeIn1

                        anchors {
                            horizontalCenter: parent.left
                        }

                        y: (item5.isClosed) ? (parent.height/2 - height/2) : (parent.height/3 - height/2)

                        width: 20
                        height: width
                        radius: width/2

                        border {
                            color: workspace.inletColor
                            width: 2
                        }

                        color: workspace.nodeColor

                        Rectangle {
                            anchors.centerIn: parent

                            width: parent.width - 8
                            height: width
                            radius: width/2

                            color: workspace.inletColor
                        }
                    }


                    Rectangle {
                        id: item5NodeIn2

                        anchors {
                            horizontalCenter: parent.left
                        }

                        y: (item5.isClosed) ? (parent.height/2 - height/2) : (parent.height * 2/3 - height/2)

                        width: 20
                        height: width
                        radius: width/2

                        border {
                            color: workspace.inletColor
                            width: 2
                        }

                        color: workspace.nodeColor

                        Rectangle {
                            anchors.centerIn: parent

                            width: parent.width - 8
                            height: width
                            radius: width/2

                            color: workspace.inletColor
                        }
                    }

                    MouseArea {
                        id: mouseArea5

                        anchors.fill: parent

                        drag.target: parent

                        onPressed: {
                            parent.z = workspace.maxZ++;
                        }

                        onDoubleClicked: {
                            item5.isClosed = !item5.isClosed;
                        }
                    }
                }


                //----------------------------------------
            }

        }



        //----------------------------------------------------------------
        //
        // Drop-area to handle drag-n-drop of agents from our agents list
        //
        //----------------------------------------------------------------

        DropArea {
            id: workspaceDropArea

            anchors.fill: parent


            // Get coordinates of drop
            function getDropCoordinates()
            {
                return workspace.mapFromItem(workspaceDropArea, workspaceDropArea.drag.x, workspaceDropArea.drag.y);
            }

            onEntered: {
                if (drag.source !== null)
                {
                    var dragItem = drag.source;
                    console.log("source "+dragItem)
                    if (typeof dragItem.agent !== 'undefined')
                    {
                        dropGhost.agent = dragItem.agent;
                    }
                    else
                    {
                        console.log("no agent "+dragItem.agent)
                        dropGhost.agent = null;
                    }
                }
                else
                {
                    console.log("no source "+ drag.source)
                }
            }


            onPositionChanged: {
                dropGhost.x = drag.x;
                dropGhost.y = drag.y;
            }


            onExited: {
                dropGhost.agent = null;
            }



            //
            // Ghost displayed when a dragged item enters the bounds of our drop area
            //
            Item {
                id: dropGhost

                property var agent: null;

                opacity: (workspaceDropArea.containsDrag ? 1 : 0)
                visible: (opacity != 0)

                Behavior on opacity {
                    NumberAnimation {}
                }


                Rectangle {
                    transformOrigin: Item.TopLeft

                    scale: workspace.scale

                    width: 150
                    height: 150

                    opacity: 0.7

                    color: workspace.nodeColor

                    radius: 8

                    border {
                        width: 2
                        color: workspace.nodeBorderColor
                    }

                    visible: workspaceDropArea.containsDrag

                    Text {
                        id: agentName

                        anchors {
                            fill: parent
                            margins: 5
                        }

                        verticalAlignment: Text.AlignVCenter
                        horizontalAlignment: Text.AlignHCenter

                        elide: Text.ElideRight

                        text: dropGhost.agent ? dropGhost.agent.name : ""

                        color: (dropGhost.agent && (dropGhost.agent.isON === true) && !dropGhost.agent.hasOnlyDefinition) ? MasticTheme.agentsListLabelColor : MasticTheme.agentOFFLabelColor

                        font: MasticTheme.headingFont
                    }
                }
            }
        }




        //
        // Legend
        //
        Text {
            id: legend

            anchors {
                left: parent.left
                right: parent.right
                top: parent.top
                leftMargin: 20
                rightMargin: 20
                topMargin: 20
            }

            text: qsTr("Demo: pan, pinch to zoom, drag-n-drop of nodes, double-click to expand/collapse a node with multiple slots")

            font: MasticTheme.normalFont
            color: MasticTheme.whiteColor

            wrapMode: Text.WordWrap
        }
    }
}
