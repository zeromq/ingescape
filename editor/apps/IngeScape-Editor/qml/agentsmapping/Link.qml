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
 *
 */

import QtQuick 2.8

import I2Quick 1.0

import INGESCAPE 1.0



I2CubicBezierCurve {
    id: rootItem


    //--------------------------------
    //
    // Properties
    //
    //--------------------------------

    property var controller: null;

    property var viewModel: null;
    property var linkInput: viewModel ? viewModel.linkInput : null;
    property var linkOutput: viewModel ? viewModel.linkOutput : null;
    //property var inputModel: (linkInput && linkInput.input) ? linkInput.input.firstModel : null;
    property var outputModel: (linkOutput && linkOutput.output) ? linkOutput.output.firstModel : null;


    secondPoint: linkInput ? linkInput.position : Qt.point(0,0)
    firstPoint: linkOutput ? linkOutput.position : Qt.point(0,0)


    // Minimum offset of control points
    property int minimumControlPointOffset: 25

    // Minimum delta-Y to render a backward S shape instead of a loop (C shape)
    property int minimumDeltaYforBackwardSshape: 125

    // Holds whether hover events are handled
    property alias hoverEnabled: mouseArea.hoverEnabled


    // Default state
    property color defaultColor: IngeScapeTheme.agentsMappingLinkDefaultColor

    // Hover state
    property int hoverFuzzyRadius: IngeScapeTheme.agentsMappingLinkHoverFuzzyRadius
    property color hoverFuzzyColor: IngeScapeTheme.agentsMappingLinkHoverFuzzyColor

    // Press state
    property color pressColor: IngeScapeTheme.agentsMappingLinkPressColor


    // Check if we represent a link or a "brin"
    property bool _isBrin: (viewModel
                            && viewModel.inputAgent && viewModel.inputAgent.isReduced
                            && viewModel.outputAgent && viewModel.outputAgent.isReduced)


    // NB: Clip MUST be true to clip our mouse area
    clip: true

    // allowing to increase mouse area
    hitTestAreaMargin : 3

    // if the inputAgent and outputAgent are reduced : global type of its outputs
    stroke: if (rootItem._isBrin && linkOutput && viewModel && viewModel.outputAgent)
            {
                if (linkOutput.isPublishedNewValue && IngeScapeEditorC.modelManager.isMappingActivated) {
                    IngeScapeTheme.colorOfIOPTypeWithConditions(viewModel.outputAgent.reducedLinkOutputsValueTypeGroup, true);
                }
                else {
                    IngeScapeTheme.colorOfIOPTypeWithConditions(viewModel.outputAgent.reducedLinkOutputsValueTypeGroup, false);
                }
            }
            else
            {
                // if the inputAgent is not reduced : type of its output
                if (linkOutput && outputModel)
                {
                    if (linkOutput.isPublishedNewValue && IngeScapeEditorC.modelManager.isMappingActivated) {
                        IngeScapeTheme.colorOfIOPTypeWithConditions(outputModel.agentIOPValueTypeGroup, true);
                    }
                    else {
                        IngeScapeTheme.colorOfIOPTypeWithConditions(outputModel.agentIOPValueTypeGroup, false);
                    }
                }
                else {
                    defaultColor;
                }
            }


    strokeWidth: rootItem._isBrin ? IngeScapeTheme.agentsMappingBrinDefaultWidth : IngeScapeTheme.agentsMappingLinkDefaultWidth

    strokeDashArray: (viewModel && viewModel.isVirtual) ? (rootItem._isBrin ? IngeScapeTheme.agentsMappingBrinVirtualStrokeDashArray : IngeScapeTheme.agentsMappingLinkVirtualStrokeDashArray)
                                                        : ""

    // Fuzzy contour
    fuzzyColor: IngeScapeTheme.lightGreyColor
    fuzzyRadius: (viewModel && controller && (controller.selectedLink === viewModel)) ? 2 : 0

    opacity: mouseArea.pressed ? 0.8 : 1

    z: (linkOutput && linkOutput.isPublishedNewValue) ? 1 : 0


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


    //
    // Compute the control points of our cubic Bézier curve based on its two extremities
    //
    function updateControlPoints()
    {
        var dx = secondPoint.x - firstPoint.x;
        var dy = secondPoint.y - firstPoint.y;
        var offsetX = Math.max(Math.abs(0.5 * dx), rootItem.minimumControlPointOffset);
        var offsetY = Math.max(Math.abs(0.5 * dy), rootItem.minimumControlPointOffset);

        var offsetX1 = offsetX;
        var offsetX2 = -offsetX1;
        var offsetY1 = offsetY;
        var offsetY2 = -offsetY1;

        if (dx > 0)
        {
            offsetY1 = offsetY2 = 0;
        }
        else
        {
            offsetX1 = Math.max(offsetX, offsetY);
            offsetX2 = -offsetX1;

            if (Math.abs(dy) <= rootItem.minimumDeltaYforBackwardSshape)
            {
                // C shape (loop)
                offsetY = 1.5 * rootItem.minimumDeltaYforBackwardSshape;

                if (dy <= 0)
                {
                    offsetY = -offsetY;
                }

                offsetY1 = offsetY2 = offsetY;
            }
            else
            {
                // S shape
                if (dy < 0)
                {
                    offsetY = -offsetY;
                }

                offsetY1 = offsetY;
                offsetY2 = -offsetY1;
            }
        }

        firstControlPoint = Qt.point(firstPoint.x + offsetX1, firstPoint.y + offsetY1);
        secondControlPoint = Qt.point(secondPoint.x + offsetX2, secondPoint.y + offsetY2);
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


    // Animate stroke changes
    Behavior on stroke {
        ColorAnimation {
        }
    }


    // Animate fuzzyRadius changes
    Behavior on fuzzyRadius {
        NumberAnimation {}
    }


    focus: true

    // Remove selected map between IOP
    Keys.onPressed: {
        if (event.key === Qt.Key_Backspace || event.key === Qt.Key_Delete)
        {
            if (controller && controller.selectedLink) {
                controller.removeLinkBetweenTwoAgents(controller.selectedLink);
            }

            event.accepted = true;
        }
    }

    // UNselect map between IOP
    onFocusChanged: {
        if (!focus) {
            if (controller && controller.selectedLink) {
                controller.selectedLink = null;
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
            rootItem.forceActiveFocus();
            if (controller) {
                if (controller.selectedLink !== model.QtObject) {
                    controller.selectedLink = model.QtObject;
                }
                else {
                    controller.selectedLink = null;
                }
            }
        }
    }
}
