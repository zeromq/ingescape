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

    // Duration of automatic pan and/or zoom animations in milliseconds
    readonly property int automaticPanZoomAnimationDuration: 300


    //--------------------------------
    //
    // Functions
    //
    //--------------------------------


    // Center our view on a given node
    function centerViewOnNode(node)
    {
        if (node)
        {
            centerViewOn(node.x + node.width/2, node.y + node.height/2);
        }
    }


    // Center our view on a given position
    function centerViewOn(x, y)
    {
        var targetX = rootItem.width/2 - x * workspace.scale;
        var targetY = rootItem.height/2 - y * workspace.scale;

        _scrollWorkspaceTo(targetX, targetY);
    }


    // Show all
    function showAll()
    {
        //TODO get the bounding box of all nodes from our controller
        var x0 = Math.min(item1.x, Math.min(item2.x, Math.min(item3.x, Math.min(item4.x, item5.x))));
        var y0 = Math.min(item1.y, Math.min(item2.y, Math.min(item3.y, Math.min(item4.y, item5.y))));

        var x1 = Math.max(item1.x + item1.width, Math.max(item2.x + item2.width, Math.max(item3.x + item3.width, Math.max(item4.x + item4.width, item5.x + item5.width))));
        var y1 = Math.max(item1.y + item1.height, Math.max(item2.y + item2.height, Math.max(item3.y + item3.height, Math.max(item4.y + item4.height, item5.y + item5.height))));

        var margin = 5;
        var area = Qt.rect(x0 - margin, y0 - margin, x1 - x0 + 2 * margin, y1 - y0 + 2 * margin);
        //--------------------

        _showArea(area);
    }




    // Scroll our workspace to a given position (top-left corner)
    function _scrollWorkspaceTo(x, y)
    {
        workspaceXAnimation.to = x;
        workspaceYAnimation.to = y;

        workspaceXAnimation.restart();
        workspaceYAnimation.restart();
    }


    // Show a given area
    function _showArea(area)
    {
        var areaWidth = area.width;
        var areaHeight = area.height;

        if ((areaWidth > 0) && (areaHeight > 0))
        {
            var scaleX = rootItem.width/areaWidth;
            var scaleY = rootItem.height/areaHeight;

            var targetScale = Math.min(scaleX, scaleY);
            var targetX = rootItem.width/2 - (area.x + areaWidth/2) * targetScale;
            var targetY = rootItem.height/2 - (area.y + areaHeight/2) * targetScale;

            workspaceXAnimation.to = targetX;
            workspaceYAnimation.to = targetY;
            workspaceScaleAnimation.to = targetScale;

            workspaceXAnimation.restart();
            workspaceYAnimation.restart();
            workspaceScaleAnimation.restart();
        }
        else
        {
            console.log("_showArea: invalid area "+area);
        }
    }



    // TEMP: to test showAll
    focus: true
    Keys.onPressed: {
         if (event.key === Qt.Key_Space)
         {
            showAll();

             event.accepted = true;
         }
    }


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
                        dragItem.opacity = 0;
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
                if (dropGhost.agent)
                {
                    dropGhost.x = drag.x;
                    dropGhost.y = drag.y;
                }
            }


            onExited: {
                var dragItem = drag.source;
                if (typeof dragItem.agent !== 'undefined')
                {
                    drag.source.opacity = 1;
                }
                dropGhost.agent = null;
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

                scrollGestureEnabled: true

                onPressed: rootItem.forceActiveFocus();

                onWheel: {
                    wheel.accepted = true;

                    // Check if we have a real wheel event
                    if ((wheel.pixelDelta.x !== 0) || (wheel.pixelDelta.y !== 0))
                    {
                        //
                        // Trackpad event
                        //

                        workspace.x += wheel.pixelDelta.x;
                        workspace.y += wheel.pixelDelta.y;
                    }
                    else
                    {
                        //
                        // Physical mouse wheel event
                        //

                        //TODO: zoom at (x, y)
                        if (wheel.angleDelta.y > 0)
                        {
                            workspace.scale = Math.max(rootItem.minimumScale, workspace.scale/1.2);
                        }
                        else if (wheel.angleDelta.y < 0)
                        {
                            workspace.scale = Math.min(rootItem.maximumScale, workspace.scale * 1.2);
                        }
                        // Else: wheel.angleDelta.y  == 0  => end of gesture
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
                // Animations used to scroll and/or scale our view
                //
                //------------------------------------------------

                PropertyAnimation {
                    id: workspaceXAnimation

                    target: workspace
                    property: "x"

                    duration: rootItem.automaticPanZoomAnimationDuration
                }

                PropertyAnimation {
                    id: workspaceYAnimation

                    target: workspace
                    property: "y"

                    duration: rootItem.automaticPanZoomAnimationDuration
                }

                PropertyAnimation {
                    id: workspaceScaleAnimation

                    target: workspace
                    property: "scale"

                    duration: rootItem.automaticPanZoomAnimationDuration
                }



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
                //       model: list of MapBetweenIOPVM

                Repeater {
                    model : controller? controller.allMapInMapping : 0;

                    Link {
                        id : link
                        mapBetweenIOPVM: model.QtObject
                    }
                }


                //
//                // Link between [item4, nodeOut] and [item5, nodeIn2]
//                //
//                Link {
//                    id: link6

//                    firstPoint: Qt.point(item4.x + item4NodeOut.x + item4NodeOut.width/2, item4.y + item4NodeOut.y + item4NodeOut.height/2)

//                    secondPoint: Qt.point(item5.x + item5NodeIn2.x + item5NodeIn2.width/2, item5.y + item5NodeIn2.y + item5NodeIn2.height/2)

//                     onClicked: console.log("Click on link6")
//               }

                //----------------------------------------


                //----------------------------------------
                //       model: list of AgentMappingVM

                Repeater {
                    model : controller? controller.agentInMappingVMList : 0;


                    AgentNodeView {
                        id: agent
                        agentMappingVM : model.QtObject

                        controller : rootItem.controller
                    }
                }

//                //
//                // Item 4
//                //
//                Rectangle {
//                    id: item4

//                    x: 400
//                    y: 550

//                    width: 150
//                    height: 70


//                    color: workspace.nodeColor

//                    radius: 8

//                    border {
//                        width: 2
//                        color: mouseArea4.pressed ? workspace.nodeSelectedBorderColor : workspace.nodeBorderColor
//                    }


//                    Rectangle {
//                        id: item4NodeIn

//                        anchors {
//                            verticalCenter: parent.verticalCenter
//                            horizontalCenter: parent.left
//                        }

//                        width: 20
//                        height: width
//                        radius: width/2

//                        border {
//                            color: workspace.inletColor
//                            width: 2
//                        }

//                        color: workspace.nodeColor

//                        Rectangle {
//                            anchors.centerIn: parent

//                            width: parent.width - 8
//                            height: width
//                            radius: width/2

//                            color: workspace.inletColor
//                        }
//                    }

//                    Rectangle {
//                        id: item4NodeOut

//                        anchors {
//                            horizontalCenter: parent.right
//                            verticalCenter: parent.verticalCenter
//                        }

//                        width: 20
//                        height: width
//                        radius: width/2

//                        border {
//                            color: workspace.outletColor
//                            width: 2
//                        }

//                        color: workspace.nodeColor

//                        Rectangle {
//                            anchors.centerIn: parent

//                            width: parent.width - 8
//                            height: width
//                            radius: width/2

//                            color: workspace.outletColor
//                        }
//                    }


//                    MouseArea {
//                        id: mouseArea4

//                        anchors.fill: parent

//                        drag.target: parent

//                        onPressed: {
//                            parent.z = workspace.maxZ++;
//                        }

//                        onDoubleClicked: {
//                            rootItem.centerViewOnNode(item4);
//                        }
//                    }
//                }


//                //
//                // Item 5
//                //
//                Rectangle {
//                    id: item5

//                    // Flag indicating if our item is closed or not
//                    property bool isClosed: false


//                    x: 700
//                    y: 250

//                    width: 150
//                    height: (isClosed) ? 75 : 150

//                    color: (isClosed) ? workspace.nodeCollapsedColor : workspace.nodeColor

//                    radius: 8

//                    border {
//                        width: 2
//                        color: mouseArea5.pressed ? workspace.nodeSelectedBorderColor : workspace.nodeBorderColor
//                    }


//                    Behavior on height {
//                        NumberAnimation {}
//                    }

//                    Rectangle {
//                        id: item5NodeIn1

//                        anchors {
//                            horizontalCenter: parent.left
//                        }

//                        y: (item5.isClosed) ? (parent.height/2 - height/2) : (parent.height/3 - height/2)

//                        width: 20
//                        height: width
//                        radius: width/2

//                        border {
//                            color: workspace.inletColor
//                            width: 2
//                        }

//                        color: workspace.nodeColor

//                        Rectangle {
//                            anchors.centerIn: parent

//                            width: parent.width - 8
//                            height: width
//                            radius: width/2

//                            color: workspace.inletColor
//                        }
//                    }


//                    Rectangle {
//                        id: item5NodeIn2

//                        anchors {
//                            horizontalCenter: parent.left
//                        }

//                        y: (item5.isClosed) ? (parent.height/2 - height/2) : (parent.height * 2/3 - height/2)

//                        width: 20
//                        height: width
//                        radius: width/2

//                        border {
//                            color: workspace.inletColor
//                            width: 2
//                        }

//                        color: workspace.nodeColor

//                        Rectangle {
//                            anchors.centerIn: parent

//                            width: parent.width - 8
//                            height: width
//                            radius: width/2

//                            color: workspace.inletColor
//                        }
//                    }

//                    MouseArea {
//                        id: mouseArea5

//                        anchors.fill: parent

//                        drag.target: parent

//                        onPressed: {
//                            parent.z = workspace.maxZ++;
//                        }

//                        onDoubleClicked: {
//                            item5.isClosed = !item5.isClosed;
//                        }
//                    }
//                }


                //----------------------------------------
            }

        }



        //
        // Ghost displayed when a dragged item enters the bounds of our drop area
        //
        Item {
            id: dropGhost

            property var agent: null;

            opacity: (agent ? (workspaceDropArea.containsDrag ? 1 : 0) : 0);
            visible: (opacity != 0)



            Behavior on opacity {
                NumberAnimation {}
            }

            AgentNodeView {
                transformOrigin: Item.TopLeft

                scale: workspace.scale
                isReduced : true
                agentName : dropGhost.agent ? dropGhost.agent.name : ""
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

            text: qsTr("Demo: pan, pinch to zoom, drag-n-drop of nodes, double-click to expand/collapse a node with multiple slots, space to show all")

            font: MasticTheme.normalFont
            color: MasticTheme.whiteColor

            wrapMode: Text.WordWrap
        }
    }
}
