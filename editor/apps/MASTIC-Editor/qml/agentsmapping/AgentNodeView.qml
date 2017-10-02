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


Item {
    id: rootItem


    //--------------------------------
    //
    // Properties
    //
    //--------------------------------

    // Model associated to our QML item
    property var agentVM: null



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



    //--------------------------------
    //
    // Content
    //
    //--------------------------------



    //
    //
    // Inlets / Input slots
    //
    Column {
        id: columnInputSlots

        anchors {
            left: parent.left
            top: parent.top
            bottom: parent.bottom
        }


        Repeater {
            model: 0

            delegate: Item {
                id: inputSlotItem

                property var myModel: null


                //
                // Bindings to save the anchor point of our input slot
                //
                // - Abscissa
                Binding {
                    target: myModel

                    property: "x"

                    // Two options: absolute value OR relative value
                    value: rootItem.x + inputSlotItem.x
                }

                // - Ordinate
                Binding {
                    target: myModel

                    property: "y"

                    // Two options: absolute value OR relative value
                    value: rootItem.y + inputSlotItem.y
                }
            }
        }
    }



    //
    //
    // Outlets / Output slots
    //
    Column {
        id: columnOutputSlots

        anchors {
            right: parent.right
            top: parent.top
            bottom: parent.bottom
        }


        Repeater {
            model: 0

            delegate: Item {
                id: outputSlotItem

                property var myModel: null


                //
                // Bindings to save the anchor point of our input slot
                //
                // - Abscissa
                Binding {
                    target: myModel

                    property: "x"

                    // Two options: absolute value OR relative value
                    value: rootItem.x + outputSlotItem.x
                }

                // - Ordinate
                Binding {
                    target: myModel

                    property: "y"

                    // Two options: absolute value OR relative value
                    value: rootItem.y + outputSlotItem.y
                }
            }
        }
    }
}
