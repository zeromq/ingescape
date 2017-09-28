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


import QtQuick 2.0

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

    // Size of cells
    property int cellSize: 100

    // Number of subCells (must be greater or equal to 1)
    property int numberOfSubCells: 5

    // Stroke color of cells
    property color cellStroke: "#323741"

    // Stroke-width of cells
    property real cellStrokeWidth: 1

    // Stroke color of sub-cells
    property color subCellStroke: "#29313A"

    // Stroke-width of sub-cells
    property real subCellStrokeWidth: 1



    //--------------------------------
    //
    // Inner properties (private)
    //
    //--------------------------------

    // Safe value of cell size
    property int _cellSize: Math.max(1, root.cellSize)

    // Size of sub-cells
    property real _subCellSize: (root.numberOfSubCells !== 0) ? root._cellSize/root.numberOfSubCells : 0

    // Number of sub-cell lines
    property int _numberOfSubCellLines: Math.max(0, root.numberOfSubCells - 1)

    // Delta along the X-axis
    property int _deltaX: ((root.offsetX % root._cellSize) + root._cellSize) % root._cellSize - root._cellSize

    // Delta along the X-axis
    property int _deltaY: ((root.offsetY % root._cellSize) + root._cellSize) % root._cellSize - root._cellSize


    //--------------------------------
    //
    // Content
    //
    //--------------------------------


    //
    // Horizontal sub lines
    //
    Item {
        id: horizontalSubLines

        y: root._deltaY

        width: parent.width
        height: parent.height + root._cellSize

        Repeater {
            model: Math.ceil(root.height/root._cellSize) + 1

            delegate: Repeater {
                id: subLinesRepeater

                property int cellIndex: index

                model: root._numberOfSubCellLines

                delegate: Rectangle {
                    anchors {
                        left: parent.left
                        right: parent.right
                    }

                    y: (cellIndex * root._cellSize) + (index + 1) * root._subCellSize

                    height: root.subCellStrokeWidth

                    color: root.subCellStroke
                }
            }
        }
    }



    //
    // Vertical lines (main lines + sub lines)
    //
    Item {
        id: verticalLines

        x: root._deltaX

        width: parent.width + root._cellSize
        height: parent.height

        Repeater {
            model: Math.ceil(root.width/root._cellSize) + 1

            delegate: Rectangle {
                anchors {
                    top: parent.top
                    bottom: parent.bottom
                }

                x: (index * root._cellSize)

                width: root.cellStrokeWidth

                color: root.cellStroke

                Repeater {
                    model: root._numberOfSubCellLines

                    delegate: Rectangle {
                        anchors {
                            top: parent.top
                            bottom: parent.bottom
                        }

                        x: (index + 1) * root._subCellSize

                        width: root.subCellStrokeWidth

                        color: root.subCellStroke
                    }
                }
            }
        }
    }




    //
    // Horizontal main lines
    //
    Item {
        id: horizontalMainLines

        y: root._deltaY

        width: parent.width
        height: parent.height + root._cellSize

        Repeater {
            model: Math.ceil(root.height/root._cellSize) + 1

            delegate: Rectangle {
                anchors {
                    left: parent.left
                    right: parent.right
                }

                y: (index * root._cellSize)

                height: root.cellStrokeWidth

                color: root.cellStroke
            }
        }
    }



}
