/*
 *	IngeScape Editor
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

import INGESCAPE 1.0

// parent-directory
import ".." as Editor;

// Popups
import "../popup/" as Popups;


Item {
    id: rootItem

    anchors.fill: parent


    //--------------------------------
    //
    // Properties
    //
    //--------------------------------

    // Controller associated to our view
    property var controller: null;

    // Minimum scale factor
    readonly property real minimumScale: 0.25;

    // Maximum scale factor
    readonly property real maximumScale: 4;

    // Duration of automatic pan and/or zoom animations in milliseconds
    readonly property int automaticPanZoomAnimationDuration: 300;

    // Size of a cell of our background
    readonly property int backgroundCellSize: 60 ;

    // Number of subdivisions for background cells
    readonly property int backgroundCellNumberOfSubDivisions: 5

    // Zoom-in delta scale factor
    readonly property real zoomInDeltaScaleFactor: 1.2


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
        //TODO get the bounding box of all nodes from our controller ??
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
                    // Remove invisible links because AgentNodeView creates links attached to (0,0)
                    ( !_qmlItemIsA(child, "Link") || (_qmlItemIsA(child, "Link") && child.visible) )
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

            _showArea(area);
        }
    }

    // Set our zoom level to a given value
    function setZoomLevel(zoom)
    {
        if (zoom > 0)
        {
            // Check bounds of zoom
            zoom = Math.min(rootItem.maximumScale, Math.max(rootItem.minimumScale, zoom));

            // Get position of our center in workspace
            var viewCenterInWorkspace = rootItem.mapToItem(workspace, rootItem.width/2, rootItem.height/2);

            var targetX = rootItem.width/2 - viewCenterInWorkspace.x * zoom;
            var targetY = rootItem.height/2 - viewCenterInWorkspace.y * zoom;

            workspaceXAnimation.to = targetX;
            workspaceYAnimation.to = targetY;
            workspaceScaleAnimation.to = zoom;

            workspaceXAnimation.restart();
            workspaceYAnimation.restart();
            workspaceScaleAnimation.restart();
        }
        else
        {
            console.log("setZoomLevel: invalid zoom factor "+zoom)
        }
    }


    // Zoom-in
    function zoomIn()
    {
        setZoomLevel(workspace.scale * rootItem.zoomInDeltaScaleFactor);
    }


    // Zoom-out
    function zoomOut()
    {
        setZoomLevel(workspace.scale / rootItem.zoomInDeltaScaleFactor);
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
            // Calculate the required scale factor to show our area
            var scaleX = rootItem.width/areaWidth;
            var scaleY = rootItem.height/areaHeight;
            var requiredScale = Math.min(scaleX, scaleY);

            // Check bounds of our scale factor
            var targetScale = Math.min(rootItem.maximumScale, Math.max(rootItem.minimumScale, requiredScale));

            // Compute position of our worksâce
            var targetX = rootItem.width/2 - (area.x + areaWidth/2) * targetScale;
            var targetY = rootItem.height/2 - (area.y + areaHeight/2) * targetScale;

            // Configure animations
            workspaceXAnimation.to = targetX;
            workspaceYAnimation.to = targetY;
            workspaceScaleAnimation.to = targetScale;

            // Start animations
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




    //--------------------------------
    //
    // Behavior
    //
    //--------------------------------

    Connections {
        target: controller

        ignoreUnknownSignals: true

        onZoomIn: {
            rootItem.zoomIn();
        }

        onZoomOut: {
            rootItem.zoomOut();
        }

        onFitToView: {
            rootItem.showAll();
        }

        onResetZoom: {
            rootItem.setZoomLevel(1);
        }

        onChangesOnLinksWhileMappingUnactivated: {
            //console.log("QML: on Changes on Links While Mapping Unactivated");

            // Open the popup about mapping modifications
            mappingModificationsPopup.open();
        }
    }

    Connections {
        target: IngeScapeEditorC

        onResetMappindView: {
            rootItem.showAll();
        }
    }

    Component.onCompleted: {
        if (controller) {
            //console.log("QML: Graph View completed !");
            // Update the flag
            controller.isLoadedView = true;
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

            color: IngeScapeEditorTheme.agentsMappingBackgroundColor

            SeamlessGrid {
                anchors.fill: parent

                offsetX: workspace.x
                offsetY: workspace.y

                cellSize: rootItem.backgroundCellSize * workspace.scale
            }
        }


        //
        // New agents spawn zone
        //
        // Always centered in the viewport.
        // Used to position the new agents onto the mapping view.
        // Always invisible but can be make visible for debugging purposes.
        //
        // NOTE: These kind of rectangles could be extracted into utility QML items to render specific zones (for debug pruposes)... Just a thought.
        //
        Rectangle {
            id: spawnZone

            anchors.centerIn: parent

            width: controller.viewWidth * workspace.scale
            height: controller.viewHeight * workspace.scale

            // Always invisible because it's only a helper item for positionning new agents onto the mapping view.
            // May be made visible for debugging needs.
            visible: false

            property int _borderWidth: 10
            property string _color: "#FF0000"

            border {
                width: spawnZone._borderWidth
                color: spawnZone._color
            }

            // Actual spawn zone represents the margins insinde the spawning zone into which the agent will actually spawn.
            // The margins ensures that enough of the agent will be visible once spawn.
            Rectangle {
                id: actualSpawnZone

                x: parent.width * 0.05
                y: parent.height * 0.05

                width: parent.width * 0.85
                height: parent.height * 0.85

                property int _borderWidth: 2
                property string _color: "#008800"

                border {
                    width: actualSpawnZone._borderWidth
                    color: actualSpawnZone._color
                }

                Text {
                    anchors {
                        top: actualSpawnZone.top
                        topMargin: actualSpawnZone._borderWidth + 5
                        left: actualSpawnZone.left
                        leftMargin: actualSpawnZone._borderWidth + 5
                    }

                    text: "ACTUAL SPAWN ZONE"

                    color: actualSpawnZone._color
                }
            }

            Text {
                anchors {
                    top: spawnZone.top
                    topMargin: spawnZone._borderWidth + 5
                    left: spawnZone.left
                    leftMargin: spawnZone._borderWidth + 5
                }

                text: "SPAWN ZONE"

                color: spawnZone._color
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
            keys: ["AgentsListItem", "ActionsListItem"]


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
                    if (typeof dragItem.agent !== 'undefined')
                    {
                        // Hide our source to avoid visual artifacts
                        _previousOpacityOfSource = dragItem.opacity;
                        dragItem.opacity = 0;

                        // Configure our ghost
                        dropGhost.agent = dragItem.agent;
                    }
                    // Check if our source has an "action" property
                    else if (typeof dragItem.action !== 'undefined')
                    {
                        console.log("ENTER action... " + dragItem.action.name);

                        // display ghost
                        //dropGhost.action = dragItem.action;
                    }
                    else
                    {
                        dropGhost.agent = null;
                        //dropGhost.action = null;
                    }
                }
            }


            onPositionChanged: {
                if (dropGhost.agent)
                {
                    dropGhost.x = drag.x;
                    dropGhost.y = drag.y;
                }
                /*else if (dropGhost.action)
                {

                }*/
            }


            onExited: {
                var dragItem = drag.source;

                // Check if our source has an "agent" property
                if (typeof dragItem.agent !== 'undefined')
                {
                    // Restore opacity of our source
                    drag.source.opacity = _previousOpacityOfSource;
                }
                // Check if our source has an "action" property
                else if (typeof dragItem.action !== 'undefined')
                {
                    console.log("EXIT action... " + dragItem.action.name);
                }

                // Clean-up ghost
                dropGhost.agent = null;
                //dropGhost.action = null;
            }


            onDropped: {
                var dragItem = drag.source;

                var dropPosition = getDropCoordinates();

                // Check if our source has an "agent" property
                if (typeof dragItem.agent !== 'undefined')
                {
                    if (IngeScapeEditorC.agentsMappingC)
                    {
                        IngeScapeEditorC.agentsMappingC.dropAgentNameToMappingAtPosition(dragItem.agent.name, dropPosition);
                    }

                    // Restore opacity of our source
                    drag.source.opacity = _previousOpacityOfSource;
                }
                // Check if our source has an "action" property
                else if (typeof dragItem.action !== 'undefined')
                {
                    console.log("DROP action... " + dragItem.action.name);

                    if (IngeScapeEditorC.agentsMappingC)
                    {
                        //IngeScapeEditorC.agentsMappingC.dropActionToMappingAtPosition(dragItem.action, dropPosition);

                        // 110 = ActionInMapping.width / 2
                        // 48 = ActionInMapping.height
                        var center = Qt.point(dropPosition.x - 110, dropPosition.y - 48);
                        IngeScapeEditorC.agentsMappingC.dropActionToMappingAtPosition(dragItem.action, center);
                    }
                }

                // Clean-up ghost
                dropGhost.agent = null;
                //dropGhost.action = null;
            }
        }



        //-----------------------------------------------
        //
        // Workspace interaction: pan & zoom
        //
        //-----------------------------------------------

        //
        // PinchArea to capture pinch gesture (zoom-in, zoom-out)
        //
        PinchArea {
            anchors.fill: parent


            onPinchUpdated: {
                // Check if we have at least two points
                if (pinch.pointCount >= 2)
                {
                    // Get position of our center in workspace
                    var viewCenterInWorkspace = rootItem.mapToItem(workspace, rootItem.width/2, rootItem.height/2);

                    // Compute delta between our new scale factor and the previous one
                    var deltaScale = pinch.scale/pinch.previousScale;

                    // Check bounds of scale
                    workspace.scale = Math.min(rootItem.maximumScale, Math.max(rootItem.minimumScale, workspace.scale * deltaScale));

                    // Set position of our workspace
                    workspace.x = rootItem.width/2 - viewCenterInWorkspace.x * workspace.scale;
                    workspace.y = rootItem.height/2 - viewCenterInWorkspace.y * workspace.scale;
                }
            }



            //
            // MouseArea to capture scroll gesture events (trackpad)
            //
            MouseArea {
                anchors.fill: parent

                scrollGestureEnabled: true

                onPressed: {
                    rootItem.forceActiveFocus();

                }

                onWheel: {
                    wheel.accepted = true;

                    if ((wheel.pixelDelta.x !== 0) || (wheel.pixelDelta.y !== 0))
                    {
                        //
                        // Trackpad flick gesture => scroll our workspace
                        //
                        workspace.x += wheel.pixelDelta.x;
                        workspace.y += wheel.pixelDelta.y;
                    }
                }



                //
                // MouseArea used to drag-n-drop our workspace AND handle real mouse wheel events (zoom-in, zoom-out)
                //
                MouseArea {
                    id: mouseAreaWorkspaceDragNDropAndWheelZoom

                    anchors.fill: parent

                    drag {
                        target: workspace
                        smoothed: false
                    }

                    // 2-finger-flick gesture should pass through to our parent MouseArea
                    scrollGestureEnabled: false

                    onPressed: {
                        rootItem.forceActiveFocus();
                        if (controller && controller.selectedAgent) {
                            controller.selectedAgent = null;
                        }

                    }

                    onWheel: {
                        wheel.accepted = true;

                        // Compute mouse position in our workspace
                        var mousePositionInWorkspace = mouseAreaWorkspaceDragNDropAndWheelZoom.mapToItem(workspace, wheel.x, wheel.y);

                        // Check if we must zoom-in or zoom-out
                        if (wheel.angleDelta.y < 0)
                        {
                            //
                            // Zoom-out
                            //

                            // Update scale of our workspace
                            var numberOfWheelDeltaZoomOut = Math.abs(wheel.angleDelta.y)/120;
                            var scaleFactorZoomOut = Math.pow(1/rootItem.zoomInDeltaScaleFactor, numberOfWheelDeltaZoomOut);
                            workspace.scale = Math.min(rootItem.maximumScale, Math.max(rootItem.minimumScale, workspace.scale * scaleFactorZoomOut));

                            // Center our workspace
                            workspace.x = wheel.x - mousePositionInWorkspace.x * workspace.scale;
                            workspace.y = wheel.y - mousePositionInWorkspace.y * workspace.scale;
                        }
                        else if (wheel.angleDelta.y > 0)
                        {
                            //
                            // Zoom-in
                            //

                            // Update scale of our workspace
                            var numberOfWheelDeltaZoomIn = Math.abs(wheel.angleDelta.y)/120;
                            var scaleFactorZoomIn = Math.pow(rootItem.zoomInDeltaScaleFactor, numberOfWheelDeltaZoomIn);
                            workspace.scale = Math.min(rootItem.maximumScale, Math.max(rootItem.minimumScale, workspace.scale * scaleFactorZoomIn));

                            // Center our workspace
                            workspace.x = wheel.x - mousePositionInWorkspace.x * workspace.scale;
                            workspace.y = wheel.y - mousePositionInWorkspace.y * workspace.scale;
                        }
                        // Else: wheel.angleDelta.y  == 0  => invalid wheel event
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
                // Methods and slots
                //
                //------------------------------------------------
                function updateSpawnZoneOffset() {
                    var offset = workspace.mapFromItem(rootItem, spawnZone.x, spawnZone.y)
                    controller.xSpawnZoneOffset = offset.x
                    controller.ySpawnZoneOffset = offset.y
                }

                onXChanged: {
                    updateSpawnZoneOffset()
                }

                onYChanged: {
                    updateSpawnZoneOffset()
                }


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
                    model: controller ? controller.allLinksInMapping : 0;

                    Link {
                        id: link
                        controller: rootItem.controller
                        viewModel: model.QtObject
                    }
                }


                //
                // Nodes
                //
                Repeater {
                    model: controller ? controller.allAgentsInMapping : null;

                    AgentNodeView {
                        id: agent

                        agentMappingVM: model.QtObject

                        controller: rootItem.controller

                        // When the agent is created, the worspace's scale is reset to 100% if it was superior.
                        // This shows the entire spawn zone, ensuring that the new poped agent is visible.
                        Component.onCompleted: {
                            if (workspace.scale > 1) {
                                setZoomLevel(1)
                            }
                        }
                    }
                }


                //
                // Actions
                //
                Repeater {
                    model: controller ? controller.allActionsInMapping : null;

                    ActionInMapping {
                        id: action

                        actionInMappingVM: model.QtObject

                        controller: rootItem.controller
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
            //property var action: null

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
                dropEnabled: (dropGhost.agent === null)
                             ||
                             (dropGhost.agent && IngeScapeEditorC.agentsMappingC && !IngeScapeEditorC.agentsMappingC.getAgentInMappingFromName(dropGhost.agent.name))
            }
        }

    }


    //----------------------------------------------------------------------------------
    //
    // Mapping Modifications Popup
    //
    //----------------------------------------------------------------------------------
    Popups.MappingModificationsPopup {
        id: mappingModificationsPopup

        onCancelMappingActivation: {
            console.log("on Cancel Mapping Activation");

            // UN-activate the mapping
//            IgsModelManager.isMappingConnected = false;
        }

        onSwitchToControl: {
            console.log("on Switch To Control");

            if (IngeScapeEditorC.modelManager)
            {
                // UN-activate the mapping
//                IgsModelManager.isMappingConnected = false;

                // Switch to CONTROL
                IngeScapeEditorC.modelManager.isMappingControlled = true;

                // Activate the mapping
//                IgsModelManager.isMappingConnected = true;
            }
        }

        onStayToObserve: {
            console.log("on Stay To Observe");

            if (controller) {
                controller.resetModificationsWhileMappingWasUNactivated();
            }
        }
    }

}
