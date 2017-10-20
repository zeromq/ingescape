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
            topMargin: 110
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
            id: headerRow1

            height: 30

            anchors {
                top: parent.top
                left: parent.left
                right: parent.right
            }

            Text {
                id: txtSearch

                text: qsTr("Rechercher...")

                color: MasticTheme.agentsListLabelColor

                font: MasticTheme.normalFont
            }

            Text {
                id: txtFilter

                text: qsTr("Filtrer...")

                color:MasticTheme.agentsListLabelColor

                font: MasticTheme.normalFont
            }
        }

        Row {
            id: headerRow2

            height: 30

            anchors {
                top: headerRow1.bottom
                left: parent.left
                right: parent.right
            }

            Button {
                id: btnAddAgent
                text: qsTr("Nouveau")
                onClicked: {
                    console.log("Nouvel Agent")
                    // TODO
                }
            }

            Button {
                id: btnImportAgent
                text: qsTr("Importer")
                onClicked: {
                    console.log("Importer Agent")
                    //controller.importAgent();
                }
            }

            Button {
                id: btnExportAgent
                text: qsTr("Exporter")
                enabled: (controller.selectedAgent ? true : false)

                onClicked: {
                    console.log("Exporter l'agent sélectionné");
                    if (controller.selectedAgent) {
                        //controller.exportAgent(controller.selectedAgent);
                    }
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
                Drag.hotSpot.y: agentItem.height

                MouseArea {
                    id: mouseArea

                    anchors.fill: draggableItem

                    hoverEnabled: true

                    drag.smoothed: false
                    drag.target: draggableItem

                    cursorShape: (mouseArea.drag.active)? Qt.ClosedHandCursor : Qt.PointingHandCursor //((mouseArea.pressed) ?  : Qt.OpenHandCursor

                    onPressed: {
                        if (controller) {
                            controller.selectedAgent = model.QtObject;
                        }

                        // Find our layer and reparent our popup in it
                        draggableItem.parent = rootItem.findLayerRootByObjectName(draggableItem, "overlayLayer");

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
                                    MasticEditorC.agentsMappingC.addAgentDefinitionToMappingAtPosition(model.QtObject.name, model.QtObject.definition, dropPosition);
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
                }

                AgentsListItem {
                    anchors.fill: draggableItem

                    agent : model.QtObject

                    controller: rootItem.controller
                }

            }

        }




    }


    //                    //                    Button {
    //                    //                        id: btnDeleteAgent

    //                    //                        anchors {
    //                    //                            left: agentRow.right
    //                    //                            top: agentRow.top
    //                    //                        }

    //                    //                        visible: (agentListItem.ListView.isCurrentItem && (model.status === AgentStatus.OFF))

    //                    //                        text: "X"

    //                    //                        onClicked: {
    //                    //                            if (controller)
    //                    //                            {
    //                    //                                // Delete our agent
    //                    //                                controller.deleteAgent(model.QtObject);
    //                    //                            }
    //                    //                        }
    //                    //                    }




    //                    //                        Text {
    //                    //                            //text: model.models ? model.models.count + " clone(s)" : ""
    //                    //                            //visible: model.models && (model.models.count > 1)
    //                    //                            text: (model && model.models) ? model.models.count + " clone(s)" : ""
    //                    //                            visible: (model && model.models) ? (model.models.count > 1) : false

    //                    //                            color: "red"
    //                    //                            font: MasticTheme.normalFont
    //                    //                        }

    //                    //                        Text {
    //                    //                            id: agentStatus
    //                    //                            text: "Status: " + AgentStatus.enumToString(model.status)
    //                    //                            visible: !model.hasOnlyDefinition

    //                    //                            height: 25
    //                    //                            color: MasticTheme.agentsListLabelColor
    //                    //                            font: MasticTheme.normalFont
    //                    //                        }


    //                    //                    Column {
    //                    //                        width: 175
    //                    //                        anchors {
    //                    //                            top: parent.top
    //                    //                            topMargin: 30
    //                    //                            bottom: parent.bottom
    //                    //                            right: parent.right
    //                    //                        }

    //                    //                        Text {
    //                    //                            text: "Variante"
    //                    //                            visible: model.definition ? model.definition.isVariant : false

    //                    //                            height: 25
    //                    //                            color: "red"
    //                    //                            font: MasticTheme.normalFont
    //                    //                        }

    //                    //                        Text {
    //                    //                            text: model.definition ? model.definition.description : ""

    //                    //                            width: 175
    //                    //                            elide: Text.ElideRight

    //                    //                            height: 25
    //                    //                            color: MasticTheme.agentsListLabelColor
    //                    //                            font: MasticTheme.normalFont
    //                    //                        }

    //                    // }


    //                    //                    Switch {
    //                    //                        checked: (model.status === AgentStatus.ON)
    //                    //                        visible: !model.hasOnlyDefinition

    //                    //                        anchors {
    //                    //                            left: agentRow.left
    //                    //                            leftMargin: 2
    //                    //                            bottom: agentRow.bottom
    //                    //                            bottomMargin: 5
    //                    //                        }
    //                    //                    }

    //                    Row {
    //                        visible: !model.hasOnlyDefinition && (model.status === AgentStatus.ON)

    //                        anchors {
    //                            right: agentRow.right
    //                            bottom: agentRow.bottom
    //                        }

    //                        Button {
    //                            id: btnMuteAll
    //                            text: model.isMuted ? "UN-mute all" : "Mute all"

    //                            width: 110

    //                            onClicked: {
    //                                model.QtObject.updateMuteAllOutputs(!model.isMuted);
    //                            }
    //                        }

    //                        Button {
    //                            id: btnFreeze
    //                            text: model.isFrozen ? "UN-freeze" : "Freeze"

    //                            visible: model.canBeFrozen

    //                            width: 90

    //                            onClicked: {
    //                                model.QtObject.updateFreeze(!model.isFrozen);
    //                            }
    //                        }
    //                    }
    //                }
    //            }
    //        }


}




