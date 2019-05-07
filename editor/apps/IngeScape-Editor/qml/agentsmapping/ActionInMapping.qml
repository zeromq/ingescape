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
 *      Vincent Peyruqueou  <peyruqueou@ingenuity.io>
 *
 */

import QtQuick 2.8
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4
//import QtQuick.Controls 2.0 as Controls2

import I2Quick 1.0

import INGESCAPE 1.0

import "../theme" as Theme;


Rectangle {
    id: rootItem


    //--------------------------------
    //
    // Properties
    //
    //--------------------------------
    // Controller associated to our view
    property var controller : null;

    // Model associated to our QML item
    property ActionInMappingVM actionInMappingVM: null;

    property var actionM: actionInMappingVM ? actionInMappingVM.action : null;

    // false if the agent is dropping and the drop is not available, true otherwise
    //property bool dropEnabled: true

    // Flag indicating if the mouse is hover our agent
    property bool actionItemIsHovered: mouseArea.containsMouse

    // To check if our item is selected or not
    property bool _isSelected: (controller && rootItem.actionInMappingVM && (controller.selectedAction === rootItem.actionInMappingVM))

    width: 150
    height: width
    radius: width / 2


    // Init position of our agent
    x: (actionInMappingVM && actionInMappingVM.position) ? actionInMappingVM.position.x : 0
    y: (actionInMappingVM && actionInMappingVM.position) ? actionInMappingVM.position.y : 0


    color: mouseArea.pressed ? IngeScapeTheme.darkGreyColor2
                             : IngeScapeTheme.darkBlueGreyColor


    border {
        color: IngeScapeTheme.selectionColor
        width: rootItem._isSelected ? 1 : 0
    }


    //--------------------------------
    //
    // Behaviors
    //
    //--------------------------------

    //
    // Bindings to save the position of our agent when we move it (drag-n-drop)
    //
    Binding {
        target: rootItem.actionInMappingVM
        property: "position"
        value: Qt.point(rootItem.x, rootItem.y)
    }


    // Animate our border when our item is selected / unselected
    Behavior on border.width {
        NumberAnimation {}
    }


    //--------------------------------
    //
    // Signals
    //
    //--------------------------------



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

    MouseArea {
        id: mouseArea

        anchors.fill: parent

        hoverEnabled: true

        drag.target: parent
        // Disable smoothed so that the Item pixel from where we started the drag remains under the mouse cursor
        drag.smoothed: false


        onPressed: {
            // bring our agent to front
            parent.z = rootItem.parent.maxZ++;
        }

        onClicked: {
            if (controller && actionInMappingVM) {
                if (controller.selectedAction === actionInMappingVM)
                {
                    controller.selectedAction = null;
                }
                else {
                    controller.selectedAction = actionInMappingVM;
                }
            }
        }


        //------------------------------------------
        //
        // Input and output slots
        //
        //------------------------------------------


        Item {
            id: inputOutputSlotSection

            anchors {
                left: parent.left
                right: parent.right

                top: parent.top
                //topMargin: 5
                bottom: parent.bottom
            }


            //
            // Inlets / Input slots
            //
            Column {
                id: columnInputSlots

                anchors {
                    fill: parent
                }

                Repeater {
                    // List of intput slots VM
                    model: 1 // rootItem.actionInMappingVM ? rootItem.actionInMappingVM.linkInputsList : null

                    delegate: Item {
                        id: inputSlotItem

                        property var myModel: model.QtObject

                        height: 22
                        anchors {
                            left: parent.left
                            right: parent.right
                        }

                        Rectangle {
                            id: draggablePointFROM

                            height: linkPoint.height
                            width: height
                            radius: height/2

                            property bool dragActive: mouseAreaPointFROM.drag.active;
                            property var agentInMappingVMOfInput: rootItem.actionInMappingVM;
                            property var inputSlotModel: model.QtObject

                            Drag.active: mouseAreaPointFROM.drag.active;
                            Drag.hotSpot.x: width/2
                            Drag.hotSpot.y: height/2
                            Drag.keys: ["InputSlotItem"]

                            border {
                                width: 1
                                color: draggablePointFROM.dragActive ? linkPoint.color : "transparent"
                            }

                            color: draggablePointFROM.dragActive ? IngeScapeTheme.blackColor : "transparent"

                            parent: draggablePointFROM.dragActive ? rootItem.parent  : linkPoint

                            MouseArea {
                                id: mouseAreaPointFROM

                                anchors.fill: parent

                                drag.target: parent
                                // Disable smoothed so that the Item pixel from where we started
                                // the drag remains under the mouse cursor
                                drag.smoothed: false

                                hoverEnabled: true

                                cursorShape: draggablePointFROM.dragActive ? Qt.ClosedHandCursor : Qt.PointingHandCursor

                                onPressed: {
                                    draggablePointFROM.z = rootItem.parent.maxZ++;
                                    linkDraggablePoint.z = rootItem.parent.maxZ++;
                                }

                                onReleased: {
                                    // Drop our item
                                    draggablePointFROM.Drag.drop();

                                    // replace the draggablePointTO
                                    draggablePointFROM.x = 0
                                    draggablePointFROM.y = 0
                                }
                            }

                            Link {
                                id: linkDraggablePoint

                                parent: rootItem.parent

                                strokeDashArray: "5, 5"
                                visible: draggablePointFROM.dragActive

                                secondPoint: Qt.point(myModel.position.x, myModel.position.y)
                                firstPoint: Qt.point(draggablePointFROM.x + draggablePointFROM.width, draggablePointFROM.y + draggablePointFROM.height/2)

                                defaultColor:linkPoint.color
                            }
                        }


                        Rectangle {
                            id: linkPoint

                            anchors {
                                horizontalCenter: parent.left
                                verticalCenter: parent.verticalCenter
                            }

                            height: 15
                            width: height
                            radius: height/2

                            border {
                                width: 0
                                color: IngeScapeTheme.whiteColor
                            }
                            color: IngeScapeEditorTheme.purpleColor
                        }


                        DropArea {
                            id: inputDropArea

                            anchors {
                                fill: linkPoint
                                margins: -3
                            }

                            // Only accept drag events from output slot items
                            keys: ["OutputSlotItem"]

                            onEntered: {
                                if (drag.source !== null)
                                {
                                    var dragItem = drag.source;

                                    if (typeof dragItem.dragActive !== 'undefined' && dragItem.outputSlotModel.canLinkWith(inputSlotItem.myModel))
                                    {
                                        dragItem.color = dragItem.border.color;
                                        linkPoint.border.width = 2
                                        linkPoint.scale = 1.2
                                    }
                                    else
                                    {
                                        console.log("inputDropArea: no dragActive "+dragItem.agent)
                                    }
                                }
                                else
                                {
                                    console.log("inputDropArea: no source "+ drag.source)
                                }
                            }


                            onExited: {
                                var dragItem = drag.source;
                                if (typeof dragItem.dragActive !== 'undefined')
                                {
                                    dragItem.color = "transparent";
                                    linkPoint.border.width = 0
                                    linkPoint.scale = 1
                                }
                            }


                            onDropped: {
                                var dragItem = drag.source;
                                if (dragItem)
                                {
                                    if ((typeof dragItem.outputSlotModel !== 'undefined') && dragItem.agentInMappingVMOfOutput && rootItem.actionInMappingVM)
                                    {
                                        dragItem.color = "transparent";
                                        linkPoint.border.width = 0
                                        linkPoint.scale = 1

                                        console.log("inputDropArea: create a link from " + dragItem.outputSlotModel + " to " + inputSlotItem.myModel);
                                        //controller.dropLinkBetweenTwoAgents(dragItem.agentInMappingVMOfOutput, dragItem.outputSlotModel, rootItem.actionInMappingVM, inputSlotItem.myModel);
                                    }
                                }
                            }

                        }



                        //
                        // Bindings to save the anchor point of our input slot
                        // i.e. the point used to draw a link
                        //
                        Binding {
                            target: myModel

                            property: "position"

                            value: Qt.point(rootItem.x + inputOutputSlotSection.x + columnInputSlots.x + inputSlotItem.x + linkPoint.x + linkPoint.width/2,
                                            rootItem.y + inputOutputSlotSection.y + columnInputSlots.y + inputSlotItem.y + linkPoint.y + linkPoint.height/2)
                        }
                    }
                }
            }



            //
            //
            // Outlets / Output slots
            //
            Column {
                id: columnOutputSlots

                anchors {
                    fill: parent
                }


                Repeater {
                    // List of output slots VM
                    model: 1 // rootItem.actionInMappingVM ? rootItem.actionInMappingVM.linkOutputsList : null

                    delegate: Item {
                        id: outputSlotItem

                        property var myModel: model.QtObject

                        height: 22
                        anchors {
                            left: parent.left
                            right: parent.right
                        }

                        Rectangle {
                            id : draggablePointTO

                            height: linkPointOut.height
                            width: height
                            radius: height/2

                            border {
                                width : 1
                                color : draggablePointTO.dragActive ? linkPointOut.color : "transparent"
                            }

                            property bool dragActive : mouseAreaPointTO.drag.active;
                            property var agentInMappingVMOfOutput : rootItem.actionInMappingVM
                            property var outputSlotModel: model.QtObject

                            Drag.active: draggablePointTO.dragActive;
                            Drag.hotSpot.x: width/2
                            Drag.hotSpot.y: height/2
                            Drag.keys: ["OutputSlotItem"]

                            color: draggablePointTO.dragActive ? IngeScapeEditorTheme.agentsMappingBackgroundColor : "transparent"
                            parent: draggablePointTO.dragActive ? rootItem.parent  : linkPointOut

                            MouseArea {
                                id: mouseAreaPointTO

                                anchors.fill: parent

                                drag.target: parent
                                // Disable smoothed so that the Item pixel from where we started
                                // the drag remains under the mouse cursor
                                drag.smoothed: false

                                hoverEnabled: true

                                cursorShape: draggablePointTO.dragActive ? Qt.ClosedHandCursor : Qt.PointingHandCursor

                                onPressed: {
                                    draggablePointTO.z = rootItem.parent.maxZ++;
                                    linkDraggablePointTO.z = rootItem.parent.maxZ++;
                                }

                                onReleased: {
                                    // Drop our item
                                    draggablePointTO.Drag.drop();

                                    // replace the draggablePointTO
                                    draggablePointTO.x = 0
                                    draggablePointTO.y = 0
                                }
                            }



                            Link {
                                id: linkDraggablePointTO

                                parent: rootItem.parent

                                strokeDashArray: "5, 5"
                                visible: draggablePointTO.dragActive

                                firstPoint: Qt.point(myModel.position.x, myModel.position.y)
                                secondPoint: Qt.point(draggablePointTO.x, draggablePointTO.y + draggablePointTO.height/2)

                                defaultColor:linkPointOut.color
                            }
                        }



                        Rectangle {
                            id: linkPointOut

                            anchors {
                                horizontalCenter: parent.right
                                verticalCenter: parent.verticalCenter
                            }

                            height: 15
                            width: height
                            radius: height/2

                            border {
                                width: 0
                                color: IngeScapeTheme.whiteColor
                            }
                            color: IngeScapeEditorTheme.purpleColor
                        }



                        DropArea {
                            id: outputDropArea

                            anchors {
                                fill: linkPointOut
                                margins: -3
                            }

                            // Only accept drag events from input slot items
                            keys: ["InputSlotItem"]

                            onEntered: {
                                if (drag.source !== null)
                                {
                                    var dragItem = drag.source;

                                    if (typeof dragItem.dragActive !== 'undefined'  && outputSlotItem.myModel.canLinkWith(dragItem.inputSlotModel))
                                    {
                                        dragItem.color = dragItem.border.color;
                                        linkPointOut.border.width = 2
                                        linkPointOut.scale = 1.2
                                    }
                                    else
                                    {
                                        console.log("outputDropArea: no dragActive "+dragItem.agent)
                                    }
                                }
                                else
                                {
                                    console.log("outputDropArea: no source "+ drag.source)
                                }
                            }


                            onExited: {
                                var dragItem = drag.source;
                                if (typeof dragItem.dragActive !== 'undefined')
                                {
                                    dragItem.color = "transparent";
                                    linkPointOut.border.width = 0
                                    linkPointOut.scale = 1
                                }
                            }

                            onDropped: {
                                var dragItem = drag.source;
                                if (dragItem)
                                {
                                    if (typeof dragItem.inputSlotModel !== 'undefined' && controller && rootItem.actionInMappingVM && outputSlotItem.myModel)
                                    {
                                        dragItem.color = "transparent";
                                        linkPointOut.border.width = 0
                                        linkPointOut.scale = 1

                                        console.log("outputDropArea: create a link from " + outputSlotItem.myModel + " to " + dragItem.inputSlotModel);
                                        //controller.dropLinkBetweenTwoAgents(rootItem.actionInMappingVM, outputSlotItem.myModel, dragItem.agentInMappingVMOfInput, dragItem.inputSlotModel);
                                    }
                                }
                            }
                        }



                        //
                        // Bindings to save the anchor point of our input slot
                        // i.e. the point used to draw a link
                        //
                        Binding {
                            target: myModel

                            property: "position"

                            value: Qt.point(rootItem.x + inputOutputSlotSection.x + columnOutputSlots.x + outputSlotItem.x + linkPointOut.x + linkPointOut.width/2,
                                            rootItem.y + inputOutputSlotSection.y + columnOutputSlots.y + outputSlotItem.y + linkPointOut.y + linkPointOut.height/2)
                        }
                    }
                }
            }
        }


        //------------------------------------------
        //
        // Header
        //
        //------------------------------------------

        // Button to open the definition
        /*Button {
            id: btnOpenDefinition

            anchors {
                top: parent.top
                topMargin: 10
                left: parent.left
                leftMargin: 10
            }

            style: LabellessSvgButtonStyle {
                fileCache: IngeScapeEditorTheme.svgFileIngeScapeEditor

                pressedID: releasedID + "-pressed"
                releasedID: "definition-button"
                disabledID : releasedID
            }

            opacity: rootItem.actionItemIsHovered ? 1 : 0

            visible: (opacity !== 0)
            enabled: visible

            Behavior on opacity {
                NumberAnimation {}
            }

            onClicked: {
                // Open the definition(s)
                rootItem.agentsGroupedByName.openDefinition();
            }
        }*/


        // Action Name
        Text {
            id: txtActionName

            anchors {
                left: parent.left
                leftMargin: 30
                right: btnRemoveFromMapping.left
                top: parent.top
                topMargin: 10
            }

            elide: Text.ElideRight
            text: rootItem.actionInMappingVM ? rootItem.actionInMappingVM.name : "";
            font: IngeScapeTheme.headingFont

            color: IngeScapeEditorTheme.agentsONNameMappingColor
        }


        // Remove button
        Button {
            id: btnRemoveFromMapping

            anchors {
                top: parent.top
                topMargin: 10
                right: parent.right
                rightMargin: 10
            }

            opacity: rootItem.actionItemIsHovered ? 1 : 0

            visible: (opacity !== 0)
            enabled: visible

            Behavior on opacity {
                NumberAnimation {}
            }

            activeFocusOnPress: true

            style: LabellessSvgButtonStyle {
                fileCache: IngeScapeTheme.svgFileIngeScape

                pressedID: releasedID + "-pressed"
                releasedID: "delete"
                disabledID : releasedID
            }

            onClicked: {
                if (controller)
                {
                    console.log("QML: FIXME TODO deleteActionInMapping...");

                    // Delete our action
                    //controller.deleteActionInMapping(rootItem.actionInMappingVM);
                }
            }
        }


        //-------------------
        //
        // Warnings
        //
        //-------------------

        // Drop Impossible
        /*I2SvgItem {
            anchors {
                right: parent.right
                top: parent.top
                margins: 2
            }

            svgFileCache: IngeScapeTheme.svgFileIngeScape
            svgElementId: "dropImpossible"

            visible: (rootItem.dropEnabled === false)
        }*/



        //-------------------
        //
        // Global Points
        //
        //-------------------
        Rectangle {
            id: inputGlobalPoint

            anchors {
                horizontalCenter: parent.left
                verticalCenter: parent.verticalCenter
            }

            height: 13
            width: height
            radius: height/2

            color: IngeScapeEditorTheme.purpleColor
        }

        Rectangle {
            id : outputGlobalPoint

            anchors {
                horizontalCenter : parent.right
                verticalCenter: parent.verticalCenter
            }

            height: 13
            width: height
            radius: height/2

            color: IngeScapeEditorTheme.purpleColor
        }

    }
}
