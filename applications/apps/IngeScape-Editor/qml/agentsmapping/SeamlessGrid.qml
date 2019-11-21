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


import QtQuick 2.0
import INGESCAPE 1.0

Item {
    id: root

    //--------------------------------
    //
    // Properties
    //
    //--------------------------------

    // Offset along the X-axis
    property int offsetX: 0

    // Offset along the Y-axis
    property int offsetY: 0

    // Size of cells (square cells)
    property int cellSize: 100

    // Size of points (cells' corners)
    property int pointSize : 2

    // Colors of points (cells' corners)
    property color pointColor : "#575757"


    //--------------------------------
    //
    // Inner properties (private)
    //
    //--------------------------------

    // Safe value of cell size
    property int _cellSize: Math.max(1, root.cellSize)

    // Delta along the X-axis
    property int _deltaX: ((root.offsetX % root._cellSize) + root._cellSize) % root._cellSize - root._cellSize

    // Delta along the X-axis
    property int _deltaY: ((root.offsetY % root._cellSize) + root._cellSize) % root._cellSize - root._cellSize


    //--------------------------------
    //
    // Content
    //
    //--------------------------------

    // Grid points are drawn thanks to DashedBar as vertical lines
    Item {
        x: root._deltaX
        y: root._deltaY

        height: root.height + root._cellSize

        Repeater {
            model : Math.ceil(root.width/root._cellSize) + 1

            DashedBar {
                anchors {
                    top: parent.top
                    bottom: parent.bottom
                }

                x: (index * root._cellSize)

                width: root.pointSize

                color: root.pointColor

                dashArray: root.pointSize + "," + root._cellSize
            }
        }
    }
}
