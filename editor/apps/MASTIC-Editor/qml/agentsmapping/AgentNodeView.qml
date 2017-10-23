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

    property bool isClosed : false

    width : 208
    height : (rootItem.agentVM & !rootItem.isClosed)?
                 (52 + 20*Math.max(rootItem.agentVM.inputsList.count , rootItem.agentVM.outputsList.count))
                : 40


    // Init position of our agent
    x: (agentVM && agentVM.position) ? agentVM.position.x : 0
    y: (agentVM && agentVM.position) ? agentVM.position.y : 0


    //
    // Bindings to save the position of our agent
    //
    Binding {
        target: rootItem.agentVM
        property: "position"
        value: Qt.point(rootItem.x,rootItem.y)
    }
    //    // - Abscisse
    //    Binding {
    //        target: rootItem.agentVM
    //        property: "x"
    //        value: rootItem.x
    //    }

    //    // - Ordinate
    //    Binding {
    //        target: rootItem.agentVM
    //        property: "y"
    //        value: rootItem.y
    //    }


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

    Rectangle {
        anchors.fill: parent

        color : MasticTheme.darkGreyColor
        radius : 6

        // Agent Name
        Text {
            id : agentName
            anchors {
                left : parent.left
                leftMargin: 20
                right : parent.right
                rightMargin: 20
                verticalCenter: parent.top
                verticalCenterOffset: 20
            }


            elide: Text.ElideRight
            text : agentVM ? agentVM.agentName : ""

            color : MasticTheme.agentsNameMappingColor
            font: MasticTheme.headingFont

        }

        //Separator
        Rectangle {
            id : separator
            anchors {
                left : parent.left
                leftMargin: 18
                right : parent.right
                rightMargin: 18
                verticalCenter : parent.top
                verticalCenterOffset: 40
            }

            height : 2
            color : agentName.color

            visible : !rootItem.isClosed
        }




        //
        //
        // Inlets / Input slots
        //
        Column {
            id: columnInputSlots

            anchors {
                left: parent.left
                right : parent.right
                top: separator.bottom
                topMargin: 5
                bottom: parent.bottom
            }


            Repeater {
                // List of intput slots VM
                model: rootItem.agentVM? rootItem.agentVM.inputsList : 0

                delegate: Item {
                    id: inputSlotItem

                    property var myModel: model.QtObject

                    height : 20
                    anchors {
                        left : parent.left
                        right : parent.right
                    }

                    Text {
                        id : agentInput
                        anchors {
                            left : parent.left
                            leftMargin: 20
                            right : parent.right
                            rightMargin: 20
                            verticalCenter: parent.verticalCenter
                        }
                        elide: Text.ElideRight
                        text : myModel.iopModel ? myModel.iopModel.name : ""

                        color : MasticTheme.agentsInputsOutputsMappingColor
                        font: MasticTheme.heading2Font
                    }

                    //
                    // Bindings to save the anchor point of our input slot
                    // i.e. the point used to draw a link
                    //
                    // - Abscisse
//                    Binding {
//                        target: myModel

//                        property: "x"

//                        // Two options: absolute value OR relative value
//                        value: rootItem.x + inputSlotItem.x
//                    }

//                    // - Ordinate
//                    Binding {
//                        target: myModel

//                        property: "y"

//                        // Two options: absolute value OR relative value
//                        value: rootItem.y + inputSlotItem.y
//                    }
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
}
