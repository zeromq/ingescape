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
 *      Justine Limoges   <limoges@ingenuity.io>
 */

import QtQuick 2.8
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4

import I2Quick 1.0

import MASTIC 1.0
import "../theme" as Theme


Item {
    id : actionVMItem

    // model of the action VM
    property  var myActionVM : null;

    // Controllers associated to our view
    property var controller : null;
    property var viewController : MasticEditorC.timeLineC;

    // height of a line in the time line
    property int lineHeight : MasticTheme.lineInTimeLineHeight


    //ghost temporary start time when drag is active
    property var temporaryStartTime: null;


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
                    viewController.convertTimeInMillisecondsToAbscissaInCoordinateSystem(myActionVM.startTime, viewController.pixelsPerMinute)
               }
               else {
                   0
               }
        when: !actionVMMouseArea.drag.active
    }

    x : myActionVM ? viewController.convertTimeInMillisecondsToAbscissaInCoordinateSystem(myActionVM.startTime, viewController.pixelsPerMinute) : 0;
    y : myActionVM ? (actionVMItem.lineHeight * myActionVM.lineInTimeLine) : 0;
    height : actionVMItem.lineHeight
    width : if (myActionVM && myActionVM.modelM)
            {
                switch (myActionVM.modelM.validityDurationType)
                {
                case ValidationDurationType.IMMEDIATE:
                    0;
                    break;
                case ValidationDurationType.FOREVER:
                    (viewController.timeTicksTotalWidth - viewController.convertTimeInMillisecondsToAbscissaInCoordinateSystem(myActionVM.startTime, viewController.pixelsPerMinute))
                    break;
                case ValidationDurationType.CUSTOM:
                    viewController.convertDurationInMillisecondsToLengthInCoordinateSystem(myActionVM.modelM.validityDuration, viewController.pixelsPerMinute)
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
            if (controller && controller.selectedActionVMInTimeline) {
                controller.removeActionVMFromTimeLine(controller.selectedActionVMInTimeline);
            }

            event.accepted = true;
        }
    }

    //deselect action VM
    onFocusChanged: {
        if (!focus) {
            if (controller && controller.selectedActionVMInTimeline) {
                controller.selectedActionVMInTimeline = null;
            }
        }
    }

    //
    // Functions
    //
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

        height : actionVMItem.lineHeight/2
        color : MasticTheme.blueGreyColor2

        border {
            width : (controller && controller.selectedActionVMInTimeline && actionVMItem.myActionVM  && controller.selectedActionVMInTimeline === actionVMItem.myActionVM) ? 1 :0;
            color : MasticTheme.orangeColor
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
                    x : viewController.convertDurationInMillisecondsToLengthInCoordinateSystem(model.executionTime, viewController.pixelsPerMinute) - width/2;

                    onXChanged: {
                        if (visible) {
                            executionsItem.minimunXValueInExecutionList = Math.min(executionsItem.minimunXValueInExecutionList, notRevertItem.x);
                        }
                    }

                    anchors {
                        verticalCenter: parent.verticalCenter
                    }

                    visible : !model.shallRevert

                    svgFileCache : MasticTheme.svgFileMASTIC;
                    svgElementId:  (model.neverExecuted)?
                                       "notExcutedAction"
                                     : ((model.isExecuted) ? "timelineAction" : "currentAction");
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
                        color : (model.neverExecuted)? MasticTheme.darkGreyColor : MasticTheme.whiteColor;

                    }

                    I2SvgItem {
                        id : actionExecution
                        x : viewController.convertDurationInMillisecondsToLengthInCoordinateSystem(model.executionTime, viewController.pixelsPerMinute);
                        y : 0

                        svgFileCache : MasticTheme.svgFileMASTIC;
                        svgElementId: (model.neverExecuted)?
                                          "notExecutedRevertAction"
                                        : ((model.isExecuted) ? "revertAction" : "currentRevertAction");
                    }

                    I2SvgItem {
                        id : revertActionExecution
                        x : viewController.convertDurationInMillisecondsToLengthInCoordinateSystem(model.reverseTime, viewController.pixelsPerMinute) - width;
                        y : 0
                        rotation : 180
                        svgFileCache : MasticTheme.svgFileMASTIC;
                        svgElementId: (model.neverExecuted)?
                                          "notExecutedRevertAction"
                                        : ((model.isExecuted) ? "revertAction" : "currentRevertAction");
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
        color : MasticTheme.blackColor
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
        color : (controller && controller.selectedActionVMInTimeline && actionVMItem.myActionVM  && controller.selectedActionVMInTimeline === actionVMItem.myActionVM) ?
                    actionVMMouseArea.pressed? MasticTheme.lightGreyColor : MasticTheme.orangeColor
        : actionVMMouseArea.pressed? MasticTheme.lightGreyColor : MasticTheme.darkGreyColor

        text : (actionVMItem.myActionVM && actionVMItem.myActionVM.modelM) ? actionVMItem.myActionVM.modelM.name : ""
        font {
            family : MasticTheme.textFontFamily
            pixelSize: 11
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
        drag.target: !controller.isPlaying ? actionVMItem : null
        cursorShape: (actionVMMouseArea.drag.active)? Qt.PointingHandCursor : Qt.OpenHandCursor //Qt.OpenHandCursor

        onPressed: {
        }

        onPositionChanged: {
            itemDragged.x = mouseX - 12 - itemDragged.width + actionVMMouseArea.x;
            itemDragged.y = mouseY - 12 - itemDragged.height;
        }

        onReleased: {
            actionVMItem.Drag.drop();

            //
            // Reset the position of our action
            //
            if (actionVMItem.myActionVM) {
                actionVMItem.x = viewController.convertTimeInMillisecondsToAbscissaInCoordinateSystem(myActionVM.startTime, viewController.pixelsPerMinute);
                actionVMItem.y = (actionVMItem.lineHeight * myActionVM.lineInTimeLine);
            }
        }


        onClicked: {
            actionVMItem.forceActiveFocus()

            // selection of the action VM
            if (controller && actionVMItem.myActionVM) {
                if (controller.selectedActionVMInTimeline !== actionVMItem.myActionVM) {
                    controller.selectedActionVMInTimeline = actionVMItem.myActionVM;
                }
                else {
                    controller.selectedActionVMInTimeline = null;
                }
            }
        }

        onDoubleClicked: {
            // open action editor
            if (controller && actionVMItem.myActionVM) {
                controller.openActionEditorFromActionVM(actionVMItem.myActionVM);
                // selection of the action VM
                controller.selectedActionVMInTimeline = actionVMItem.myActionVM;
            }
        }
    }


    // ghost appearing when the action vm is dragging in the timeline
    I2CustomRectangle{
        id : itemDragged
        height : columnText.height + 8
        width : nameAction.width + 14
        color : MasticTheme.darkBlueGreyColor
        visible: actionVMMouseArea.drag.active

        // - fuzzy radius around our rectangle
        fuzzyRadius: 2
        fuzzyColor : MasticTheme.blackColor

        Column {
            id : columnText
            height : (nameAction.height + temporaryStartTimeAction.height) + 3

            anchors.centerIn: parent
            spacing: 6

            Text {
                id : nameAction
                color : MasticTheme.lightGreyColor
                text : (actionVMItem.myActionVM && actionVMItem.myActionVM.modelM) ? actionVMItem.myActionVM.modelM.name : ""
                anchors.horizontalCenter: parent.horizontalCenter
                horizontalAlignment: Text.AlignHCenter

                font {
                    family : MasticTheme.textFontFamily
                    pixelSize: 14
                }
            }

            Text {
                id : temporaryStartTimeAction
                visible: text !== ""
                color : MasticTheme.lightGreyColor
                text : actionVMItem.temporaryStartTime?
                           actionVMItem.temporaryStartTime.toLocaleString(Qt.locale(),"hh:mm:ss.zzz")
                         : "";

                anchors.horizontalCenter: parent.horizontalCenter
                horizontalAlignment: Text.AlignHCenter

                font {
                    family : MasticTheme.textFontFamily
                    pixelSize: 14
                }
            }
        }

    }


}

