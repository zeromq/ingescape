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
    readonly property real minimumScale: 0.25;

    // Maximum scale factor
    readonly property real maximumScale: 4;

    // Duration of automatic pan and/or zoom animations in milliseconds
    readonly property int automaticPanZoomAnimationDuration: 300;


    // Size of a cell of our background
    readonly property int backgroundCellSize: 150;

    // Number of subdivisions for background cells
    readonly property int backgroundCellNumberOfSubDivisions: 5



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
        var x0 = Number.POSITIVE_INFINITY;
        var y0 = Number.POSITIVE_INFINITY;
        var x1 = Number.NEGATIVE_INFINITY;
        var y1 = Number.NEGATIVE_INFINITY;
        var validBoundingBox = false;

        for (var index = 0; index < workspace.visibleChildren.length; index++)
        {
            var child = workspace.visibleChildren[index];

            // Check if our child must be filtered
            if (
                    // We don't need repeaters because they don't have a valid geometry (they create items and add them to their parent)
                    !_qmlItemIsA(child, "Repeater")
                    &&
                    //TEMP FIXME: remove links because AgentNodeView creates links attached to (0,0)
                    !_qmlItemIsA(child, "Link")
                    )
            {
                x0 = Math.min(x0, child.x);
                y0 = Math.min(y0, child.y);

                x1 = Math.max(x1, child.x + child.width);
                y1 = Math.max(y1, child.y + child.height);

                validBoundingBox = true;
            }
            // Else: child has been filtered
        }

        if (validBoundingBox)
        {
            var margin = 5;
            var area = Qt.rect(x0 - margin, y0 - margin, x1 - x0 + 2 * margin, y1 - y0 + 2 * margin);
            console.log("NodeGraphView: bounding box (+ margins) = " + area + " VS workspace.childrenRect =" + workspace.childrenRect);

            _showArea(area);
        }
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


    // Check if a given item is an instance of a given class
    function _qmlItemIsA(item, className)
    {
        var result = false;

        if (item)
        {
            var itemToString = item.toString();

            result = (
                        // class + ( + address + ) => class instance without modification
                        (itemToString.indexOf(className + "(") === 0)
                        ||
                        // QQuick + class + ( + address + ) => basic QML class instance
                        // E.g. QQuickRepeater(0x7fa8c8667070)
                        (itemToString.indexOf("QQuick" + className + "(") === 0)
                        ||
                        // class + _QMLTYPE_ + number + ( + address + ) => class instance with user-defined properties
                        // E.g. AgentNodeView_QMLTYPE_90(0x7f97cb1416e0)
                        (itemToString.indexOf(className + "_QML") === 0)
                        );
        }

        return result;
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
        // Seamless background
        //
        Rectangle {
            id: seamlessBackgroundVector

            anchors.fill: parent

            color: MasticTheme.agentsMappingBackgroundColor

            SeamlessGrid {
                anchors.fill: parent

                offsetX: workspace.x
                offsetY: workspace.y

                cellSize: rootItem.backgroundCellSize * workspace.scale
                numberOfSubDivisions: rootItem.backgroundCellNumberOfSubDivisions

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

            // Only accept drag events from AgentsListItem
            keys: ["AgentsListItem"]


            // To save the opacity of our source
            property real _previousOpacityOfSource: 1;


            // Get coordinates of drop
            function getDropCoordinates()
            {
                return workspace.mapFromItem(workspaceDropArea, workspaceDropArea.drag.x, workspaceDropArea.drag.y);
            }


            onEntered: {
                if (drag.source !== null)
                {
                    var dragItem = drag.source;

                    // Check if our source has an "agent" property
                    // NB: it should be useless because we only accept drag events with key AgentsListItem
                    if (typeof dragItem.agent !== 'undefined')
                    {
                        // Hide our source to avoid visual artifacts
                        _previousOpacityOfSource = dragItem.opacity;
                        dragItem.opacity = 0;

                        // Configure our ghost
                        dropGhost.agent = dragItem.agent;
                    }
                    else
                    {
                        dropGhost.agent = null;
                    }
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
                    // Restore opacity of our source
                    drag.source.opacity = _previousOpacityOfSource;
                }


                // Clean-up ghost
                dropGhost.agent = null;
            }


            onDropped: {
                var dragItem = drag.source;
                if (typeof dragItem.agent !== 'undefined')
                {
                    var dropPosition = getDropCoordinates();

                    if (MasticEditorC.agentsMappingC)
                    {
                        MasticEditorC.agentsMappingC.dropAgentToMappingAtPosition(dragItem.agent.name, dragItem.agent.models, dropPosition);
                    }

                    // Restore opacity of our source
                    drag.source.opacity = _previousOpacityOfSource;
                }

                 dropGhost.agent = null;
            }
        }



        //-----------------------------------------------
        //
        // Workspace interaction: pan & zoom
        //
        //-----------------------------------------------
        PinchArea {
            anchors.fill: parent

            pinch {
                target: workspace

                minimumScale: rootItem.minimumScale
                maximumScale: rootItem.maximumScale
            }


            //
            // MouseArea used to drag-n-drop our workspace AND handle mouse wheel events and scroll gesture events (trackpad)
            //
            MouseArea {
                anchors.fill: parent

                drag.target: workspace

                scrollGestureEnabled: true

                onPressed: {
                    rootItem.forceActiveFocus();
                }

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
            // Workspace: nodes and links
            //
            //-----------------------------------------------
            Item {
                id: workspace

                transformOrigin: Item.TopLeft

                width: parent.width
                height: parent.height


                //------------------------------------------------
                //
                // Properties
                //
                //------------------------------------------------

                // Maximum Z-index
                property int maxZ: 0



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
                // Content of our workspace
                //
                //------------------------------------------------


                //
                // Links between nodes
                //
                Repeater {
                    model : controller? controller.allMapInMapping : 0;

                    Link {
                        id : link

                        mapBetweenIOPVM: model.QtObject
                    }
                }



                //
                // Nodes
                //
                Repeater {
                    model : controller? controller.agentInMappingVMList : 0;


                    AgentNodeView {
                        id: agent

                        agentMappingVM : model.QtObject
                        controller : rootItem.controller
                    }
                }

            }
        }



        //----------------------------------------------------------------------------------
        //
        // Ghost displayed when we drag-n-drop on agent from our list of agents (left panel)
        //
        //----------------------------------------------------------------------------------
        Item {
            id: dropGhost

            property var agent: null

            opacity: (agent && workspaceDropArea.containsDrag ? 1 : 0)
            visible: (opacity != 0)

            Behavior on opacity {
                NumberAnimation {}
            }

            AgentNodeView {
                transformOrigin: Item.TopLeft

                scale: workspace.scale

                isReduced: true

                agentName: dropGhost.agent ? dropGhost.agent.name : ""
                dropEnabled : (dropGhost.agent === null)
                              ||
                              ((dropGhost.agent && MasticEditorC.agentsMappingC) && !MasticEditorC.agentsMappingC.getAgentInMappingForName(dropGhost.agent.name))
            }
        }

    }
}
