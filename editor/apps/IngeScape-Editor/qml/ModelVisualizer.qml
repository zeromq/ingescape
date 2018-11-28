/*
 *	IngeScape Editor
 *
 *  Copyright © 2017-2018 Ingenuity i/o. All rights reserved.
 *
 *	See license terms for the rights and conditions
 *	defined by copyright holders.
 *
 *
 *	Contributors:
 *      Vincent Peyruqueou <peyruqueou@ingenuity.io>
 *
 */

import QtQuick 2.8
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4

import I2Quick 1.0
import QtQuick.Window 2.3

import INGESCAPE 1.0


// agent sub-directory
import "agent" as Agent


Rectangle {
    id: rootRectangle

    anchors.fill: parent

    color: IngeScapeTheme.editorsBackgroundColor
    border {
        width: 2
        color: IngeScapeTheme.editorsBackgroundBorderColor
    }

    clip: true

    visible: IngeScapeEditorC.isVisibleModelVisualizer


    //
    // List of all "Agents Grouped by Name"
    //
    ListView {
        anchors {
            left: parent.left
            right: parent.right
            top: parent.top
            bottom: parent.bottom
            margins: 2
        }
        //height: contentHeight

        model: IngeScapeEditorC.modelManager.allAgentsGroupsByName

        delegate: Rectangle {

            property var groupByNameVM: model.QtObject

            anchors {
                left: parent.left
            }
            width: childrenRect.width + 5
            height: columnListOfGroupsByDefinition.height + 5

            color: groupByNameVM.isON ? "#22CCCC" : "#227777"
            border {
                width: 1
                color: IngeScapeTheme.editorsBackgroundBorderColor
            }

            Row {
                spacing: 15

                Text {
                    text: groupByNameVM.name
                    width: 150
                    color: groupByNameVM.isON ? "white" : "#888888"
                    font {
                        pointSize: 14
                        weight: Font.Bold
                    }
                }

                Text {
                    text: groupByNameVM.models.count + " M"
                    color: groupByNameVM.isON ? "white" : "#888888"
                    font.pointSize: 14
                }

                Text {
                    text: groupByNameVM.numberOfAgentsON + " ON"
                    color: "#00FF00"
                    font {
                        pointSize: 14
                        weight: Font.Bold
                    }
                }

                Text {
                    text: groupByNameVM.numberOfAgentsOFF + " OFF"
                    color: "#FF0000"
                    font {
                        pointSize: 14
                        weight: Font.Bold
                    }
                }

                Column {
                    id: columnListOfGroupsByDefinition

                    height: childrenRect.height + 5

                    Repeater {
                        model: groupByNameVM ? groupByNameVM.allAgentsGroupsByDefinition : null

                        Rectangle {

                            property var groupByDefinitionVM: model.QtObject

                            width: childrenRect.width + 5
                            height: childrenRect.height + 5

                            color: groupByDefinitionVM.isON ? "#22CC22" : "#227722"
                            border {
                                width: 1
                                color: IngeScapeTheme.editorsBackgroundBorderColor
                            }

                            Row {
                                spacing: 15

                                Text {
                                    text: groupByDefinitionVM.definition ? groupByDefinitionVM.definition.name : "Def is NULL"
                                    color: groupByDefinitionVM.isON ? "white" : "#888888"
                                    width: 150
                                    elide: Text.ElideMiddle
                                    font.pointSize: 14
                                }

                                Text {
                                    text: groupByDefinitionVM.models.count + " M"
                                    color: groupByDefinitionVM.isON ? "white" : "#888888"
                                    font.pointSize: 14
                                }


                                Column {
                                    id: columnAgents

                                    height: childrenRect.height + 5

                                    Repeater {
                                        model: groupByDefinitionVM.models

                                        Rectangle {

                                            property var agentM: model.QtObject

                                            width: childrenRect.width
                                            height: 25

                                            color: agentM.isON ? "#2222CC" : "#222277"

                                            Row {
                                                spacing: 15

                                                Text {
                                                    text: agentM.hostname
                                                    color: agentM.isON ? "white" : "#888888"
                                                    font {
                                                        pointSize: 14
                                                        weight: Font.Bold
                                                    }
                                                }

                                                Text {
                                                    text: agentM.peerId
                                                    color: agentM.isON ? "white" : "#888888"
                                                }

                                                Text {
                                                    text: agentM.commandLine
                                                    color: agentM.isON ? "white" : "#888888"
                                                    width: 400
                                                    elide: Text.ElideLeft
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }




}
