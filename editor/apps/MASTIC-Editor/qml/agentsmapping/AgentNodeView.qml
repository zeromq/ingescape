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
 *
 */

import QtQuick 2.8
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4

import I2Quick 1.0

import MASTIC 1.0

import "../theme" as Theme;


Item {
    id: rootItem


    //--------------------------------
    //
    // Properties
    //
    //--------------------------------
    // Controller associated to our view
    property var controller : null;

    // Model associated to our QML item
    property var agentMappingVM: null
    property var agentName: agentMappingVM ? agentMappingVM.agentName : ""

    property bool isReduced : agentMappingVM && agentMappingVM.isReduced

    width : 228
    height : (rootItem.agentMappingVM && !rootItem.isReduced)?
                 (54 + 20*Math.max(rootItem.agentMappingVM.inputsList.count , rootItem.agentMappingVM.outputsList.count))
               : 42


    // Init position of our agent
    x: (agentMappingVM && agentMappingVM.position) ? agentMappingVM.position.x : 0
    y: (agentMappingVM && agentMappingVM.position) ? agentMappingVM.position.y : 0


    clip : true



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
        }
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


        drag.target: parent
        // Disable smoothed so that the Item pixel from where we started
        // the drag remains under the mouse cursor
        drag.smoothed: false


        onPressed: {
            if (controller && agentMappingVM) {
                controller.selectedAgent = agentMappingVM
            }

            parent.z = rootItem.parent.maxZ++;
        }

        onPositionChanged: {
            //     console.log("agentMapping position " + model.position.x + "  " + model.position.y)
        }

        onDoubleClicked: {
            if (agentMappingVM) {
                agentMappingVM.isReduced = !agentMappingVM.isReduced;
            }
        }
    }



    Rectangle {
        anchors {
            fill: parent
            leftMargin: 8
            rightMargin: 10
            topMargin: 1
            bottomMargin: 1
        }

        color : mouseArea.pressed?
                    MasticTheme.darkGreyColor2
                  : (rootItem.agentMappingVM && rootItem.agentMappingVM.isON)? MasticTheme.darkBlueGreyColor : MasticTheme.veryDarkGreyColor
        radius : 6

        Rectangle {
            anchors.fill: parent
            anchors.margins: -1

            visible : (controller && rootItem.agentMappingVM && (controller.selectedAgent === rootItem.agentMappingVM))
            color : "transparent"
            radius : 6

            border {
                width : 2
                color : MasticTheme.selectedAgentColor
            }

            Button {
                id: removeButton

                visible : (rootItem.agentMappingVM && !rootItem.agentMappingVM.isON)

                anchors {
                    top: parent.top
                    topMargin: 10
                    right : parent.right
                    rightMargin: 10
                }

                style: Theme.LabellessSvgButtonStyle {
                    fileCache: MasticTheme.svgFileMASTIC

                    pressedID: releasedID + "-pressed"
                    releasedID: "supprimer"
                    disabledID : releasedID
                }

                onClicked: {
                    //                if (controller)
                    //                {
                    //                    // Delete our agent
                    //                    controller.deleteAgent(model.QtObject);
                    //                }
                }
            }

        }

        // Agent Name
        Text {
            id : agentName
            anchors {
                left : parent.left
                leftMargin: 20
                right : parent.right
                rightMargin: 20
                verticalCenter: parent.top
                verticalCenterOffset: 20
            }


            elide: Text.ElideRight
            text : rootItem.agentName

            color : (rootItem.agentMappingVM && rootItem.agentMappingVM.isON)? MasticTheme.agentsONNameMappingColor : MasticTheme.agentsOFFNameMappingColor
            font: MasticTheme.headingFont
        }


        // Warnings
        Rectangle {
            id : agentWithSameName
            height : 17
            width : height
            radius : height/2

            visible: false // (model && model.models) ? (model.models.count > 1) : false

            anchors {
                verticalCenter: agentName.verticalCenter
                verticalCenterOffset:  2
                right : parent.right
                rightMargin: 48
            }

            color : MasticTheme.redColor

            Text {
                anchors.centerIn : parent
                anchors.verticalCenterOffset: -1

                text: "3" //(model && model.models) ? model.models.count : ""

                color : MasticTheme.whiteColor
                font {
                    family: MasticTheme.labelFontFamily
                    weight : Font.Black
                    pixelSize : 13
                }
            }
        }

        Rectangle {
            height : 17
            width : height
            radius : height/2

            visible: false // (model && model.models) ? (model.models.count > 1) : false

            anchors {
                verticalCenter: agentName.verticalCenter
                verticalCenterOffset:  2
                left : agentWithSameName.right
                leftMargin: 5
            }

            color : MasticTheme.redColor

            Text {
                anchors {
                    centerIn : parent
                    verticalCenterOffset:  -1
                }
                text: "!" //(model && model.models) ? model.models.count : ""

                color : MasticTheme.whiteColor
                font {
                    family: MasticTheme.labelFontFamily
                    weight : Font.Black
                    pixelSize : 14
                }
            }
        }


        //Separator
        Rectangle {
            id : separator
            anchors {
                left : parent.left
                leftMargin: 18
                right : parent.right
                rightMargin: 18
                verticalCenter : parent.top
                verticalCenterOffset: 40
            }

            height : 2
            color : agentName.color

            visible : !rootItem.isReduced
        }



        //
        //
        // Global Points
        //
        Rectangle {
            id : inputGlobalPoint

            anchors {
                horizontalCenter : parent.left
                verticalCenter: parent.verticalCenter
            }

            height : 13
            width : height
            radius : height/2

            color : if (agentMappingVM) {
                        switch (agentMappingVM.reducedMapValueTypeInInput)
                        {
                        case AgentIOPValueTypes.INTEGER:
                            agentMappingVM.isON? MasticTheme.orangeColor2 : MasticTheme.darkOrangeColor2
                            break;
                        case AgentIOPValueTypes.DOUBLE:
                            agentMappingVM.isON? MasticTheme.orangeColor2 : MasticTheme.darkOrangeColor2
                            break;
                        case AgentIOPValueTypes.STRING:
                            agentMappingVM.isON? MasticTheme.redColor2 : MasticTheme.darkRedColor2
                            break;
                        case AgentIOPValueTypes.BOOL:
                            agentMappingVM.isON? MasticTheme.orangeColor2 : MasticTheme.darkOrangeColor2
                            break;
                        case AgentIOPValueTypes.IMPULSION:
                            agentMappingVM.isON? MasticTheme.purpleColor : MasticTheme.darkPurpleColor
                            break;
                        case AgentIOPValueTypes.DATA:
                            agentMappingVM.isON? MasticTheme.greenColor : MasticTheme.darkGreenColor
                            break;
                        case AgentIOPValueTypes.MIXED:
                            agentMappingVM.isON? MasticTheme.whiteColor : MasticTheme.darkGreyColor
                            break;
                        case AgentIOPValueTypes.UNKNOWN:
                            "#000000"
                            break;
                        default:
                            MasticTheme.whiteColor;
                            break;
                        }
                    } else {
                        MasticTheme.whiteColor
                    }

            visible : rootItem.isReduced && rootItem.agentMappingVM && rootItem.agentMappingVM.inputsList.count > 0
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
                        switch (agentMappingVM.reducedMapValueTypeInOutput)
                        {
                        case AgentIOPValueTypes.INTEGER:
                            agentMappingVM.isON? MasticTheme.orangeColor2 : MasticTheme.darkOrangeColor2
                            break;
                        case AgentIOPValueTypes.DOUBLE:
                            agentMappingVM.isON? MasticTheme.orangeColor2 : MasticTheme.darkOrangeColor2
                            break;
                        case AgentIOPValueTypes.STRING:
                            agentMappingVM.isON? MasticTheme.redColor2 : MasticTheme.darkRedColor2
                            break;
                        case AgentIOPValueTypes.BOOL:
                            agentMappingVM.isON? MasticTheme.orangeColor2 : MasticTheme.darkOrangeColor2
                            break;
                        case AgentIOPValueTypes.IMPULSION:
                            agentMappingVM.isON? MasticTheme.purpleColor : MasticTheme.darkPurpleColor
                            break;
                        case AgentIOPValueTypes.DATA:
                            agentMappingVM.isON? MasticTheme.greenColor : MasticTheme.darkGreenColor
                            break;
                        case AgentIOPValueTypes.MIXED:
                            agentMappingVM.isON? MasticTheme.whiteColor : MasticTheme.darkGreyColor
                            break;
                        case AgentIOPValueTypes.UNKNOWN:
                            "#000000"
                            break;
                        default:
                            MasticTheme.whiteColor;
                            break;
                        }
                    } else {
                        MasticTheme.whiteColor
                    }

            visible : rootItem.isReduced && rootItem.agentMappingVM && rootItem.agentMappingVM.outputsList.count > 0
        }


        //
        //
        // Inlets / Input slots
        //
        CollapsibleColumn {
            id: columnInputSlots

            anchors {
                left: parent.left
                right : parent.right

                top: separator.bottom
                topMargin: 5
                bottom: parent.bottom
            }

            visible : !rootItem.isReduced

            Repeater {
                // List of intput slots VM
                model: rootItem.agentMappingVM ? rootItem.agentMappingVM.inputsList : 0

                delegate: Item {
                    id: inputSlotItem

                    property var myModel: model.QtObject

                    height : 20
                    anchors {
                        left : parent.left
                        right : parent.right
                    }

                    Text {
                        id : agentInput
                        anchors {
                            left : parent.left
                            leftMargin: 20
                            right : parent.horizontalCenter
                            rightMargin: 5
                            verticalCenter: parent.verticalCenter
                        }
                        elide: Text.ElideRight
                        text : myModel.modelM ? myModel.modelM.name : ""

                        color : (rootItem.agentMappingVM && rootItem.agentMappingVM.isON)? MasticTheme.agentsONInputsOutputsMappingColor : MasticTheme.agentsOFFInputsOutputsMappingColor
                        font: MasticTheme.heading2Font
                    }

                    Rectangle {
                        id : draggablePointFROM

                        height : linkPoint.height
                        width : height
                        radius : height/2

                        property bool dragActive : mouseAreaPointFROM.drag.active;
                        property var inputSlotModel: model.QtObject

                        Drag.active: mouseAreaPointFROM.drag.active;
                        Drag.hotSpot.x: width/2
                        Drag.hotSpot.y: height/2
                        Drag.keys: ["InputSlotItem"]

                        border {
                            width : 1
                            color : draggablePointFROM.dragActive? linkPoint.color : "transparent"
                        }


                        color : draggablePointFROM.dragActive? MasticTheme.agentsMappingBackgroundColor : "transparent"
                        parent : draggablePointFROM.dragActive? rootItem.parent  : linkPoint

                        MouseArea {
                            id: mouseAreaPointFROM

                            anchors.fill: parent

                            drag.target: parent
                            // Disable smoothed so that the Item pixel from where we started
                            // the drag remains under the mouse cursor
                            drag.smoothed: false

                            hoverEnabled: true

                            cursorShape: (draggablePointFROM.dragActive)? Qt.ClosedHandCursor : Qt.PointingHandCursor //Qt.OpenHandCursor

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
                            id : linkDraggablePoint

                            parent : rootItem.parent

                            visible: draggablePointFROM.dragActive

                            secondPoint: Qt.point(myModel.position.x + draggablePointFROM.width/2 , myModel.position.y)
                            firstPoint: Qt.point(draggablePointFROM.x + draggablePointFROM.width/2, draggablePointFROM.y + draggablePointFROM.height/2)

                            defaultColor:linkPoint.color
                        }
                    }


                    Rectangle {
                        id : linkPoint

                        anchors {
                            horizontalCenter: parent.left
                            verticalCenter: parent.verticalCenter
                        }

                        height : 13
                        width : height
                        radius : height/2

                        border {
                            width : 0
                            color : MasticTheme.lightGreyColor // "#DADADA"
                        }

                        color : if (agentMappingVM && myModel && myModel.modelM) {

                                    switch (myModel.modelM.agentIOPValueType)
                                    {
                                    case AgentIOPValueTypes.INTEGER:
                                        agentMappingVM.isON? MasticTheme.orangeColor2 : MasticTheme.darkOrangeColor2
                                        break;
                                    case AgentIOPValueTypes.DOUBLE:
                                        agentMappingVM.isON? MasticTheme.orangeColor2 : MasticTheme.darkOrangeColor2
                                        break;
                                    case AgentIOPValueTypes.STRING:
                                        agentMappingVM.isON? MasticTheme.redColor2 : MasticTheme.darkRedColor2
                                        break;
                                    case AgentIOPValueTypes.BOOL:
                                        agentMappingVM.isON? MasticTheme.orangeColor2 : MasticTheme.darkOrangeColor2
                                        break;
                                    case AgentIOPValueTypes.IMPULSION:
                                        agentMappingVM.isON? MasticTheme.purpleColor : MasticTheme.darkPurpleColor
                                        break;
                                    case AgentIOPValueTypes.DATA:
                                        agentMappingVM.isON? MasticTheme.greenColor : MasticTheme.darkGreenColor
                                        break;
                                    case AgentIOPValueTypes.MIXED:
                                        agentMappingVM.isON? MasticTheme.whiteColor : MasticTheme.darkGreyColor
                                        break;
                                    case AgentIOPValueTypes.UNKNOWN:
                                        "#000000"
                                        break;
                                    default:
                                        "#000000"
                                        break;
                                    }

                                } else {
                                    MasticTheme.whiteColor
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
                            console.log("inputDropArea: drag enter ");

                            if (drag.source !== null)
                            {
                                var dragItem = drag.source;

                                if (typeof dragItem.dragActive !== 'undefined')
                                {
                                    dragItem.color = dragItem.border.color;
                                    linkPoint.border.width = 2
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
                            console.log("inputDropArea: onExited");
                            var dragItem = drag.source;
                            if (typeof dragItem.dragActive !== 'undefined')
                            {
                                dragItem.color = "transparent";
                                linkPoint.border.width = 0
                            }
                        }


                        onDropped: {
                            var dragItem = drag.source;
                            if (dragItem)
                            {
                                if (typeof dragItem.outputSlotModel !== 'undefined')
                                {
                                    dragItem.color = "transparent";
                                    linkPoint.border.width = 0

                                    console.log("inputDropArea: create a link from " + dragItem.outputSlotModel + " to " + inputSlotItem.myModel);
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

                        // the position inside the agent is not the same if the agent is reduced or not
                        value:  (rootItem.agentMappingVM && !rootItem.agentMappingVM.isReduced) ?
                                    (Qt.point(rootItem.x + columnInputSlots.x + inputSlotItem.x + linkPoint.x + linkPoint.width/2,
                                              rootItem.y + columnInputSlots.y + inputSlotItem.y + linkPoint.y + linkPoint.height/2))
                                  : (Qt.point(rootItem.x + inputGlobalPoint.x + inputGlobalPoint.width/2,
                                              rootItem.y + inputGlobalPoint.y + inputGlobalPoint.height/2));
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
                left: parent.left
                right : parent.right
                top: separator.bottom
                topMargin: 5
                bottom: parent.bottom
            }

            visible : !rootItem.isReduced

            Repeater {
                // List of output slots VM
                model: (rootItem.agentMappingVM)? rootItem.agentMappingVM.outputsList : 0

                delegate: Item {
                    id: outputSlotItem

                    property var myModel: model.QtObject

                    height : 20
                    anchors {
                        left : parent.left
                        right : parent.right
                    }

                    Text {
                        id : agentOutput
                        anchors {
                            left : parent.horizontalCenter
                            leftMargin: 5
                            right : parent.right
                            rightMargin: 20
                            verticalCenter: parent.verticalCenter
                        }

                        horizontalAlignment : Text.AlignRight

                        elide: Text.ElideRight
                        text : myModel.modelM ? myModel.modelM.name : ""

                        color : (rootItem.agentMappingVM && rootItem.agentMappingVM.isON)? MasticTheme.agentsONInputsOutputsMappingColor : MasticTheme.agentsOFFInputsOutputsMappingColor
                        font: MasticTheme.heading2Font
                    }



                    Rectangle {
                        id : draggablePointTO

                        height : linkPointOut.height
                        width : height
                        radius : height/2

                        border {
                            width : 1
                            color : draggablePointTO.dragActive? linkPointOut.color : "transparent"
                        }

                        property bool dragActive : mouseAreaPointTO.drag.active;
                        property var outputSlotModel: model.QtObject

                        Drag.active: draggablePointTO.dragActive;
                        Drag.hotSpot.x: 0
                        Drag.hotSpot.y: 0
                        Drag.keys: ["OutputSlotItem"]

                        color : draggablePointTO.dragActive? MasticTheme.agentsMappingBackgroundColor : "transparent"
                        parent : draggablePointTO.dragActive? rootItem.parent  : linkPointOut

                        MouseArea {
                            id: mouseAreaPointTO

                            anchors.fill: parent

                            drag.target: parent
                            // Disable smoothed so that the Item pixel from where we started
                            // the drag remains under the mouse cursor
                            drag.smoothed: false

                            hoverEnabled: true

                            cursorShape: (draggablePointTO.dragActive)? Qt.ClosedHandCursor : Qt.PointingHandCursor //Qt.OpenHandCursor

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
                            id : linkDraggablePointTO

                            parent : rootItem.parent
                            visible: draggablePointTO.dragActive

                            firstPoint: Qt.point(myModel.position.x + draggablePointTO.width/2 , myModel.position.y)
                            secondPoint: Qt.point(draggablePointTO.x + draggablePointTO.width/2, draggablePointTO.y + draggablePointTO.height/2)

                            defaultColor:linkPointOut.color
                        }
                    }



                    Rectangle {
                        id : linkPointOut

                        anchors {
                            horizontalCenter: parent.right
                            verticalCenter: parent.verticalCenter
                        }

                        height : 13
                        width : height
                        radius : height/2

                        color : if (agentMappingVM && myModel && myModel.modelM) {

                                    switch (myModel.modelM.agentIOPValueType)
                                    {
                                    case AgentIOPValueTypes.INTEGER:
                                        (agentMappingVM.isON && !myModel.modelM.isMuted) ? MasticTheme.orangeColor2 : MasticTheme.darkOrangeColor2
                                        break;
                                    case AgentIOPValueTypes.DOUBLE:
                                        (agentMappingVM.isON && !myModel.modelM.isMuted)? MasticTheme.orangeColor2 : MasticTheme.darkOrangeColor2
                                        break;
                                    case AgentIOPValueTypes.STRING:
                                        (agentMappingVM.isON && !myModel.modelM.isMuted)? MasticTheme.redColor2 : MasticTheme.darkRedColor2
                                        break;
                                    case AgentIOPValueTypes.BOOL:
                                        (agentMappingVM.isON && !myModel.modelM.isMuted)? MasticTheme.orangeColor2 : MasticTheme.darkOrangeColor2
                                        break;
                                    case AgentIOPValueTypes.IMPULSION:
                                        (agentMappingVM.isON && !myModel.modelM.isMuted)? MasticTheme.purpleColor : MasticTheme.darkPurpleColor
                                        break;
                                    case AgentIOPValueTypes.DATA:
                                        (agentMappingVM.isON && !myModel.modelM.isMuted)? MasticTheme.greenColor : MasticTheme.darkGreenColor
                                        break;
                                    case AgentIOPValueTypes.MIXED:
                                        (agentMappingVM.isON && !myModel.modelM.isMuted)? MasticTheme.whiteColor : MasticTheme.darkGreyColor
                                        break;
                                    case AgentIOPValueTypes.UNKNOWN:
                                        "#000000"
                                        break;
                                    default:
                                        "#000000"
                                        break;
                                    }

                                } else {
                                    MasticTheme.whiteColor
                                }



                        I2SvgItem {
                            anchors.centerIn: parent
                            svgFileCache: MasticTheme.svgFileMASTIC
                            svgElementId: "outputIsMuted"

                            visible : myModel.modelM && myModel.modelM.isMuted
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
                            console.log("outputDropArea: drag enter ");

                            if (drag.source !== null)
                            {
                                var dragItem = drag.source;

                                if (typeof dragItem.dragActive !== 'undefined')
                                {
                                    dragItem.color = dragItem.border.color;
                                    linkPointOut.border.width = 2
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


                        onPositionChanged: {
                        }


                        onExited: {
                            console.log("outputDropArea: onExited");
                            var dragItem = drag.source;
                            if (typeof dragItem.dragActive !== 'undefined')
                            {
                                dragItem.color = "transparent";
                                linkPointOut.border.width = 0
                            }
                        }

                        onDropped: {
                            var dragItem = drag.source;
                            if (dragItem)
                            {
                                if (typeof dragItem.inputSlotModel !== 'undefined')
                                {
                                    dragItem.color = "transparent";
                                    linkPointOut.border.width = 0

                                    console.log("outputDropArea: create a link from " + outputSlotItem.myModel + " to " + dragItem.inputSlotModel);
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

                        // the position inside the agent is not the same if the agent is reduced or not
                        value: (rootItem.agentMappingVM && !rootItem.agentMappingVM.isReduced) ?
                                   (Qt.point(rootItem.x + columnOutputSlots.x + outputSlotItem.x + linkPointOut.x + linkPointOut.width/2,
                                             rootItem.y + columnOutputSlots.y + outputSlotItem.y + linkPointOut.y + linkPointOut.height/2))
                                 : (Qt.point(rootItem.x + outputGlobalPoint.x + outputGlobalPoint.width/2,
                                             rootItem.y + outputGlobalPoint.y + outputGlobalPoint.height/2));
                    }
                }
            }
        }

    }



}
