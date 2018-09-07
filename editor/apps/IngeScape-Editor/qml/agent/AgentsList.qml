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
 *      Justine Limoges    <limoges@ingenuity.io>
 *      Vincent Peyruqueou <peyruqueou@ingenuity.io>
 *
 */

import QtQuick 2.8
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4

import I2Quick 1.0

import INGESCAPE 1.0


// agent sub-directory
import "../agentsmapping" as AgentMapping

// popup sub-directory
import "../popup" as Popups

// theme sub-directory
import "../theme" as Theme;

// parent-directory
import ".." as Editor;


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


    //

    //-----------------------------------------
    //
    // Functions
    //
    //-----------------------------------------

    // Find a root layer by its object name
    function findLayerRootByObjectName(startingObject, layerObjectName)
    {
        var currentObject = startingObject;
        var layerRoot = null;

        while ((currentObject !== null) && (layerRoot == null))
        {
            var index = 0;
            while ((index < currentObject.data.length) && (layerRoot == null))
            {
                if (currentObject.data[index].objectName === layerObjectName)
                {
                    layerRoot = currentObject.data[index];
                }
                index++;
            }

            currentObject = currentObject.parent;
        }

        return layerRoot;
    }


    //--------------------------------
    //
    // Content
    //
    //--------------------------------

    // allowing to deselect selected agent
    MouseArea {
        anchors.fill: parent
        onClicked: {
            if (controller.selectedAgent)
            {
                controller.selectedAgent = null;
            }
        }
    }

    //
    // List of agents
    //
    ScrollView {
        id : agentsListScrollView

        anchors {
            top: parent.top
            topMargin: 78
            bottom: parent.bottom
            left: parent.left
            right: parent.right
        }

        // Prevent drag overshoot on Windows
        flickableItem.boundsBehavior: Flickable.OvershootBounds

        style: IngeScapeScrollViewStyle {
        }

        // Content of our scrollview
        ListView {
            id: agentsList

            model: controller.agentsList

            delegate: componentAgentListItem


            //
            // Transition animations
            //
            add: Transition {
                NumberAnimation { property: "opacity"; from: 0.0; to: 1.0 }
                NumberAnimation { property: "scale"; from: 0.0; to: 1.0 }
            }

            displaced: Transition {
                NumberAnimation { properties: "x,y"; easing.type: Easing.OutBounce }

                // ensure opacity and scale values return to 1.0
                NumberAnimation { property: "opacity"; to: 1.0 }
                NumberAnimation { property: "scale"; to: 1.0 }
            }

            move: Transition {
                NumberAnimation { properties: "x,y"; easing.type: Easing.OutBounce }

                // ensure opacity and scale values return to 1.0
                NumberAnimation { property: "opacity"; to: 1.0 }
                NumberAnimation { property: "scale"; to: 1.0 }
            }

            remove: Transition {
                // ensure opacity and scale values return to 0.0
                NumberAnimation { property: "opacity"; to: 0.0 }
                NumberAnimation { property: "scale"; to: 0.0 }
            }

        }
    }



    //
    // Header
    //
    Rectangle {
        id: header

        anchors {
            top: parent.top
            left: parent.left
            right: parent.right
            bottom : agentsListScrollView.top
        }

        color : IngeScapeTheme.selectedTabsBackgroundColor

        Row {
            id: headerRow

            height: btnAddAgent.height
            spacing : 8

            anchors {
                top: parent.top
                topMargin: 23
                left: parent.left
                leftMargin: 10
            }

            Button {
                id: btnAddAgent

                enabled:false
                activeFocusOnPress: true

                anchors {
                    verticalCenter: parent.verticalCenter
                }

                style: Theme.LabellessSvgButtonStyle {
                    fileCache: IngeScapeTheme.svgFileINGESCAPE

                    pressedID: releasedID + "-pressed"
                    releasedID: "new-agent"
                    disabledID : releasedID + "-disabled"

                }

                onClicked: {
                    console.log("Create a new Agent")
                    // TODO
                }
            }

            Rectangle {
                anchors {
                    verticalCenter: parent.verticalCenter
                }
                height : btnAddAgent.height
                width : 1
                color : IngeScapeTheme.blueGreyColor
            }

            Button {
                id: btnImportAgentOrAgentsList

                anchors {
                    verticalCenter: parent.verticalCenter
                }
                activeFocusOnPress: true

                style: Theme.LabellessSvgButtonStyle {
                    fileCache: IngeScapeTheme.svgFileINGESCAPE

                    pressedID: releasedID + "-pressed"
                    releasedID: "import"
                    disabledID : releasedID + "-disabled"

                }

                onClicked: {
                    if (IngeScapeEditorC.modelManager)
                    {
                        //console.log("Import Agent(s)")
                        var success = IngeScapeEditorC.modelManager.importAgentOrAgentsListFromSelectedFile();
                        if (!success) {
                            popupErrorMessage.open();
                        }
                    }
                }
            }

        }

        Row {
            id: headerRow2

            height: btnAddAgent.height
            spacing : 8

            anchors {
                verticalCenter: headerRow.verticalCenter
                right : parent.right
                rightMargin: 10
            }


            Button {
                id: btnExportAgentsList

                enabled: visible && (controller.agentsList.count > 0)
                activeFocusOnPress: true

                anchors {
                    verticalCenter: parent.verticalCenter
                }

                style: Theme.LabellessSvgButtonStyle {
                    fileCache: IngeScapeTheme.svgFileINGESCAPE

                    pressedID: releasedID + "-pressed"
                    releasedID: "export"
                    disabledID : releasedID + "-disabled"
                }

                onClicked: {
                    if (IngeScapeEditorC.agentsSupervisionC) {
                        //console.log("Export Agent(s)")
                        IngeScapeEditorC.agentsSupervisionC.exportAgentsListToSelectedFile();
                    }
                }
            }


            Rectangle {
                anchors {
                    verticalCenter: parent.verticalCenter
                }
                height: btnRemoveAgent.height
                width: 1
                color: IngeScapeTheme.blueGreyColor
            }


            Button {
                id: btnRemoveAgent

                enabled: false
                activeFocusOnPress: true

                anchors {
                    verticalCenter: parent.verticalCenter
                }

                style: Theme.LabellessSvgButtonStyle {
                    fileCache: IngeScapeTheme.svgFileINGESCAPE

                    pressedID: releasedID + "-pressed"
                    releasedID: "delete-all"
                    disabledID : releasedID + "-disabled"

                }

                onClicked: {
                    console.log("Remove Agent")
                    // TODO
                }
            }

        }
    }


    //
    // Separator
    //
    Rectangle {
        anchors {
            bottom: agentsListScrollView.top
            left: parent.left
            right: parent.right
        }

        color: IngeScapeTheme.leftPanelBackgroundColor

        height: 1
    }



    //
    // Visual representation of an agent in our list
    //
    Component {
        id: componentAgentListItem

        Item {
            id : agentItem

            width: IngeScapeTheme.leftPanelWidth
            height: 85


            // Not Draggable Agent Item
            AgentsListItem {
                id : notDraggableItem

                anchors.fill : parent

                agent : model.QtObject
                controller: rootItem.controller

                visible: mouseArea.drag.active

                agentItemIsHovered: mouseArea.containsMouse
            }

            // Draggable Agent Item
            Item {
                id : draggableItem

                height : notDraggableItem.height
                width : notDraggableItem.width

                // Reference to our agent that can be used by a DropArea item
                property var agent: model.QtObject

                Drag.active: mouseArea.drag.active
                Drag.hotSpot.x: 0
                Drag.hotSpot.y: 0
                Drag.keys: ["AgentsListItem"]

                MouseArea {
                    id: mouseArea

                    anchors.fill: draggableItem

                    hoverEnabled: true

                    // Disable smoothed so that the Item pixel from where we started the drag remains under the mouse cursor
                    drag.smoothed: false
                    drag.target: draggableItem

                    cursorShape: (mouseArea.drag.active)? Qt.ClosedHandCursor : Qt.OpenHandCursor

                    onPressed: {
                        if (controller) {
                            if (controller.selectedAgent === model.QtObject)
                            {
                                controller.selectedAgent = null;
                            }
                            else {
                                controller.selectedAgent = model.QtObject;
                            }

                        }

                        // Find our layer and reparent our popup in it
                        draggableItem.parent = rootItem.findLayerRootByObjectName(draggableItem, "overlayLayerDraggableItem");

                        // Compute new position if needed
                        if (draggableItem.parent != null)
                        {
                            var newPosition = agentItem.mapToItem(parent, 0, 0);
                            draggableItem.x = newPosition.x;
                            draggableItem.y = newPosition.y;
                        }
                    }

                    onPositionChanged: {
                    }

                    onReleased: {
                        draggableItem.Drag.drop();

                        //
                        // Reset the position of our draggable item
                        //
                        // - restore our parent if needed
                        draggableItem.parent = agentItem;

                        // - restore our previous position in parent
                        draggableItem.x = 0;
                        draggableItem.y = 0;
                    }


                    AgentsListItem {
                        height: notDraggableItem.height
                        width: notDraggableItem.width

                        agent: model.QtObject
                        controller: rootItem.controller

                        agentItemIsHovered: mouseArea.containsMouse
                        visible: !mouseArea.drag.active

                        onNeedConfirmationtoDeleteAgent: {
                            deleteConfirmationPopup.open();
                        }

                        onConfigureFilesPaths: {
                            //console.log("Open 'Configure Files Paths' for " + agent.name);
                            agentFilesPathsPopup.agent = agent;
                            agentFilesPathsPopup.open();
                        }
                    }

                    AgentMapping.AgentNodeView {
                        isReduced: true
                        agentName: model.name
                        visible: mouseArea.drag.active
                        dropEnabled : false
                    }
                }
            }
        }
    }


    //
    // Popup about Delete Confirmation
    //
    Editor.DeleteConfirmationPopup {
        id: deleteConfirmationPopup

        confirmationText: "This agent is used in the platform.\nDo you want to completely delete it?"

        onDeleteConfirmed: {
            if (controller)
            {
                controller.deleteSelectedAgent();
            }
        }
    }


    //
    // Popup for Error messages
    //
    Popups.MessagePopup {
        id: popupErrorMessage

        anchors.centerIn: parent
    }


    //
    // Agent File Paths (Popup)
    //
    AgentFilesPaths {
        id: agentFilesPathsPopup

        anchors.centerIn: parent
    }
}
