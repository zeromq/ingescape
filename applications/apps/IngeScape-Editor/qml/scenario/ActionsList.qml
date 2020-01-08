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
 *      Vincent Peyruqueou <peyruqueou@ingenuity.io>
 *      Alexandre Lemort   <lemort@ingenuity.io>
 *
 */

import QtQuick 2.8
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4
import QtQuick.Window 2.3

import I2Quick 1.0

// parent-directory
import ".." as Editor;

import INGESCAPE 1.0
import "../theme" as Theme
import "../popup" as Popup

Item {
    id: rootItem

    anchors.fill: parent

    //--------------------------------
    //
    // Properties
    //
    //--------------------------------

    // Controller associated to our view
    property AbstractScenarioController scenarioController: null;

    // Licenses controller
    property LicensesController licensesController: IngeScapeEditorC.licensesC;

    // Flag indicating if the user have a valid license for the editor
    property bool isEditorLicenseValid: mainWindow.licensesController && mainWindow.licensesController.mergedLicense && mainWindow.licensesController.mergedLicense.editorLicenseValidity

    // Flag indicating if the user wants to organizate actions list or manipulate actions in editor views
    property bool organizeActionMode: false


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

        while ((currentObject !== null) && (layerRoot === null))
        {
            var index = 0;
            while ((index < currentObject.data.length) && (layerRoot === null))
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
    // Signals
    //
    //--------------------------------

    // Signal emitted when the user tries to perform an action forbidden by the license
    signal unlicensedAction();


    //--------------------------------
    //
    // Content
    //
    //--------------------------------

    // allowing to deselect selected action
    MouseArea {
        anchors.fill: parent

        onClicked: {
            if (scenarioController.selectedAction)
            {
                scenarioController.selectedAction = null;
            }
        }
    }


    //
    // List of actions
    //
    ScrollView {
        id : actionsListScrollView

        anchors {
            top: parent.top
            topMargin: 78
            bottom: parent.bottom
            left: parent.left
            right: parent.right
        }

        style: IngeScapeScrollViewStyle {
        }

        // Prevent drag overshoot on Windows
        flickableItem.boundsBehavior: Flickable.OvershootBounds

        // Content of our item
        ListView {
            id: actionsList

            model: scenarioController ? scenarioController.actionsList : 0

            delegate: componentActionsListItem


            //
            // Transition animations
            //
            add: Transition {
                NumberAnimation { property: "opacity"; from: 0.0; to: 1.0 }
                NumberAnimation { property: "scale"; from: 0.0; to: 1.0 }
            }

//            displaced: Transition {
//                NumberAnimation { properties: "x,y"; easing.type: Easing.OutBounce }

//                // ensure opacity and scale values return to 1.0
//                NumberAnimation { property: "opacity"; to: 1.0 }
//                NumberAnimation { property: "scale"; to: 1.0 }
//            }

//            move: Transition {
//                NumberAnimation { properties: "x,y"; easing.type: Easing.OutBounce }

//                // ensure opacity and scale values return to 1.0
//                NumberAnimation { property: "opacity"; to: 1.0 }
//                NumberAnimation { property: "scale"; to: 1.0 }
//            }

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
            bottom : actionsListScrollView.top
        }

        color : IngeScapeEditorTheme.selectedTabsBackgroundColor


        Row {
            id: headerRow

            height: btnAddAction.height
            spacing : 8

            anchors {
                top: parent.top
                topMargin: 23
                left: parent.left
                leftMargin: 10
            }

            LabellessSvgButton {
                id: btnAddAction

                anchors {
                    verticalCenter: parent.verticalCenter
                }

                enabled: (IgsModelManager && (IgsModelManager.allAgentsGroupsByName.count > 0))

                fileCache: IngeScapeEditorTheme.svgFileIngeScapeEditor

                pressedID: releasedID + "-pressed"
                releasedID: "new-agent"
                disabledID : releasedID + "-disabled"


                onClicked: {
                    if (!rootItem.isEditorLicenseValid
                            && rootItem.scenarioController && rootItem.scenarioController.actionsList && rootItem.scenarioController.actionsList.count >= 1)
                    {
                        rootItem.unlicensedAction();
                    }
                    else
                    {
                        scenarioController.openActionEditorWithModel(null);
                    }
                }
            }
        }

        // TODO design button and show it in Editor
//        Row {
//            id: headerRowOrganize

//            height: btnOrganizeActions.height
//            spacing : 8

//            anchors {
//                top: parent.top
//                topMargin: 23
//                left: headerRow.right
//                leftMargin: 10
//            }

//            Button {
//                id: btnOrganizeActions

//                anchors {
//                    verticalCenter: parent.verticalCenter
//                }

//                enabled: scenarioController ? scenarioController.actionsList.count > 1 : false

//                text: rootItem.organizeActionMode ? "Disable Organize Mode" : "Enable Organize Mode"

//                onClicked: {
//                    rootItem.organizeActionMode = !rootItem.organizeActionMode
//                }
//            }
//        }

        Row {
            id: headerRow2

            height: btnAddAction.height
            spacing : 8

            anchors {
                verticalCenter: headerRow.verticalCenter
                right : parent.right
                rightMargin: 10
            }

            LabellessSvgButton {
                id: btnRemoveAction

                anchors {
                    verticalCenter: parent.verticalCenter
                }

                enabled: false

                fileCache: IngeScapeEditorTheme.svgFileIngeScapeEditor

                pressedID: releasedID + "-pressed"
                releasedID: "delete-all"
                disabledID : releasedID + "-disabled"


                onClicked: {
                    console.log("Supprimer Action")
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
            bottom: actionsListScrollView.top
            left: parent.left
            right: parent.right
        }

        color: IngeScapeTheme.blackColor

        height: 1
    }


    //
    // Visual representation of an action in our list
    //
    Component {
        id: componentActionsListItem

        // Set content action list item draggable
        Editor.DraggableListItem {
            dragEnable: organizeActionMode // Only when we are in organize action list mode

            keysDragNDrop: ["OrganizeActionItem"]

            // Reorganize actions list model when an item is moved
            onMoveItemRequested: {
                scenarioController.actionsList.move(from, to, 1);
            }

            Item {
                id : actionListItem

                width: IngeScapeEditorTheme.leftPanelWidth
                height: 42

                // Not Draggable Action Item
                ActionsListItem {
                    id : notDraggableItem

                    anchors.fill : parent

                    action : model.QtObject
                    controller: rootItem.scenarioController
                    agentsMappingController: IngeScapeEditorC.agentsMappingC

                    // visible: mouseArea.drag.active

                    actionItemIsHovered: mouseArea.containsMouse /*|| dragArea.containsMouse*/
                    actionItemIsPressed: mouseArea.pressed /*|| dragArea.pressed*/
                }

                // Draggable Action Item
                Item {
                    id : draggableItem

                    height : notDraggableItem.height
                    width : notDraggableItem.width

                    // Reference to our action that can be used by a DropArea item
                    property var action: model.QtObject

                    // StartTime of the action when it is dragged in timeline
                    property var temporaryStartTime: null;

                    Drag.active: mouseArea.drag.active
                    Drag.hotSpot.x: mouseArea.mouseX
                    Drag.hotSpot.y: mouseArea.mouseY
                    Drag.keys: ["ActionsListItem"]

                    MouseArea {
                        id: mouseArea

                        anchors.fill: draggableItem

                        enabled: !rootItem.organizeActionMode

                        hoverEnabled: true

                        drag.smoothed: false
                        drag.target: rootItem.organizeActionMode ? undefined : draggableItem
                        cursorShape: mouseArea.drag.active ? Qt.PointingHandCursor : Qt.OpenHandCursor

                        onPressed: {
                            if (scenarioController)
                            {
                                if (scenarioController.selectedAction === model.QtObject) {
                                    scenarioController.selectedAction = null;
                                }
                                else {
                                    scenarioController.selectedAction = model.QtObject;
                                }
                            }

                            // Find our layer and reparent our popup in it
                            draggableItem.parent = rootItem.findLayerRootByObjectName(draggableItem, "overlayLayerDraggableItem");

                            // Compute new position if needed
                            if (draggableItem.parent != null)
                            {
                                var newPosition = actionListItem.mapToItem(parent, 0, 0);
                                draggableItem.x = newPosition.x;
                                draggableItem.y = newPosition.y;
                            }
                        }

                        onPositionChanged: {
                            //itemDragged.x = mouseX - 12 - itemDragged.width;
                            //itemDragged.y = mouseY - 12 - itemDragged.height;
                            itemDragged.x = mouseX - itemDragged.width / 2.0;
                            itemDragged.y = mouseY - itemDragged.height - 20;
                        }

                        onCanceled: {
                            draggableItem.Drag.drop();

                            //
                            // Reset the position of our draggable item
                            //
                            // - restore our parent if needed
                            draggableItem.parent = actionListItem;

                            // - restore our previous position in parent
                            draggableItem.x = 0;
                            draggableItem.y = 0;
                        }

                        onReleased: {
                            draggableItem.Drag.drop();

                            //
                            // Reset the position of our draggable item
                            //
                            // - restore our parent if needed
                            draggableItem.parent = actionListItem;

                            // - restore our previous position in parent
                            draggableItem.x = 0;
                            draggableItem.y = 0;
                        }


                        ActionsListItem {
                            height: notDraggableItem.height
                            width: notDraggableItem.width

                            action: model.QtObject
                            controller: rootItem.scenarioController
                            agentsMappingController: IngeScapeEditorC.agentsMappingC

                            actionItemIsHovered: mouseArea.containsMouse /*|| dragArea.containsMouse*/
                            actionItemIsPressed: mouseArea.pressed /*|| dragArea.pressed*/

                            visible: !mouseArea.drag.active

                            isLicenseValid: rootItem.isEditorLicenseValid;

                            //
                            // Slot on signal "On Unlicensed Action"
                            //
                            onUnlicensedAction : {
                                // Transmit signal
                                rootItem.unlicensedAction();
                            }

                            //
                            // Slot on signal "Need Confirmation to Delete Action"
                            //
                            onNeedConfirmationToDeleteAction: {
                                deleteConfirmationPopup.myAction = action;
                                deleteConfirmationPopup.open();
                            }
                        }

                        I2CustomRectangle {
                            id: itemDragged

                            width: Math.max(IngeScapeTheme.timeWidth, nameAction.width + 10)
                            //width: Math.min(Math.max(IngeScapeTheme.timeWidth, nameAction.width + 10), 250)
                            height: columnText.height + 8

                            color: IngeScapeTheme.darkBlueGreyColor

                            visible: mouseArea.drag.active

                            // - fuzzy radius around our rectangle
                            fuzzyRadius: 2
                            fuzzyColor : IngeScapeTheme.blackColor

                            Column {
                                id: columnText

                                anchors.centerIn: parent
                                height: temporaryStartTimeAction.visible ? (nameAction.height + temporaryStartTimeAction.height + 3)
                                                                         : nameAction.height

                                spacing: 6

                                Text {
                                    id: nameAction

                                    anchors {
                                        horizontalCenter: parent.horizontalCenter
                                        //left: parent.left
                                        //right: parent.right
                                    }
                                    horizontalAlignment: Text.AlignHCenter

                                    text: model.name
                                    //elide: Text.ElideRight

                                    color: IngeScapeTheme.lightGreyColor
                                    font {
                                        family : IngeScapeTheme.textFontFamily
                                        pixelSize: 14
                                    }
                                }

                                Text {
                                    id: temporaryStartTimeAction

                                    anchors.horizontalCenter: parent.horizontalCenter
                                    horizontalAlignment: Text.AlignHCenter

                                    visible: (text !== "")
                                    text: draggableItem.temporaryStartTime ? draggableItem.temporaryStartTime.toLocaleString(Qt.locale(), "HH:mm:ss.zzz")
                                                                           : ""

                                    color: IngeScapeTheme.lightGreyColor
                                    font {
                                        family : IngeScapeTheme.textFontFamily
                                        pixelSize: 14
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }


    //
    // Delete Confirmation
    //
    ConfirmationPopup {
        id: deleteConfirmationPopup

        property var myAction: null

        confirmationText: "This action is used in the platform.\nDo you want to completely delete it?"

        onConfirmed: {
            if (myAction && scenarioController) {
                // Delete our action
                scenarioController.deleteAction(myAction);
            }
        }
    }

    Popup.MessagePopup {
        id: noMoreActionMessage
        anchors.centerIn: parent

        width: 300

        message: qsTr("The editor has no valid license.\nYou cannot create any more actions.")
    }
}
