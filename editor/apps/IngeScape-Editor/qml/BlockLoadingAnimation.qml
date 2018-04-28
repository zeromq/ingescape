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
    property int horizontalBlockCount: 5

    // Number of vertical blocks
    property int verticalBlockCount: 7

    // Duration of an animation step in milliseconds
    property int animationStepDurationInMilliseconds: 200

    // Size of of block in pixels
    property int blockSize: 50

    // Radius of a block in pixels
    property int blockRadius: 3

    // Spacing between blocks in pixels
    property int blockSpacing: 10

    // Color of a block
    property color blockColor: "white"

    // Visibility flags of blocks column by colum (top-down, left-right)
    // NB: the array must have verticalBlockCount * horizontalBlockCount values
    property var visibilitiesOfBlock: [
                                       true, true, true, true, true,
                                       true, true, true, true, true,
                                       true, true, true, true, true,
                                       true, true, true, true, true,
                                       true, true, true, true, true,
                                       true, true, true, true, true,
                                       true, true, true, true, true
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

                            color: rootItem.blockColor

                            smooth: true

                            visible: (rootItem.visibilitiesOfBlock.length - 1 >= _blockIndex) ? rootItem.visibilitiesOfBlock[_blockIndex] : false
                        }


                        transformOrigin: Item.BottomRight

                        SequentialAnimation {
                            loops: Animation.Infinite
                            running: rect.visible

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

