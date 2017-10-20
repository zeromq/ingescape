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



    // Init position of our agent
    x: (agentVM) ? agentVM.x : 0
    y: (agentVM) ? agentVM.y : 0


    //
    // Bindings to save the position of our agent
    //
    // - Abscissa
    Binding {
        target: rootItem.agentVM
        property: "x"
        value: rootItem.x
    }

    // - Ordinate
    Binding {
        target: rootItem.agentVM
        property: "y"
        value: rootItem.y
    }



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
            // List of intput slots VM
            model: 0

            delegate: Item {
                id: inputSlotItem

                property var myModel: null


                //
                // Bindings to save the anchor point of our input slot
                // i.e. the point used to draw a link
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
            // List of output slots VM
            model: 0

            delegate: Item {
                id: outputSlotItem

                property var myModel: null


                //
                // Bindings to save the anchor point of our input slot
                // i.e. the point used to draw a link
                //
                // - Abscissa
                Binding {
                    target: myModel

                    property: "x"

                    // Two options: absolute value OR relative value
                    value: rootItem.x + rootItem.width + outputSlotItem.x
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
