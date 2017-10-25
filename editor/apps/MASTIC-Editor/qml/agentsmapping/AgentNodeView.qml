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

    width : 228
    height : (rootItem.agentVM && !rootItem.isClosed)?
                 (52 + 20*Math.max(rootItem.agentVM.inputsList.count , rootItem.agentVM.outputsList.count))
               : 40


    // Init position of our agent
    x: (agentVM && agentVM.position) ? agentVM.position.x : 0
    y: (agentVM && agentVM.position) ? agentVM.position.y : 0


    clip : true

    //
    // Bindings to save the position of our agent
    //
    Binding {
        target: rootItem.agentVM
        property: "position"
        value: Qt.point(rootItem.x,rootItem.y)
    }


    Behavior on height {
        NumberAnimation {
            onStopped: {
            }
        }
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

    Rectangle {
        anchors {
            fill: parent
            leftMargin: 10
            rightMargin: 10
        }

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
                model: (rootItem.agentVM && !rootItem.isClosed)? rootItem.agentVM.inputsList : 0

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
                            right : parent.horizontalCenter
                            rightMargin: 5
                            verticalCenter: parent.verticalCenter
                        }
                        elide: Text.ElideRight
                        text : myModel.iopModel ? myModel.iopModel.name : ""

                        color : MasticTheme.agentsInputsOutputsMappingColor
                        font: MasticTheme.heading2Font
                    }


                    Rectangle {
                        id : linkPoint

                        anchors {
                            horizontalCenter: parent.left
                            verticalCenter: parent.verticalCenter
                        }

                        height : 13
                        width : height
                        radius : height/2

                        color : MasticTheme.whiteColor
                    }


                    //
                    // Bindings to save the anchor point of our input slot
                    // i.e. the point used to draw a link
                    //
                    Binding {
                        target: myModel

                        property: "position"

                        value: Qt.point(rootItem.x + inputSlotItem.mapToItem(rootItem, linkPoint.x, linkPoint.y).x + linkPoint.width/2,
                                        rootItem.y + inputSlotItem.mapToItem(rootItem, linkPoint.x, linkPoint.y).y + linkPoint.height/2)
                    }


//                    Connections {
//                        target : myModel
//                        onPositionChanged : {
//                            console.log( " position changed input" + myModel.position.x + "   " + myModel.position.y)
//                        }
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
                left: parent.left
                right : parent.right
                top: separator.bottom
                topMargin: 5
                bottom: parent.bottom
            }

            Repeater {
                // List of output slots VM
                model: (rootItem.agentVM && !rootItem.isClosed)? rootItem.agentVM.outputsList : 0

                delegate: Item {
                    id: outputSlotItem

                    property var myModel: model.QtObject

                    height : 20
                    anchors {
                        left : parent.left
                        right : parent.right
                    }

                    Text {
                        id : agentOutput
                        anchors {
                            left : parent.horizontalCenter
                            leftMargin: 5
                            right : parent.right
                            rightMargin: 20
                            verticalCenter: parent.verticalCenter
                        }

                        horizontalAlignment : Text.AlignRight

                        elide: Text.ElideRight
                        text : myModel.iopModel ? myModel.iopModel.name : ""

                        color : MasticTheme.agentsInputsOutputsMappingColor
                        font: MasticTheme.heading2Font
                    }


                    Rectangle {
                        id : linkPointOut

                        anchors {
                            horizontalCenter: parent.right
                            verticalCenter: parent.verticalCenter
                        }

                        height : 13
                        width : height
                        radius : height/2

                        color : MasticTheme.whiteColor
                    }


                    //
                    // Bindings to save the anchor point of our input slot
                    // i.e. the point used to draw a link
                    //
                    Binding {
                        target: myModel

                        property: "position"

                        value: Qt.point(rootItem.x + outputSlotItem.mapToItem(rootItem, linkPointOut.x, linkPointOut.y).x + linkPointOut.width/2,
                                        rootItem.y + outputSlotItem.mapToItem(rootItem, linkPointOut.x, linkPointOut.y).y + linkPointOut.height/2)
                    }

//                    Connections {
//                        target : myModel
//                        onPositionChanged : {
//                            console.log( " position changed output " + myModel.position.x + "   " + myModel.position.y)
//                        }
//                    }
                }
            }
        }



    }
}
