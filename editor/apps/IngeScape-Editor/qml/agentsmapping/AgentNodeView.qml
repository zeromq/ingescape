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
 *      Alexandre Lemort    <lemort@ingenuity.io>
 *      Justine Limoges     <limoges@ingenuity.io>
 *      Vincent Peyruqueou  <peyruqueou@ingenuity.io>
 *
 */

import QtQuick 2.8
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4
import QtQuick.Controls 2.0 as Controls2

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
    property AgentInMappingVM agentMappingVM: null

    property var agentsGroupedByName: agentMappingVM ? agentMappingVM.agentsGroupedByName : null

    property string agentName: agentMappingVM ? agentMappingVM.name : ""

    // Flag indicating if our agent is reduced (List of Inputs/Outputs are hidden)
    property bool isReduced: agentMappingVM && agentMappingVM.isReduced

    // Flag indicating if mouse areas over input/output names (to display the tooltip) are enabled
    // When our agent is reduced, the user must hover the header of our agent to double click on it (to open it)
    // --> this flag allows to prevent the mouse areas over input/output names to detect a hover and to display the tooltip
    property bool areToolTipEnabled: false

    // false if the agent is dropping and the drop is not available, true otherwise
    property bool dropEnabled: true

    // Flag indicating if the mouse is hover our agent
    property bool agentItemIsHovered: mouseArea.containsMouse

    // To check if our item is selected or not
    property bool _isSelected: (controller && rootItem.agentMappingVM && (controller.selectedAgent === rootItem.agentMappingVM))


    // Duration of expand/collapse animation in milliseconds (250 ms => default duration of QML animations)
    property int _expandCollapseAnimationDuration: 250


    width: 258

    height: (rootItem.agentMappingVM && !rootItem.isReduced) ? (54 + 22 * Math.max(rootItem.agentMappingVM.linkInputsList.count, rootItem.agentMappingVM.linkOutputsList.count))
                                                             : 42


    // Init position of our agent
    x: (agentMappingVM && agentMappingVM.position) ? agentMappingVM.position.x : 0
    y: (agentMappingVM && agentMappingVM.position) ? agentMappingVM.position.y : 0


    radius: 6

    color: (dropEnabled === true) ? (mouseArea.pressed ? IngeScapeTheme.darkGreyColor2
                                                       : (rootItem.agentsGroupedByName && rootItem.agentsGroupedByName.isON) ? IngeScapeTheme.darkBlueGreyColor : IngeScapeTheme.veryDarkGreyColor)
                                  : IngeScapeTheme.darkGreyColor2


    border {
        color: IngeScapeTheme.selectedAgentColor
        width: rootItem._isSelected ? 1 : 0
    }


    onIsReducedChanged: {
        if (rootItem.isReduced)
        {
            //console.log(rootItem.agentName + " Reduced --> areToolTipEnabled = false");

            // Disable each mouse areas over input/output names to prevent them to display the corresponding tooltip
            rootItem.areToolTipEnabled = false;
        }
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
        target: rootItem.agentMappingVM
        property: "position"
        value: Qt.point(rootItem.x, rootItem.y)
    }



    //
    // Animation used when our item is collapsed or expanded (its height changes)
    //
    Behavior on height {
        NumberAnimation {
            duration: rootItem._expandCollapseAnimationDuration
        }
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
            if (controller && agentMappingVM) {
                if (controller.selectedAgent === agentMappingVM)
                {
                    controller.selectedAgent = null;
                }
                else {
                    controller.selectedAgent = agentMappingVM;
                }
            }
        }

        onDoubleClicked: {
            // Check if our agent is locked reduced (prevent to open the list of Inputs/Outputs)
            if (agentMappingVM && !agentMappingVM.isLockedReduced)
            {
                agentMappingVM.isReduced = !agentMappingVM.isReduced;
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
                bottom: parent.bottom

                top: separator.bottom

                // When our agent is reduced, all slots must be centered on Y = AgentNodeView.height/2 = 21
                topMargin: (!rootItem.isReduced) ? 5 : -31
            }

            visible: (opacity !== 0)
            opacity: !rootItem.isReduced ? 1 : 0

            Behavior on opacity {
                NumberAnimation {
                    duration: rootItem._expandCollapseAnimationDuration
                }
            }

            Behavior on anchors.topMargin {
                NumberAnimation {
                    duration: rootItem._expandCollapseAnimationDuration
                }
            }


            //
            // Inlets / Input slots
            //
            CollapsibleColumn {
                id: columnInputSlots

                anchors {
                    fill: parent
                }

                value: rootItem.isReduced ? 0 : 1

                Behavior on value {
                    NumberAnimation {
                        duration: rootItem._expandCollapseAnimationDuration
                    }
                }


                Repeater {
                    // List of intput slots VM
                    model: (rootItem.agentMappingVM ? rootItem.agentMappingVM.linkInputsList : 0)

                    delegate: Item {
                        id: inputSlotItem

                        property var myModel: model.QtObject

                        height: 22
                        anchors {
                            left: parent.left
                            right: parent.right
                        }

                        Text {
                            id: agentInput

                            anchors {
                                left: parent.left
                                leftMargin: 20
                                right: parent.horizontalCenter
                                rightMargin: 5
                                verticalCenter: parent.verticalCenter
                            }

                            elide: Text.ElideRight
                            text: myModel ? myModel.name : ""

                            color: (myModel && myModel.input && myModel.input.isDefinedInAllDefinitions) ? (rootItem.agentsGroupedByName && rootItem.agentsGroupedByName.isON ? IngeScapeTheme.agentsONInputsOutputsMappingColor : IngeScapeTheme.agentsOFFInputsOutputsMappingColor)
                                                                                                         : (rootItem.agentsGroupedByName && rootItem.agentsGroupedByName.isON ? IngeScapeTheme.redColor : IngeScapeTheme.middleDarkRedColor)

                            font: IngeScapeTheme.heading2Font

                            MouseArea {
                                id: rootTooltipInput
                                anchors.fill: parent
                                acceptedButtons: Qt.NoButton
                                hoverEnabled: rootItem.areToolTipEnabled
                                cursorShape: Qt.PointingHandCursor
                            }

                            Controls2.ToolTip {
                                delay: 400
                                visible: rootTooltipInput.containsMouse
                                text: ((myModel && myModel.input && myModel.input.firstModel) ? myModel.name + " (" + AgentIOPValueTypes.enumToString(myModel.input.firstModel.agentIOPValueType) + ")": "")
                            }
                        }


                        Rectangle {
                            id: draggablePointFROM

                            height: linkPoint.height
                            width: height
                            radius: height/2

                            property bool dragActive: mouseAreaPointFROM.drag.active;
                            property var agentInMappingVMOfInput: rootItem.agentMappingVM;
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

                                cursorShape: (draggablePointFROM.dragActive) ? Qt.ClosedHandCursor : Qt.PointingHandCursor

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
                            id : linkPoint

                            anchors {
                                horizontalCenter: parent.left
                                verticalCenter: parent.verticalCenter
                            }

                            height: 15
                            width: height
                            radius: height/2

                            border {
                                width : 0
                                color : IngeScapeTheme.whiteColor
                            }

                            color: if (myModel && myModel.input && myModel.input.firstModel) {
                                       IngeScapeTheme.colorOfIOPTypeWithConditions(myModel.input.firstModel.agentIOPValueTypeGroup, true);
                                   }
                                   else {
                                       IngeScapeTheme.whiteColor
                                   }
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


                            onPositionChanged: {
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
                                    if ((typeof dragItem.outputSlotModel !== 'undefined') && dragItem.agentInMappingVMOfOutput && rootItem.agentMappingVM)
                                    {
                                        dragItem.color = "transparent";
                                        linkPoint.border.width = 0
                                        linkPoint.scale = 1

                                        //console.log("inputDropArea: create a link from " + dragItem.outputSlotModel + " to " + inputSlotItem.myModel);
                                        controller.dropLinkBetweenAgents(dragItem.agentInMappingVMOfOutput, dragItem.outputSlotModel, rootItem.agentMappingVM, inputSlotItem.myModel);
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
            CollapsibleColumn {
                id: columnOutputSlots

                anchors {
                    fill: parent
                }

                value: rootItem.isReduced ? 0 : 1

                Behavior on value {
                    NumberAnimation {
                        duration: rootItem._expandCollapseAnimationDuration
                    }
                }


                Repeater {
                    // List of output slots VM
                    model: (rootItem.agentMappingVM ? rootItem.agentMappingVM.linkOutputsList : 0)

                    delegate: Item {
                        id: outputSlotItem

                        property var myModel: model.QtObject

                        height: 22
                        anchors {
                            left: parent.left
                            right: parent.right
                        }

                        Text {
                            id: agentOutput

                            anchors {
                                left: parent.horizontalCenter
                                leftMargin: 5
                                right: parent.right
                                rightMargin: 20
                                verticalCenter: parent.verticalCenter
                            }

                            horizontalAlignment: Text.AlignRight
                            elide: Text.ElideRight
                            text: myModel ? myModel.name : ""

                            color: (myModel && myModel.output && myModel.output.isDefinedInAllDefinitions) ? (rootItem.agentsGroupedByName && rootItem.agentsGroupedByName.isON ? IngeScapeTheme.agentsONInputsOutputsMappingColor : IngeScapeTheme.agentsOFFInputsOutputsMappingColor)
                                                                                                           : (rootItem.agentsGroupedByName && rootItem.agentsGroupedByName.isON ? IngeScapeTheme.redColor : IngeScapeTheme.middleDarkRedColor)
                            font: IngeScapeTheme.heading2Font

                            MouseArea {
                                id: rootTooltipOutput
                                anchors.fill: parent
                                acceptedButtons: Qt.NoButton
                                hoverEnabled: rootItem.areToolTipEnabled
                                cursorShape: Qt.PointingHandCursor
                            }

                            Controls2.ToolTip {
                                delay: 400
                                visible: rootTooltipOutput.containsMouse
                                text: ((myModel && myModel.output && myModel.output.firstModel) ? myModel.name + " (" + AgentIOPValueTypes.enumToString(myModel.output.firstModel.agentIOPValueType) + ")": "")
                            }
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
                            property var agentInMappingVMOfOutput : rootItem.agentMappingVM
                            property var outputSlotModel: model.QtObject

                            Drag.active: draggablePointTO.dragActive;
                            Drag.hotSpot.x: width/2
                            Drag.hotSpot.y: height/2
                            Drag.keys: ["OutputSlotItem"]

                            color: draggablePointTO.dragActive ? IngeScapeTheme.agentsMappingBackgroundColor : "transparent"
                            parent: draggablePointTO.dragActive ? rootItem.parent  : linkPointOut

                            MouseArea {
                                id: mouseAreaPointTO

                                anchors.fill: parent

                                drag.target: parent
                                // Disable smoothed so that the Item pixel from where we started
                                // the drag remains under the mouse cursor
                                drag.smoothed: false

                                hoverEnabled: true

                                cursorShape: (draggablePointTO.dragActive) ? Qt.ClosedHandCursor : Qt.PointingHandCursor

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

                            color: if (myModel && myModel.output && myModel.output.firstModel) {
                                       IngeScapeTheme.colorOfIOPTypeWithConditions(myModel.output.firstModel.agentIOPValueTypeGroup, !myModel.output.firstModel.isMuted);
                                   }
                                   else {
                                       IngeScapeTheme.whiteColor
                                   }



                            I2SvgItem {
                                anchors.centerIn: parent

                                svgFileCache: IngeScapeTheme.svgFileINGESCAPE
                                svgElementId: "outputIsMuted"

                                visible: (myModel.output && myModel.output.firstModel && myModel.output.firstModel.isMuted)
                            }
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
                                    if (typeof dragItem.inputSlotModel !== 'undefined' && controller && rootItem.agentMappingVM && outputSlotItem.myModel)
                                    {
                                        dragItem.color = "transparent";
                                        linkPointOut.border.width = 0
                                        linkPointOut.scale = 1

                                        //console.log("outputDropArea: create a link from " + outputSlotItem.myModel + " to " + dragItem.inputSlotModel);
                                        controller.dropLinkBetweenAgents(rootItem.agentMappingVM, outputSlotItem.myModel, dragItem.agentInMappingVMOfInput, dragItem.inputSlotModel);
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


        // Timer started when the flag "is Reduced" evolve from true to false (reduced --> opened)
        Timer {
            interval: 500
            running: !rootItem.isReduced

            onTriggered: {
                if (!rootItem.isReduced)
                {
                    //console.log(rootItem.agentName + " on timer triggered: areToolTipEnabled = true");

                    // Enable each mouse areas over input/output names to allows them to display the corresponding tooltip
                    rootItem.areToolTipEnabled = true;
                }
            }
        }


        //------------------------------------------
        //
        // Header
        //
        //------------------------------------------


        // Expand / Collapse mask
        Rectangle {
            id: expandCollapseMask

            anchors {
                left: parent.left
                leftMargin: 18
                right: parent.right
                rightMargin: 18

                bottom: separator.bottom
                top: parent.top
                topMargin: 2
            }

            color: rootItem.color

            visible: ((columnInputSlots.value !== 0) && (columnInputSlots.value !== 1))
        }


        // Separator
        Rectangle {
            id: separator

            anchors {
                left: parent.left
                leftMargin: 18
                right: parent.right
                rightMargin: 18
                top: parent.top
                topMargin: 41
            }

            height: 2
            color: agentName.color

            opacity: (!rootItem.isReduced) ? 1 : 0
            visible: (opacity !== 0)

            Behavior on opacity {
                NumberAnimation {
                    duration: rootItem._expandCollapseAnimationDuration
                }
            }
        }


        // Button to open the definition
        Button {
            id: btnOpenDefinition

            anchors {
                top: parent.top
                topMargin: 10
                left: parent.left
                leftMargin: 10
            }

            style: Theme.LabellessSvgButtonStyle {
                fileCache: IngeScapeTheme.svgFileINGESCAPE

                pressedID: releasedID + "-pressed"
                releasedID: "definition-button"
                disabledID : releasedID
            }

            opacity: rootItem.agentItemIsHovered ? 1 : 0

            visible: (opacity !== 0)
            enabled: visible

            Behavior on opacity {
                NumberAnimation {}
            }

            onClicked: {
                // Open the definition(s)
                rootItem.agentsGroupedByName.openDefinition();
            }
        }


        // Agent Name
        Text {
            id: agentName

            anchors {
                left: parent.left
                leftMargin: 30
                right: agentWithSameName.visible ? agentWithSameName.left : btnRemoveFromMapping.left
                top: parent.top
                topMargin: 10
            }

            elide: Text.ElideRight
            text: rootItem.agentName
            font: IngeScapeTheme.headingFont

            color: (dropEnabled === true) ? ((rootItem.agentsGroupedByName && rootItem.agentsGroupedByName.isON) ? IngeScapeTheme.agentsONNameMappingColor : IngeScapeTheme.agentsOFFNameMappingColor)
                                          : IngeScapeTheme.lightGreyColor
        }

        // FIXME DEBUG
        Row {
            anchors {
                top: parent.top
                right: parent.right
            }
            Rectangle {
                visible: agentMappingVM ? agentMappingVM.hadLinksAdded_WhileMappingWasUNactivated : false
                color: "red"
                width: 8
                height: 8

                Text {
                    anchors.centerIn: parent
                    text: "+"
                }
            }
            Rectangle {
                visible: agentMappingVM ? agentMappingVM.hadLinksRemoved_WhileMappingWasUNactivated : false
                color: "red"
                width: 8
                height: 8

                Text {
                    anchors.centerIn: parent
                    text: "-"
                }
            }
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

            opacity: rootItem.agentItemIsHovered ? 1 : 0

            visible: (opacity !== 0)
            enabled: visible

            Behavior on opacity {
                NumberAnimation {}
            }

            activeFocusOnPress: true

            style: Theme.LabellessSvgButtonStyle {
                fileCache: IngeScapeTheme.svgFileINGESCAPE

                pressedID: releasedID + "-pressed"
                releasedID: "delete"
                disabledID : releasedID
            }

            onClicked: {
                if (controller)
                {
                    // Delete our agent
                    controller.deleteAgentInMapping(rootItem.agentMappingVM);
                }
            }
        }


        //-------------------
        //
        // Warnings
        //
        //-------------------
        Rectangle {
            id: agentWithSameName

            anchors {
                top: parent.top
                topMargin: 8
                right: parent.right
                rightMargin: 26
            }
            height: 16
            width: height
            radius: height / 2

            visible: (rootItem.agentsGroupedByName && (rootItem.agentsGroupedByName.numberOfAgentsON > 1))

            color: IngeScapeTheme.redColor

            Text {
                anchors.centerIn: parent

                text: (rootItem.agentsGroupedByName ? rootItem.agentsGroupedByName.numberOfAgentsON : "")

                color: IngeScapeTheme.whiteColor

                font {
                    family: IngeScapeTheme.labelFontFamily
                    weight: Font.Black
                    pixelSize: 13
                }
            }
        }


        // Drop Impossible
        I2SvgItem {
            anchors {
                right: parent.right
                top: parent.top
                margins: 2
            }

            svgFileCache: IngeScapeTheme.svgFileINGESCAPE
            svgElementId: "dropImpossible"

            visible : (rootItem.dropEnabled === false)
        }



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

            height : 13
            width : height
            radius : height/2

            color : if (agentMappingVM) {
                        IngeScapeTheme.colorOfIOPTypeWithConditions(agentMappingVM.reducedLinkInputsValueTypeGroup, true);
                    }
                    else {
                        IngeScapeTheme.whiteColor
                    }

            opacity: (rootItem.isReduced && rootItem.agentMappingVM && (rootItem.agentMappingVM.linkInputsList.count > 0)) ? 1 : 0
            visible: (opacity !== 0)

            Behavior on opacity {
                NumberAnimation {
                    duration: rootItem._expandCollapseAnimationDuration
                }
            }
        }

        Rectangle {
            id : outputGlobalPoint

            anchors {
                horizontalCenter : parent.right
                verticalCenter: parent.verticalCenter
            }

            height : 13
            width : height
            radius : height/2

            color : if (agentMappingVM) {
                        IngeScapeTheme.colorOfIOPTypeWithConditions(agentMappingVM.reducedLinkOutputsValueTypeGroup, true);
                    }
                    else {
                        IngeScapeTheme.whiteColor
                    }

            opacity: (rootItem.isReduced && rootItem.agentMappingVM && (rootItem.agentMappingVM.linkOutputsList.count > 0)) ? 1 : 0
            visible: (opacity !== 0)

            Behavior on opacity {
                NumberAnimation {
                    duration: rootItem._expandCollapseAnimationDuration
                }
            }
        }


    }
}
