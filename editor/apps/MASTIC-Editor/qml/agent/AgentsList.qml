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
 *      Vincent Peyruqueou <peyruqueou@ingenuity.io>
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
                text: qsTr("Nouvel Agent")
                onClicked: {
                    console.log("Nouvel Agent")
                    // TODO
                }
            }

            Text {
                text: qsTr("Importer...")

                color: MasticTheme.agentsListLabelColor

                font: MasticTheme.normalFont
            }

            Text {
                text: qsTr("Exporter...")

                color: MasticTheme.agentsListLabelColor

                font: MasticTheme.normalFont
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
            }


            // Draggable Agent Item
            Item {
                id : draggableItem
                height : notDraggableItem.height
                width : notDraggableItem.width

                Drag.active: mouseArea.drag.active
                Drag.hotSpot.x: 0
                Drag.hotSpot.y: (agentItem.height/2)

                MouseArea {
                    id: mouseArea

                    property  var beginPositionX : null;
                    property  var beginPositionY : null;

                    anchors.fill: draggableItem
                    hoverEnabled: true
                    drag.smoothed: false
                    drag.target: draggableItem
                    cursorShape: (mouseArea.drag.active)? Qt.ClosedHandCursor : Qt.PointingHandCursor //((mouseArea.pressed) ?  : Qt.OpenHandCursor

                    onPressed: {
                        if (controller) {
                            controller.selectedAgent = model.QtObject;
                        }

                        beginPositionX  = draggableItem.x;
                        beginPositionY  = draggableItem.y;

                        // Find our layer and reparent our popup in it
                        draggableItem.parent = rootItem.findLayerRootByObjectName(draggableItem, "overlayLayer");

                        // Compute new position if needed
                        if ((draggableItem.parent != null))
                        {
                            // NB: Repositionning does not work if some anchors (e.g. anchors.fill: parent) are used
                            var newPosition = agentItem.mapToItem(parent, beginPositionX, beginPositionY);
                            draggableItem.x = newPosition.x;
                            draggableItem.y = newPosition.y;
                        }

                        if (draggableItem.parent != null)
                        {
                            if (typeof draggableItem.parent.addContentItem == 'function')
                            {
                                draggableItem.parent.addContentItem(draggableItem);
                            }
                        }

                    }

                    onPositionChanged: {
                    }


                    onReleased: {
                        if( draggableItem.Drag.target !== null)
                        {
                            // Drop Event
                            var dropElement = draggableItem.Drag.target;

                            // Convert x, y coordinate to the target Item
                            var newPos = mouseArea.mapToItem(dropElement, 0, 0)

                        }

                        //reset the position when the drop target is undefined

                        // Indicate that our popup is no more the current content of our layer
                        if (typeof draggableItem.parent.removeContentItem == 'function')
                        {
                            draggableItem.parent.removeContentItem(draggableItem);
                        }

                        // Restore our parent if needed
                        draggableItem.parent = agentItem;

                        // Restore our previous position in parent if needed
                        draggableItem.x = beginPositionX;
                        draggableItem.y = beginPositionY;
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




