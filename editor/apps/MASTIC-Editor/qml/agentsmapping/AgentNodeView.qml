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
    property var agentMappingVM: null
    property var agentName: agentMappingVM ? agentMappingVM.agentName : ""

    property bool agentIsPressed : false;

    property bool isReduced : agentMappingVM && agentMappingVM.isReduced

    width : 228
    height : (rootItem.agentMappingVM && !rootItem.isReduced)?
                 (52 + 20*Math.max(rootItem.agentMappingVM.inputsList.count , rootItem.agentMappingVM.outputsList.count))
               : 40


    // Init position of our agent
    x: (agentMappingVM && agentMappingVM.position) ? agentMappingVM.position.x : 0
    y: (agentMappingVM && agentMappingVM.position) ? agentMappingVM.position.y : 0


    clip : true

    //
    // Bindings to save the position of our agent
    //
    Binding {
        target: rootItem.agentMappingVM
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

        color : rootItem.agentIsPressed?
                    MasticTheme.darkGreyColor2
                  : (rootItem.agentMappingVM && rootItem.agentMappingVM.isON)? MasticTheme.darkGreyColor : MasticTheme.veryDarkGreyColor
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
            text : rootItem.agentName

            color : (rootItem.agentMappingVM && rootItem.agentMappingVM.isON)? MasticTheme.agentsONNameMappingColor : MasticTheme.agentsOFFNameMappingColor
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

            visible : !rootItem.isReduced
        }



        //
        //
        // Global Points
        //
        Rectangle {
            id : inputGlobalPoint

            anchors {
                horizontalCenter : parent.left
                verticalCenter: parent.verticalCenter
            }

            height : 13
            width : height
            radius : height/2

            color : if (agentMappingVM) {
                        switch (agentMappingVM.reducedMapValueTypeInInput)
                        {
                        case AgentIOPValueTypes.INTEGER:
                            agentMappingVM.isON? MasticTheme.orangeColor2 : MasticTheme.darkOrangeColor2
                            break;
                        case AgentIOPValueTypes.DOUBLE:
                            agentMappingVM.isON? MasticTheme.orangeColor2 : MasticTheme.darkOrangeColor2
                            break;
                        case AgentIOPValueTypes.STRING:
                            agentMappingVM.isON? MasticTheme.redColor2 : MasticTheme.darkRedColor2
                            break;
                        case AgentIOPValueTypes.BOOL:
                            agentMappingVM.isON? MasticTheme.orangeColor2 : MasticTheme.darkOrangeColor2
                            break;
                        case AgentIOPValueTypes.IMPULSION:
                            agentMappingVM.isON? MasticTheme.purpleColor : MasticTheme.darkPurpleColor
                            break;
                        case AgentIOPValueTypes.DATA:
                            agentMappingVM.isON? MasticTheme.greenColor : MasticTheme.darkGreenColor
                            break;
                        case AgentIOPValueTypes.MIXED:
                            agentMappingVM.isON? MasticTheme.whiteColor : MasticTheme.greyColor4
                            break;
                        case AgentIOPValueTypes.UNKNOWN:
                            "#000000"
                            break;
                        default:
                            MasticTheme.whiteColor;
                            break;
                        }
                    } else {
                        MasticTheme.whiteColor
                    }

            visible : rootItem.isReduced && rootItem.agentMappingVM && rootItem.agentMappingVM.inputsList.count > 0
        }

        Rectangle {
            id : outputGlobalPoint

            anchors {
                horizontalCenter : parent.right
                verticalCenter: parent.verticalCenter
            }

            height : 13
            width : height
            radius : height/2

            color : if (agentMappingVM) {
                        switch (agentMappingVM.reducedMapValueTypeInOutput)
                        {
                        case AgentIOPValueTypes.INTEGER:
                            agentMappingVM.isON? MasticTheme.orangeColor2 : MasticTheme.darkOrangeColor2
                            break;
                        case AgentIOPValueTypes.DOUBLE:
                            agentMappingVM.isON? MasticTheme.orangeColor2 : MasticTheme.darkOrangeColor2
                            break;
                        case AgentIOPValueTypes.STRING:
                            agentMappingVM.isON? MasticTheme.redColor2 : MasticTheme.darkRedColor2
                            break;
                        case AgentIOPValueTypes.BOOL:
                            agentMappingVM.isON? MasticTheme.orangeColor2 : MasticTheme.darkOrangeColor2
                            break;
                        case AgentIOPValueTypes.IMPULSION:
                            agentMappingVM.isON? MasticTheme.purpleColor : MasticTheme.darkPurpleColor
                            break;
                        case AgentIOPValueTypes.DATA:
                            agentMappingVM.isON? MasticTheme.greenColor : MasticTheme.darkGreenColor
                            break;
                        case AgentIOPValueTypes.MIXED:
                            agentMappingVM.isON? MasticTheme.whiteColor : MasticTheme.greyColor4
                            break;
                        case AgentIOPValueTypes.UNKNOWN:
                            "#000000"
                            break;
                        default:
                            MasticTheme.whiteColor;
                            break;
                        }
                    } else {
                        MasticTheme.whiteColor
                    }

            visible : rootItem.isReduced && rootItem.agentMappingVM && rootItem.agentMappingVM.outputsList.count > 0
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

            visible : !rootItem.isReduced

            Repeater {
                // List of intput slots VM
                model: rootItem.agentMappingVM ? rootItem.agentMappingVM.inputsList : 0

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
                        text : myModel.modelM ? myModel.modelM.name : ""

                        color : (rootItem.agentMappingVM && rootItem.agentMappingVM.isON)? MasticTheme.agentsONInputsOutputsMappingColor : MasticTheme.agentsOFFInputsOutputsMappingColor
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

                        color : if (agentMappingVM && myModel && myModel.iopModel) {

                                    switch (myModel.iopModel.agentIOPValueType)
                                    {
                                    case AgentIOPValueTypes.INTEGER:
                                        agentMappingVM.isON? MasticTheme.orangeColor2 : MasticTheme.darkOrangeColor2
                                        break;
                                    case AgentIOPValueTypes.DOUBLE:
                                        agentMappingVM.isON? MasticTheme.orangeColor2 : MasticTheme.darkOrangeColor2
                                        break;
                                    case AgentIOPValueTypes.STRING:
                                        agentMappingVM.isON? MasticTheme.redColor2 : MasticTheme.darkRedColor2
                                        break;
                                    case AgentIOPValueTypes.BOOL:
                                        agentMappingVM.isON? MasticTheme.orangeColor2 : MasticTheme.darkOrangeColor2
                                        break;
                                    case AgentIOPValueTypes.IMPULSION:
                                        agentMappingVM.isON? MasticTheme.purpleColor : MasticTheme.darkPurpleColor
                                        break;
                                    case AgentIOPValueTypes.DATA:
                                        agentMappingVM.isON? MasticTheme.greenColor : MasticTheme.darkGreenColor
                                        break;
                                    case AgentIOPValueTypes.MIXED:
                                        agentMappingVM.isON? MasticTheme.whiteColor : MasticTheme.greyColor4
                                        break;
                                    case AgentIOPValueTypes.UNKNOWN:
                                        "#000000"
                                        break;
                                    default:
                                        "#000000"
                                        break;
                                    }

                                } else {
                                    MasticTheme.whiteColor
                                }
                    }


                    //
                    // Bindings to save the anchor point of our input slot
                    // i.e. the point used to draw a link
                    //
                    Binding {
                        target: myModel

                        property: "position"

                        // the position inside the agent is not the same if the agent is reduced or not
                        value:  (rootItem.agentMappingVM && !rootItem.agentMappingVM.isReduced) ?
                                    (Qt.point(rootItem.x + inputSlotItem.mapToItem(rootItem, linkPoint.x, linkPoint.y).x + linkPoint.width/2,
                                              rootItem.y + inputSlotItem.mapToItem(rootItem, linkPoint.x, linkPoint.y).y + linkPoint.height/2))
                                  : (Qt.point(rootItem.x + inputGlobalPoint.x + inputGlobalPoint.width/2,
                                              rootItem.y + inputGlobalPoint.y + inputGlobalPoint.height/2));
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

            visible : !rootItem.isReduced

            Repeater {
                // List of output slots VM
                model: (rootItem.agentMappingVM)? rootItem.agentMappingVM.outputsList : 0

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
                        text : myModel.modelM ? myModel.modelM.name : ""

                        color : (rootItem.agentMappingVM && rootItem.agentMappingVM.isON)? MasticTheme.agentsONInputsOutputsMappingColor : MasticTheme.agentsOFFInputsOutputsMappingColor
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

                        color : if (agentMappingVM && myModel && myModel.iopModel) {

                                    switch (myModel.iopModel.agentIOPValueType)
                                    {
                                    case AgentIOPValueTypes.INTEGER:
                                         agentMappingVM.isON? MasticTheme.orangeColor2 : MasticTheme.darkOrangeColor2
                                        break;
                                    case AgentIOPValueTypes.DOUBLE:
                                         agentMappingVM.isON? MasticTheme.orangeColor2 : MasticTheme.darkOrangeColor2
                                        break;
                                    case AgentIOPValueTypes.STRING:
                                         agentMappingVM.isON? MasticTheme.redColor2 : MasticTheme.darkRedColor2
                                        break;
                                    case AgentIOPValueTypes.BOOL:
                                         agentMappingVM.isON? MasticTheme.orangeColor2 : MasticTheme.darkOrangeColor2
                                        break;
                                    case AgentIOPValueTypes.IMPULSION:
                                         agentMappingVM.isON? MasticTheme.purpleColor : MasticTheme.darkPurpleColor
                                        break;
                                    case AgentIOPValueTypes.DATA:
                                         agentMappingVM.isON? MasticTheme.greenColor : MasticTheme.darkGreenColor
                                        break;
                                    case AgentIOPValueTypes.MIXED:
                                         agentMappingVM.isON? MasticTheme.whiteColor : MasticTheme.greyColor4
                                        break;
                                    case AgentIOPValueTypes.UNKNOWN:
                                        "#000000"
                                        break;
                                    default:
                                        "#000000"
                                        break;
                                    }

                                } else {
                                    MasticTheme.whiteColor
                                }
                    }


                    //
                    // Bindings to save the anchor point of our input slot
                    // i.e. the point used to draw a link
                    //
                    Binding {
                        target: myModel

                        property: "position"

                        // the position inside the agent is not the same if the agent is reduced or not
                        value: (rootItem.agentMappingVM && !rootItem.agentMappingVM.isReduced) ?
                                   (Qt.point(rootItem.x + outputSlotItem.mapToItem(rootItem, linkPointOut.x, linkPointOut.y).x + linkPointOut.width/2,
                                             rootItem.y + outputSlotItem.mapToItem(rootItem, linkPointOut.x, linkPointOut.y).y + linkPointOut.height/2))
                                 : (Qt.point(rootItem.x + outputGlobalPoint.x + outputGlobalPoint.width/2,
                                             rootItem.y + outputGlobalPoint.y + outputGlobalPoint.height/2));
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
