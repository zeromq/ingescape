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
 *      Justine Limoges   <limoges@ingenuity.io>
 */

import QtQuick 2.8
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4

import I2Quick 1.0

import INGESCAPE 1.0
import "../theme" as Theme


Item {
    id : actionVMItem

    // model of the action VM
    property  var myActionVM : null;

    // Controllers associated to our view
    property var scenarioController: null;
    property var timeLineController: null;

    // height of a line in the time line
    property int lineHeight: IngeScapeTheme.lineInTimeLineHeight


    //ghost temporary start time when drag is active
    property var temporaryStartTime: null;


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



    Drag.active: actionVMMouseArea.drag.active
    Drag.hotSpot.x: (actionVMMouseArea.mouseX + actionVMMouseArea.x)
    Drag.hotSpot.y: actionVMMouseArea.mouseY
    Drag.keys: ["ActionInTimeLine"]

    Binding {
        target: actionVMItem
        property: "y"
        value: if (myActionVM)
               {
                   actionVMItem.lineHeight * myActionVM.lineInTimeLine
               }
               else {
                   0
               }
        when: !actionVMMouseArea.drag.active
    }

    Binding {
        target: actionVMItem
        property: "x"
        value: if (myActionVM)
               {
                    timeLineController.convertTimeInMillisecondsToAbscissaInCoordinateSystem(myActionVM.startTime, timeLineController.pixelsPerMinute)
               }
               else {
                   0
               }
        when: !actionVMMouseArea.drag.active
    }

    x : myActionVM ? timeLineController.convertTimeInMillisecondsToAbscissaInCoordinateSystem(myActionVM.startTime, timeLineController.pixelsPerMinute) : 0;
    y : myActionVM ? (actionVMItem.lineHeight * myActionVM.lineInTimeLine) : 0;
    height : actionVMItem.lineHeight
    width : if (myActionVM && myActionVM.modelM)
            {
                switch (myActionVM.modelM.validityDurationType)
                {
                case ValidationDurationTypes.IMMEDIATE:
                    0;
                    break;
                case ValidationDurationTypes.FOREVER:
                    (timeLineController.timeTicksTotalWidth - timeLineController.convertTimeInMillisecondsToAbscissaInCoordinateSystem(myActionVM.startTime, timeLineController.pixelsPerMinute))
                    break;
                case ValidationDurationTypes.CUSTOM:
                    timeLineController.convertDurationInMillisecondsToLengthInCoordinateSystem(myActionVM.modelM.validityDuration, timeLineController.pixelsPerMinute)
                    break;
                default:
                    0;
                    break;
                }
            }
            else {
                0;
            }


    focus: true

    //remove action VM
    Keys.onPressed: {
        if (event.key === Qt.Key_Backspace || event.key === Qt.Key_Delete)
        {
            if (scenarioController && scenarioController.selectedActionVMInTimeline) {
                scenarioController.removeActionVMfromTimeLine(scenarioController.selectedActionVMInTimeline);
            }

            event.accepted = true;
        }
    }

    //deselect action VM
    onFocusChanged: {
        if (!focus) {
            if (scenarioController && scenarioController.selectedActionVMInTimeline) {
                scenarioController.selectedActionVMInTimeline = null;
            }
        }
    }


    // rectangle representing the validity conditions time
    Rectangle {
        id : conditionsValidityRect
        anchors {
            top : parent.top
            left : parent.left
            right : parent.right
        }

        //see only the ghost when the drag is active
        visible: !actionVMMouseArea.drag.active

        height : (actionVMItem.lineHeight-7)/2
        color : IngeScapeTheme.blueGreyColor2

        border {
            width : (scenarioController && scenarioController.selectedActionVMInTimeline && actionVMItem.myActionVM  && scenarioController.selectedActionVMInTimeline === actionVMItem.myActionVM) ? 1 :0;
            color : IngeScapeTheme.orangeColor
        }
    }

    Item {
        id : executionsItem
        height : conditionsValidityRect.height
        width : childrenRect.width
        //see only the ghost when the drag is active
        visible: !actionVMMouseArea.drag.active

        // capture minimum X value in order to make the clic enable on the whole actionInTimeline item when the x value is below 0;
        property real minimunXValueInExecutionList : 0;

        // executions List
        Repeater {
            model: if (myActionVM) {
                       myActionVM.executionsList;
                   }
                   else {
                       0;
                   }

            Item {
                height : conditionsValidityRect.height
                width : model.shallRevert? revertItem.width : notRevertItem.width;


                // Not revert action
                I2SvgItem {
                    id : notRevertItem
                    x : timeLineController.convertDurationInMillisecondsToLengthInCoordinateSystem(model.executionTime, timeLineController.pixelsPerMinute) - width/2;

                    onXChanged: {
                        if (visible) {
                            executionsItem.minimunXValueInExecutionList = Math.min(executionsItem.minimunXValueInExecutionList, notRevertItem.x);
                        }
                    }

                    anchors {
                        verticalCenter: parent.verticalCenter
                    }

                    visible: !model.shallRevert

                    svgFileCache: IngeScapeTheme.svgFileIngeScape
                    svgElementId: (model.neverExecuted ? "notExcutedAction"
                                                       : (model.isExecuted ? "timelineAction" : "currentAction"))
                }

                // Revert action
                Item {
                    id : revertItem
                    visible : model.shallRevert
                    height : childrenRect.height
                    width : childrenRect.width

                    anchors {
                        verticalCenter: parent.verticalCenter
                    }

                    Rectangle {
                        anchors {
                            verticalCenter: actionExecution.verticalCenter
                            left : actionExecution.horizontalCenter
                            right : revertActionExecution.horizontalCenter
                        }
                        height : 1
                        color : (model.neverExecuted)? IngeScapeTheme.darkGreyColor : IngeScapeTheme.whiteColor;

                    }

                    I2SvgItem {
                        id : actionExecution
                        x : timeLineController.convertDurationInMillisecondsToLengthInCoordinateSystem(model.executionTime, timeLineController.pixelsPerMinute);
                        y : 0

                        svgFileCache: IngeScapeTheme.svgFileIngeScape;
                        svgElementId: (model.neverExecuted ? "notExecutedRevertAction"
                                                           : (model.isExecuted ? "revertAction" : "currentRevertAction"))
                    }

                    I2SvgItem {
                        id : revertActionExecution
                        x : timeLineController.convertDurationInMillisecondsToLengthInCoordinateSystem(model.reverseTime, timeLineController.pixelsPerMinute) - width;
                        y : 0
                        rotation : 180
                        svgFileCache: IngeScapeTheme.svgFileIngeScape
                        svgElementId: (model.neverExecuted ? "notExecutedRevertAction"
                                                           : (model.isExecuted ? "revertAction" : "currentRevertAction"))
                    }
                }

            }
        }
    }

    // Action Name
    Rectangle {
        id : backgroundActionName
        //see only the ghost when the drag is active
        visible: !actionVMMouseArea.drag.active

        anchors {
            fill : actionName
            leftMargin:-1
            rightMargin:-1
        }
        color : IngeScapeTheme.blackColor
    }

    Text {
        id: actionName
        //see only the ghost when the drag is active
        visible: !actionVMMouseArea.drag.active

        anchors {
            top : parent.verticalCenter
            bottomMargin: 1
            bottom : parent.bottom
            left : parent.left
        }
        verticalAlignment: Text.AlignVCenter
        color: (scenarioController && scenarioController.selectedActionVMInTimeline && actionVMItem.myActionVM  && (scenarioController.selectedActionVMInTimeline === actionVMItem.myActionVM)) ?
                   (actionVMMouseArea.pressed ? IngeScapeTheme.lightGreyColor : IngeScapeTheme.orangeColor)
                 : (actionVMMouseArea.pressed ? IngeScapeTheme.greyColor : IngeScapeTheme.lightGreyColor)

        text: (actionVMItem.myActionVM && actionVMItem.myActionVM.modelM) ? actionVMItem.myActionVM.modelM.name : ""
        font {
            family : IngeScapeTheme.textFontFamily
            pixelSize: 16
        }
    }


   // Mouse Area allows opening the editor, selecting the action VM and dragging it
    MouseArea {
        id : actionVMMouseArea
        anchors {
            top : actionVMItem.top
            bottom : actionVMItem.bottom
        }
        // according to the executions list executionsItem.x value could be below actionVMItem.x
        x : Math.min(executionsItem.minimunXValueInExecutionList, 0);

        // find element with the maximum width in the actionInTimeline item : could be the name, conditions validity time or executions list
        // x could be below 0 => need to add '-x'
        width : Math.max(backgroundActionName.width,
                         actionVMItem.width,
                         executionsItem.width) - x
        hoverEnabled: true

        drag.smoothed: false
        drag.target: !scenarioController.isPlaying ? actionVMItem : null
        cursorShape: (actionVMMouseArea.drag.active)? Qt.PointingHandCursor : Qt.OpenHandCursor //Qt.OpenHandCursor

        onPressed: {
            // Find our layer and reparent our popup in it
            itemDragged.parent = actionVMItem.findLayerRootByObjectName(itemDragged, "overlayLayerDraggableItem");
        }

        onPositionChanged: {// Compute new position if needed
            if (itemDragged.parent !== null)
            {
                var newPosition = actionVMItem.mapToItem(itemDragged.parent,
                                                         mouse.x - 12 - itemDragged.width + actionVMMouseArea.x,
                                                         mouse.y - 12 - itemDragged.height);
                itemDragged.x = newPosition.x;
                itemDragged.y = newPosition.y;
            }
        }

        onReleased: {
            actionVMItem.Drag.drop();

            //
            // Reset the position of our action
            //
            itemDragged.parent = actionVMItem;

            if (actionVMItem && actionVMItem.myActionVM && actionVMItem.timeLineController) {
                actionVMItem.x = actionVMItem.timeLineController.convertTimeInMillisecondsToAbscissaInCoordinateSystem(actionVMItem.myActionVM.startTime, actionVMItem.timeLineController.pixelsPerMinute);
                actionVMItem.y = (actionVMItem.lineHeight * actionVMItem.myActionVM.lineInTimeLine);
            }
        }


        onClicked: {
            actionVMItem.forceActiveFocus()

            // selection of the action VM
            if (scenarioController && actionVMItem.myActionVM) {
                if (scenarioController.selectedActionVMInTimeline !== actionVMItem.myActionVM) {
                    scenarioController.selectedActionVMInTimeline = actionVMItem.myActionVM;
                }
                else {
                    scenarioController.selectedActionVMInTimeline = null;
                }
            }
        }

        onDoubleClicked: {
            // open action editor
            if (scenarioController && actionVMItem.myActionVM)
            {
                scenarioController.openActionEditorWithViewModel(actionVMItem.myActionVM);

                // select the action VM
                scenarioController.selectedActionVMInTimeline = actionVMItem.myActionVM;
            }
        }
    }


    // Ghost appearing when the action vm is dragging in the timeline
    I2CustomRectangle{
        id: itemDragged

        width: Math.max(IngeScapeTheme.timeWidth, nameAction.width + 10)
        height : columnText.height + 8

        color : IngeScapeTheme.darkBlueGreyColor
        visible: actionVMMouseArea.drag.active

        // - fuzzy radius around our rectangle
        fuzzyRadius: 2
        fuzzyColor: IngeScapeTheme.blackColor

        Column {
            id: columnText
            height: temporaryStartTimeAction.visible ? (nameAction.height + temporaryStartTimeAction.height) + 3
                                                     : nameAction.height

            anchors.centerIn: parent
            spacing: 6

            Text {
                id: nameAction

                anchors.horizontalCenter: parent.horizontalCenter
                horizontalAlignment: Text.AlignHCenter

                color: IngeScapeTheme.lightGreyColor

                text: (actionVMItem.myActionVM && actionVMItem.myActionVM.modelM) ? actionVMItem.myActionVM.modelM.name
                                                                                  : ""

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
                color: IngeScapeTheme.lightGreyColor

                text: actionVMItem.temporaryStartTime ? actionVMItem.temporaryStartTime.toLocaleString(Qt.locale(), "HH:mm:ss.zzz")
                                                      : "";

                font {
                    family : IngeScapeTheme.textFontFamily
                    pixelSize: 14
                }
            }
        }
    }

}

