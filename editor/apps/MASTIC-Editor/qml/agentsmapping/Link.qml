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
 *
 */

import QtQuick 2.8

import I2Quick 1.0

import MASTIC 1.0



I2CubicBezierCurve {
    id: rootItem


    //--------------------------------
    //
    // Properties
    //
    //--------------------------------

    property var mapBetweenIOPVM : null
    property var controller : null
    property var inputModel : mapBetweenIOPVM ?  mapBetweenIOPVM.pointTo : null
    property var outputModel : mapBetweenIOPVM ?  mapBetweenIOPVM.pointFrom : null


    secondPoint: inputModel? inputModel.position : Qt.point(0,0)
    firstPoint: outputModel? outputModel.position : Qt.point(0,0)


    // Minimum offset of control point
    property int minimumControlPointOffset: 20

    // Holds whether hover events are handled
    property alias hoverEnabled: mouseArea.hoverEnabled


    // Default state
    property color defaultColor: MasticTheme.agentsMappingLinkDefaultColor

    // Hover state
    property int hoverFuzzyRadius: MasticTheme.agentsMappingLinkHoverFuzzyRadius
    property color hoverFuzzyColor: MasticTheme.agentsMappingLinkHoverFuzzyColor

    // Press state
    property color pressColor: MasticTheme.agentsMappingLinkPressColor


    // NB: Clip MUST be true to clip our mousearea
    clip: true


    // if the agentTo and agentFrom are reduced : global type of its inputs
    stroke: if (mapBetweenIOPVM && mapBetweenIOPVM.agentTo && mapBetweenIOPVM.agentTo.isReduced && mapBetweenIOPVM.agentFrom && mapBetweenIOPVM.agentFrom.isReduced
                    && outputModel)
            {
                switch (mapBetweenIOPVM.agentFrom.reducedMapValueTypeGroupInOutput)
                {
                case AgentIOPValueTypeGroups.NUMBER:
                    outputModel.isPublishedNewValue ? MasticTheme.orangeColor2 : MasticTheme.darkOrangeColor2
                    break;
                case AgentIOPValueTypeGroups.STRING:
                    outputModel.isPublishedNewValue ? MasticTheme.redColor2 : MasticTheme.darkRedColor2
                    break;
                case AgentIOPValueTypeGroups.IMPULSION:
                    outputModel.isPublishedNewValue ? MasticTheme.purpleColor : MasticTheme.darkPurpleColor
                    break;
                case AgentIOPValueTypeGroups.DATA:
                    outputModel.isPublishedNewValue ? MasticTheme.greenColor : MasticTheme.darkGreenColor
                    break;
                case AgentIOPValueTypeGroups.MIXED:
                    outputModel.isPublishedNewValue ? MasticTheme.whiteColor : MasticTheme.darkGreyColor
                    break;
                case AgentIOPValueTypeGroups.UNKNOWN:
                    "#000000"
                    break;
                default:
                    defaultColor;
                    break;
                }
            }
            else {
                // if the agentTo is not reduced : type of its output
                if (outputModel && outputModel.firstModel)
                {
                    switch (outputModel.firstModel.agentIOPValueTypeGroup)
                    {
                    case AgentIOPValueTypeGroups.NUMBER:
                        outputModel.isPublishedNewValue ? MasticTheme.orangeColor2 : MasticTheme.darkOrangeColor2
                        break;
                    case AgentIOPValueTypeGroups.STRING:
                        outputModel.isPublishedNewValue ? MasticTheme.redColor2 : MasticTheme.darkRedColor2
                        break;
                    case AgentIOPValueTypeGroups.IMPULSION:
                        outputModel.isPublishedNewValue ? MasticTheme.purpleColor : MasticTheme.darkPurpleColor
                        break;
                    case AgentIOPValueTypeGroups.DATA:
                        outputModel.isPublishedNewValue ? MasticTheme.greenColor : MasticTheme.darkGreenColor
                        break;
                    case AgentIOPValueTypeGroups.MIXED:
                        outputModel.isPublishedNewValue ? MasticTheme.whiteColor : MasticTheme.darkGreyColor
                        break;
                    case AgentIOPValueTypeGroups.UNKNOWN:
                        "#000000"
                        break;
                    default:
                        MasticTheme.whiteColor;
                        break;
                    }
                }
                else {
                    defaultColor;
                }
            }


    strokeWidth: if (mapBetweenIOPVM && mapBetweenIOPVM.agentTo && mapBetweenIOPVM.agentTo.isReduced
                         && mapBetweenIOPVM.agentFrom && mapBetweenIOPVM.agentFrom.isReduced) {
                     MasticTheme.agentsMappingBrinDefaultWidth
                 }
                 else {
                     MasticTheme.agentsMappingLinkDefaultWidth
                 }



    // Fuzzy contour
    fuzzyColor: (mapBetweenIOPVM && controller.selectedMapBetweenIOP === mapBetweenIOPVM) ? MasticTheme.lightGreyColor : "transparent"
    fuzzyRadius: 2

    opacity: mouseArea.pressed ? 0.8 : 1;


    //--------------------------------
    //
    // Behaviors
    //
    //--------------------------------
    Behavior on stroke {
        ColorAnimation {
        }
    }


    //--------------------------------
    //
    // Signals
    //
    //--------------------------------

    // Emitted when there is a click
    signal clicked();

    onClicked: {
        if (controller) {
            if (controller.selectedMapBetweenIOP !== model.QtObject) {
                controller.selectedMapBetweenIOP = model.QtObject;
            } else {
                controller.selectedMapBetweenIOP = null;
            }
        }
    }

    //--------------------------------
    //
    // Functions
    //
    //--------------------------------


    //
    // Compute the control points of our cubic Bézier curve based on its two extremities
    //
    function updateControlPoints()
    {
        var dx = secondPoint.x - firstPoint.x;
        var dy = secondPoint.y - firstPoint.y;
        var offsetX = Math.max(Math.abs(0.5 * dx), rootItem.minimumControlPointOffset);
        var offsetY = Math.max(Math.abs(0.5 * dy), rootItem.minimumControlPointOffset);

        if (dx > 0)
        {
            offsetY = 0;
        }
        else
        {
            offsetX = Math.max(offsetX, offsetY);

            if (dy < 0)
            {
                offsetY = -offsetY;
            }
        }

        firstControlPoint = Qt.point(firstPoint.x + offsetX, firstPoint.y + offsetY);
        secondControlPoint = Qt.point(secondPoint.x - offsetX, secondPoint.y - offsetY );
    }



    //--------------------------------
    //
    // Behaviors
    //
    //--------------------------------


    // Update control points when our first point has changed
    onFirstPointChanged: updateControlPoints();

    // Update control points when our second point has changed
    onSecondPointChanged: updateControlPoints();


    Component.onCompleted: {
        // Compute control points when our item is created
        updateControlPoints();
    }



    Behavior on fuzzyRadius {
        NumberAnimation {}
    }


    focus: true
    Keys.onPressed: {
        if (event.key === Qt.Key_Backspace || event.key === Qt.Key_Delete)
        {
            if (controller && controller.selectedMapBetweenIOP) {
                controller.deleteLinkBetweenTwoAgents(controller.selectedMapBetweenIOP);
            }

            event.accepted = true;
        }
    }

    onFocusChanged: {
        if (!focus) {
            if (controller && controller.selectedMapBetweenIOP) {
                controller.selectedMapBetweenIOP = null;
            }
        }
    }

    //--------------------------------
    //
    // Content
    //
    //--------------------------------


    MouseArea {
        id: mouseArea

        anchors {
            fill: parent
        }

        enabled: rootItem.visible
        hoverEnabled: enabled

        onClicked: {
            rootItem.clicked();
            rootItem.forceActiveFocus();
        }
    }
}
