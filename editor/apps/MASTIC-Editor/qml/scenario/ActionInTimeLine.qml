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


    x : myActionVM? viewController.convertTimeInMillisecondsToAbscissaInCoordinateSystem(myActionVM.startTime, viewController.pixelsPerMinute) : 0;
    y : myActionVM? (actionVMItem.lineHeight * myActionVM.lineInTimeLine) : à;
    height : actionVMItem.lineHeight
    width : if (myActionVM && myActionVM.actionModel) {
                switch (myActionVM.actionModel.validityDurationType)
                {
                case ValidationDurationType.IMMEDIATE:
                    0;
                    break;
                case ValidationDurationType.FOREVER:
                    (viewController.timeTicksTotalWidth - viewController.convertTimeInMillisecondsToAbscissaInCoordinateSystem(myActionVM.startTime, viewController.pixelsPerMinute))
                    break;
                case ValidationDurationType.CUSTOM:
                    viewController.convertDurationInMillisecondsToLengthInCoordinateSystem(myActionVM.actionModel.validityDuration, viewController.pixelsPerMinute)
                    break;
                default:
                    0
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


    // rectangle representing the validity conditions time
    Rectangle {
        id : conditionsValidityRect
        anchors {
            top : parent.top
            left : parent.left
            right : parent.right
        }

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



    //            MouseArea {
    //                x : model.shallRevert? revertItem.x : notRevertItem.x;
    //                anchors {
    //                    top : parent.top
    //                    bottom : parent.bottom
    //                    bottomMargin: - actionVMItem.lineHeight/2
    //                }
    //                width : parent.width

    //            }
            }
        }

    }

    // Action Name
    Rectangle {
        id : backgroundActionName
        anchors {
            fill : actionName
            leftMargin:-1
            rightMargin:-1
        }
        color : MasticTheme.blackColor
    }

    Text {
        id: actionName
        anchors {
            top : parent.verticalCenter
            bottomMargin: 1
            bottom : parent.bottom
            left : parent.left
        }
        verticalAlignment: Text.AlignVCenter
        color : (controller && controller.selectedActionVMInTimeline && actionVMItem.myActionVM  && controller.selectedActionVMInTimeline === actionVMItem.myActionVM) ?
                    openEditorMouseArea.pressed? MasticTheme.lightGreyColor : MasticTheme.orangeColor
        : openEditorMouseArea.pressed? MasticTheme.lightGreyColor : MasticTheme.darkGreyColor

        text : (actionVMItem.myActionVM && actionVMItem.myActionVM.actionModel) ? actionVMItem.myActionVM.actionModel.name : ""
        font {
            family : MasticTheme.textFontFamily
            pixelSize: 11
        }
    }

    MouseArea {
        id : openEditorMouseArea
        anchors {
            top : actionVMItem.top
            bottom : actionVMItem.bottom
            left : actionVMItem.left
            leftMargin: (actionVMItem.myActionVM && actionVMItem.myActionVM.actionModel && !actionVMItem.myActionVM.actionModel.shallRevert) ? -5 : 0
        }

        width : Math.max(backgroundActionName.width, actionVMItem.width, executionsItem.width)
        hoverEnabled: true
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

}

