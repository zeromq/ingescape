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


    // Stroke
    stroke: (mouseArea.pressed) ? pressColor : defaultColor
    strokeWidth: MasticTheme.agentsMappingLinkDefaultWidth


    // Fuzzy contour
    fuzzyColor: hoverFuzzyColor
    fuzzyRadius: (mouseArea.containsMouse) ? hoverFuzzyRadius : 0



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

    // Compute control points when our item is created
    Component.onCompleted: updateControlPoints();


    Behavior on fuzzyRadius {
        NumberAnimation {}
    }


    //--------------------------------
    //
    // Content
    //
    //--------------------------------


    MouseArea {
        id: mouseArea

        anchors.fill: parent

        hoverEnabled: true
    }
}
