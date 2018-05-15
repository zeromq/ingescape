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
        onClicked:  {
            if(controller.selectedAgent)
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
                    releasedID: "creernouvelagent"
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
                    releasedID: "importer"
                    disabledID : releasedID + "-disabled"

                }

                onClicked: {
                    if (IngeScapeEditorC.modelManager) {
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

                enabled: visible & (controller.agentsList.count > 0 ? true : false)
                activeFocusOnPress: true

                anchors {
                    verticalCenter: parent.verticalCenter
                }

                style: Theme.LabellessSvgButtonStyle {
                    fileCache: IngeScapeTheme.svgFileINGESCAPE

                    pressedID: releasedID + "-pressed"
                    releasedID: "exporter"
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
                height : btnRemoveAgent.height
                width : 1
                color : IngeScapeTheme.blueGreyColor
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
                    releasedID: "supprimerplusieurs"
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
                Drag.keys: ["AgentsListItem"]

                MouseArea {
                    id: mouseArea

                    anchors.fill: draggableItem

                    hoverEnabled: true

                    drag.smoothed: false
                    drag.target: draggableItem
                    cursorShape: (mouseArea.drag.active)? Qt.ClosedHandCursor : Qt.OpenHandCursor

                    onPressed: {
                        if (controller) {
                            if(controller.selectedAgent === model.QtObject)
                            {
                                controller.selectedAgent = null;
                            } else {
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
                        height : notDraggableItem.height
                        width : notDraggableItem.width


                        agent : model.QtObject
                        controller: rootItem.controller

                        agentItemIsHovered : mouseArea.containsMouse
                        visible: !mouseArea.drag.active

                        onNeedConfirmationtoDeleteAgent : {
                            deleteConfirmationPopup.open();
                        }
                    }

                    AgentMapping.AgentNodeView {
                        isReduced : true
                        agentName : model.name
                        visible: mouseArea.drag.active
                        dropEnabled : false
                    }
                }
            }
        }
    }


    //
    // Delete Confirmation
    //
    Editor.DeleteConfirmationPopup {
        id : deleteConfirmationPopup

        confirmationText : "This agent is used in the platform.\nDo you want to completely delete it?"

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
    I2PopupBase {
        id: popupErrorMessage

        height: 150
        width: 350
        anchors.centerIn: parent

        isModal: true
        dismissOnOutsideTap : true
        keepRelativePositionToInitialParent : false

        Rectangle {

            anchors.fill: parent
            radius: 5
            border {
                width: 2
                color: IngeScapeTheme.editorsBackgroundBorderColor
            }
            color: IngeScapeTheme.editorsBackgroundColor

            Text {
                id: popupText

                text: "The file does not contain an agent definition !"

                anchors {
                    left : parent.left
                    right : parent.right
                    verticalCenter: parent.verticalCenter
                    verticalCenterOffset: -20
                }

                horizontalAlignment: Text.AlignHCenter
                lineHeight: 24
                lineHeightMode: Text.FixedHeight
                color: IngeScapeTheme.whiteColor

                font {
                    family: IngeScapeTheme.textFontFamily
                    pixelSize: 16
                }
            }

            Button {
                anchors {
                    horizontalCenter: parent.horizontalCenter
                    bottom : parent.bottom
                    bottomMargin: 16
                }

                property var boundingBox: IngeScapeTheme.svgFileINGESCAPE.boundsOnElement("button");
                height: boundingBox.height
                width:  boundingBox.width

                activeFocusOnPress: true
                text: "OK"

                style: I2SvgButtonStyle {
                    fileCache: IngeScapeTheme.svgFileINGESCAPE

                    pressedID: releasedID + "-pressed"
                    releasedID: "button"
                    disabledID : releasedID

                    font {
                        family: IngeScapeTheme.textFontFamily
                        weight : Font.Medium
                        pixelSize : 16
                    }
                    labelColorPressed: IngeScapeTheme.blackColor
                    labelColorReleased: IngeScapeTheme.whiteColor
                    labelColorDisabled: IngeScapeTheme.whiteColor
                }

                onClicked: {
                    // Close our popup
                    popupErrorMessage.close();
                }
            }
        }
    }
}