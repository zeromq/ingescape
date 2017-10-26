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
 *      Justine Limoges    <limoges@ingenuity.io>
 *      Vincent Peyruqueou <peyruqueou@ingenuity.io>
 *
 */

import QtQuick 2.8
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4

import I2Quick 1.0

import MASTIC 1.0


// agent sub-directory
import "../agentsmapping" as AgentMapping


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

    //
    // List of agents
    //
    ListView {
        id: agentsList

        anchors {
            top: parent.top
            topMargin: 108
            bottom: parent.bottom
            left: parent.left
            right: parent.right
        }

        model: controller.agentsList

        delegate: componentAgentListItem

        /*onCurrentIndexChanged: {
            //console.log("onCurrentIndexChanged " + agentsList.currentIndex);
            console.log("onCurrentIndexChanged " + model.get(agentsList.currentIndex).name);
        }
        onCurrentItemChanged: {
            console.log("onCurrentItemChanged " + agentsList.currentItem);
        }*/

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


    //
    // Header
    //
    Rectangle {
        id: header

        anchors {
            top: parent.top
            left: parent.left
            right: parent.right
            bottom : agentsList.top
        }

        color : MasticTheme.selectedTabsBackgroundColor

        Row {
            id: headerRow

            height: btnAddAgent.height
            spacing : 8

            anchors {
                top: parent.top
                topMargin: 43
                left: parent.left
                leftMargin: 10
            }

            Button {
                id: btnAddAgent

                property var boundingBox: MasticTheme.svgFileMASTIC.boundsOnElement("creernouvelagent");
                height : boundingBox.height
                width :  boundingBox.width

                enabled:false

                anchors {
                    verticalCenter: parent.verticalCenter
                }

                style: I2SvgButtonStyle {
                    fileCache: MasticTheme.svgFileMASTIC

                    pressedID: releasedID + "-pressed"
                    releasedID: "creernouvelagent"
                    disabledID : releasedID + "-disabled"

                }

                onClicked: {
                    console.log("Nouvel Agent")
                    // TODO
                }
            }

            Rectangle {
                anchors {
                    verticalCenter: parent.verticalCenter
                }
                height : btnAddAgent.height
                width : 1
                color : MasticTheme.greyColor
            }

            Button {
                id: btnImportAgent

                property var boundingBox: MasticTheme.svgFileMASTIC.boundsOnElement("importer");
                height : boundingBox.height
                width :  boundingBox.width

                anchors {
                    verticalCenter: parent.verticalCenter
                }

                style: I2SvgButtonStyle {
                    fileCache: MasticTheme.svgFileMASTIC

                    pressedID: releasedID + "-pressed"
                    releasedID: "importer"
                    disabledID : releasedID + "-disabled"

                }

                onClicked: {
                    console.log("Importer Agent")
                    MasticEditorC.modelManager.importAgentFromSelectedFiles();
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
                id: btnExportAgent

                property var boundingBox: MasticTheme.svgFileMASTIC.boundsOnElement("exporter");
                height : boundingBox.height
                width :  boundingBox.width

                enabled: visible & (controller.selectedAgent ? true : false)

                anchors {
                    verticalCenter: parent.verticalCenter
                }

                style: I2SvgButtonStyle {
                    fileCache: MasticTheme.svgFileMASTIC

                    pressedID: releasedID + "-pressed"
                    releasedID: "exporter"
                    disabledID : releasedID + "-disabled"
                }

                onClicked: {
                    console.log("Exporter l'agent sélectionné");
                    if (controller.selectedAgent) {
                        //controller.exportAgent(controller.selectedAgent);
                    }
                }
            }


            Rectangle {
                anchors {
                    verticalCenter: parent.verticalCenter
                }
                height : btnRemoveAgent.height
                width : 1
                color : MasticTheme.greyColor
            }


            Button {
                id: btnRemoveAgent

                property var boundingBox: MasticTheme.svgFileMASTIC.boundsOnElement("supprimerplusieurs");
                height : boundingBox.height
                width :  boundingBox.width

                enabled: false

                anchors {
                    verticalCenter: parent.verticalCenter
                }

                style: I2SvgButtonStyle {
                    fileCache: MasticTheme.svgFileMASTIC

                    pressedID: releasedID + "-pressed"
                    releasedID: "supprimerplusieurs"
                    disabledID : releasedID + "-disabled"

                }

                onClicked: {
                    console.log("Supprimer Agent")
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
            bottom: agentsList.top
            left: parent.left
            right: parent.right
        }

        color: MasticTheme.leftPanelBackgroundColor

        height: 1
    }



    //
    // Visual representation of an agent in our list
    //
    Component {
        id: componentAgentListItem

        Item {
            id : agentItem

            width: MasticTheme.leftPanelWidth
            height: 85


            // Not Draggable Agent Item
            AgentsListItem {
                id : notDraggableItem

                anchors.fill : parent

                agent : model.QtObject
                controller: rootItem.controller

                visible: mouseArea.drag.active

                agentItemIsHovered : mouseArea.containsMouse
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

                MouseArea {
                    id: mouseArea

                    anchors.fill: draggableItem

                    hoverEnabled: true

                    drag.smoothed: false
                    drag.target: draggableItem
                    cursorShape: (mouseArea.drag.active)? Qt.ClosedHandCursor : Qt.PointingHandCursor //Qt.OpenHandCursor

                    onPressed: {
                        if (controller) {
                            controller.selectedAgent = model.QtObject;
                        }

                        // Find our layer and reparent our popup in it
                        draggableItem.parent = rootItem.findLayerRootByObjectName(draggableItem, "overlayLayer2");

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
                        // Check if we have a drop area below our item
                        if (draggableItem.Drag.target !== null)
                        {
                            var dropAreaElement = draggableItem.Drag.target;

                            if (typeof dropAreaElement.getDropCoordinates == 'function')
                            {
                                var dropPosition = dropAreaElement.getDropCoordinates();
                                console.log("Drop agent " + model.QtObject.name + " at "+ dropPosition);

                                if (MasticEditorC.agentsMappingC)
                                {
                                    MasticEditorC.agentsMappingC.addAgentDefinitionToMappingAtPosition(model.QtObject.name, model.QtObject.isON, model.QtObject.definition, dropPosition);
                                }
                            }
                            else
                            {
                                console.log("AgentsList: invalid DropArea to drop an agent");
                            }
                        }
                        else
                        {
                            console.log("AgentsList: agent dropped outside the mapping area");
                        }


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
                        height : notDraggableItem.height
                        width : notDraggableItem.width


                        agent : model.QtObject
                        controller: rootItem.controller

                        agentItemIsHovered : mouseArea.containsMouse
                        visible: !mouseArea.drag.active
                    }

                    AgentMapping.AgentNodeView {
                         opacity : 0.5
                         isReduced : true
                         agentName : model.name
                         visible: mouseArea.drag.active
                    }
                }

            }

        }

    }


}




