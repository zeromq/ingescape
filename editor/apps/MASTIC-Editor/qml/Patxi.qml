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
 *      Alexandre Lemort <lemort@ingenuity.io>
 *
 */

import QtQuick 2.8


/**
 * Inspired by https://dribbble.com/shots/3250272-Animated-Loader-Principle-Freebie
 */
Item {
    id: rootItem


    //--------------------------------
    //
    // Properties
    //
    //--------------------------------

    // Number of horizontal blocks
    readonly property int horizontalBlockCount: 24

    // Number of vertical blocks
    readonly property int verticalBlockCount: 14

    // Duration of an animation step in milliseconds
    property int animationStepDurationInMilliseconds: 150

    // Size of of block
    property int blockSize: 15

    // Radius of a block
    property int blockRadius: 2

    // Spacing between blocks
    property int blockSpacing: 1

    // colors of blocks column by colum (top-down, left-right)  #00932B
    readonly property var colorsOfBlocks: [
"#009543", "#009543", "#009543", "#DC241F", "#DC241F", "#DC241F", "#DC241F", "#DC241F", "#DC241F", "#DC241F", "#DC241F", "#FFFFFF", "#FFFFFF", "#DC241F", "#DC241F", "#DC241F", "#DC241F", "#DC241F", "#DC241F", "#DC241F", "#DC241F", "#009543", "#009543", "#009543",
"#009543", "#009543", "#009543", "#009543", "#009543", "#DC241F", "#DC241F", "#DC241F", "#DC241F", "#DC241F", "#DC241F", "#FFFFFF", "#FFFFFF", "#DC241F", "#DC241F", "#DC241F", "#DC241F", "#DC241F", "#DC241F", "#009543", "#009543", "#009543", "#009543", "#009543",
"#DC241F", "#DC241F", "#009543", "#009543", "#009543", "#009543", "#009543", "#DC241F", "#DC241F", "#DC241F", "#DC241F", "#FFFFFF", "#FFFFFF", "#DC241F", "#DC241F", "#DC241F", "#DC241F", "#009543", "#009543", "#009543", "#009543", "#009543", "#DC241F", "#DC241F",
"#DC241F", "#DC241F", "#DC241F", "#DC241F", "#009543", "#009543", "#009543", "#009543", "#009543", "#DC241F", "#DC241F", "#FFFFFF", "#FFFFFF", "#DC241F", "#DC241F", "#009543", "#009543", "#009543", "#009543", "#009543", "#DC241F", "#DC241F", "#DC241F", "#DC241F",
"#DC241F", "#DC241F", "#DC241F", "#DC241F", "#DC241F", "#DC241F", "#009543", "#009543", "#009543", "#009543", "#009543", "#FFFFFF", "#FFFFFF", "#009543", "#009543", "#009543", "#009543", "#009543", "#DC241F", "#DC241F", "#DC241F", "#DC241F", "#DC241F", "#DC241F",
"#DC241F", "#DC241F", "#DC241F", "#DC241F", "#DC241F", "#DC241F", "#DC241F", "#DC241F", "#009543", "#009543", "#009543", "#FFFFFF", "#FFFFFF", "#009543", "#009543", "#009543", "#DC241F", "#DC241F", "#DC241F", "#DC241F", "#DC241F", "#DC241F", "#DC241F", "#DC241F",
"#FFFFFF", "#FFFFFF", "#FFFFFF", "#FFFFFF", "#FFFFFF", "#FFFFFF", "#FFFFFF", "#FFFFFF", "#FFFFFF", "#FFFFFF", "#FFFFFF", "#FFFFFF", "#FFFFFF", "#FFFFFF", "#FFFFFF", "#FFFFFF", "#FFFFFF", "#FFFFFF", "#FFFFFF", "#FFFFFF", "#FFFFFF", "#FFFFFF", "#FFFFFF", "#FFFFFF",
"#FFFFFF", "#FFFFFF", "#FFFFFF", "#FFFFFF", "#FFFFFF", "#FFFFFF", "#FFFFFF", "#FFFFFF", "#FFFFFF", "#FFFFFF", "#FFFFFF", "#FFFFFF", "#FFFFFF", "#FFFFFF", "#FFFFFF", "#FFFFFF", "#FFFFFF", "#FFFFFF", "#FFFFFF", "#FFFFFF", "#FFFFFF", "#FFFFFF", "#FFFFFF", "#FFFFFF",
"#DC241F", "#DC241F", "#DC241F", "#DC241F", "#DC241F", "#DC241F", "#DC241F", "#DC241F", "#009543", "#009543", "#009543", "#FFFFFF", "#FFFFFF", "#009543", "#009543", "#009543", "#DC241F", "#DC241F", "#DC241F", "#DC241F", "#DC241F", "#DC241F", "#DC241F", "#DC241F",
"#DC241F", "#DC241F", "#DC241F", "#DC241F", "#DC241F", "#DC241F", "#009543", "#009543", "#009543", "#009543", "#009543", "#FFFFFF", "#FFFFFF", "#009543", "#009543", "#009543", "#009543", "#009543", "#DC241F", "#DC241F", "#DC241F", "#DC241F", "#DC241F", "#DC241F",
"#DC241F", "#DC241F", "#DC241F", "#DC241F", "#009543", "#009543", "#009543", "#009543", "#009543", "#DC241F", "#DC241F", "#FFFFFF", "#FFFFFF", "#DC241F", "#DC241F", "#009543", "#009543", "#009543", "#009543", "#009543", "#DC241F", "#DC241F", "#DC241F", "#DC241F",
"#DC241F", "#DC241F", "#009543", "#009543", "#009543", "#009543", "#009543", "#DC241F", "#DC241F", "#DC241F", "#DC241F", "#FFFFFF", "#FFFFFF", "#DC241F", "#DC241F", "#DC241F", "#DC241F", "#009543", "#009543", "#009543", "#009543", "#009543", "#DC241F", "#DC241F",
"#009543", "#009543", "#009543", "#009543", "#009543", "#DC241F", "#DC241F", "#DC241F", "#DC241F", "#DC241F", "#DC241F", "#FFFFFF", "#FFFFFF", "#DC241F", "#DC241F", "#DC241F", "#DC241F", "#DC241F", "#DC241F", "#009543", "#009543", "#009543", "#009543", "#009543",
"#009543", "#009543", "#009543", "#DC241F", "#DC241F", "#DC241F", "#DC241F", "#DC241F", "#DC241F", "#DC241F", "#DC241F", "#FFFFFF", "#FFFFFF", "#DC241F", "#DC241F", "#DC241F", "#DC241F", "#DC241F", "#DC241F", "#DC241F", "#DC241F", "#009543", "#009543", "#009543"
    ]



    // Compute width and height
    width: (horizontalBlockCount + 1) * blockSize + horizontalBlockCount * blockSpacing
    height: (verticalBlockCount + 1) * blockSize + verticalBlockCount * blockSpacing



    //--------------------------------
    //
    // Content
    //
    //--------------------------------

    // Create columns one by one
    Row {
        spacing: rootItem.blockSpacing

        Repeater {
            id: rowRepeater

            model: rootItem.horizontalBlockCount

            delegate: Column {
                id: columnItem

                width: rootItem.blockSize
                spacing: rootItem.blockSpacing


                // Save index of our column
                readonly property int _columnIndex: index
                readonly property int _maxMinusIndex: rootItem.horizontalBlockCount - 1  - index


                // Create blocks of our column
                Repeater {
                    model: rootItem.verticalBlockCount

                    delegate: Item {
                        id: block

                        readonly property int _blockIndex: columnItem._columnIndex + index * rootItem.horizontalBlockCount

                        width: rootItem.blockSize
                        height: rootItem.blockSize

                        Rectangle {
                            id: rect

                            anchors.fill: parent

                            radius: rootItem.blockRadius

                            smooth: true

                            color: (rootItem.colorsOfBlocks.length - 1 >= _blockIndex) ? rootItem.colorsOfBlocks[_blockIndex] : "transparent"
                        }


                        transformOrigin: Item.BottomRight

                        SequentialAnimation {
                            loops: Animation.Infinite
                            running: true

                            RotationAnimator {
                                target: block
                                duration: columnItem._maxMinusIndex * rootItem.animationStepDurationInMilliseconds
                                from: 0
                                to: 0
                            }

                            RotationAnimator {
                                target: block
                                duration: rootItem.animationStepDurationInMilliseconds * 4
                                from: 0
                                to: 90
                                easing.type: Easing.InOutQuint
                            }

                            RotationAnimator {
                                target: block
                                duration: columnItem._columnIndex * rootItem.animationStepDurationInMilliseconds * 2
                                from: 90
                                to: 90
                            }

                            RotationAnimator {
                                target: block
                                duration: rootItem.animationStepDurationInMilliseconds * 4
                                from: 90
                                to: 0
                                easing.type: Easing.InOutQuint
                            }

                            RotationAnimator {
                                target: block
                                duration: columnItem._maxMinusIndex * rootItem.animationStepDurationInMilliseconds
                                from: 0
                                to: 0
                            }
                        }
                    }
                }
            }
        }
    }
}

