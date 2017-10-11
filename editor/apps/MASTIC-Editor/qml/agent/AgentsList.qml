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
 *      Vincent Peyruqueou <peyruqueou@ingenuity.io>
 *      Alexandre Lemort   <lemort@ingenuity.io>
 *
 */

import QtQuick 2.8
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4

import I2Quick 1.0

import MASTIC 1.0


Item {
    id: rootItem

    anchors.fill: parent

    //--------------------------------
    //
    // Properties
    //
    //--------------------------------

    // Controller associated to our view
    property var controller : null;


    //-----------------------------------------
    //
    // Functions
    //
    //-----------------------------------------


    //--------------------------------
    //
    // Content
    //
    //--------------------------------


    //
    // List of agents
    //
    ListView {
        id: agentsList

        anchors {
            top: header.bottom
            bottom: parent.bottom
            left: parent.left
            right: parent.right
        }

        model: MasticEditorC.modelManager.allAgentsVM

        delegate: componentAgentListItem


        //
        // Transition animations
        //
        add: Transition {
            NumberAnimation { property: "opacity"; from: 0.0; to: 1.0 }
            NumberAnimation { property: "scale"; from: 0.0; to: 1.0 }
        }

        displaced: Transition {
            NumberAnimation { properties: "x,y"; easing.type: Easing.OutBounce }

            // ensure opacity and scale values return to 1.0
            NumberAnimation { property: "opacity"; to: 1.0 }
            NumberAnimation { property: "scale"; to: 1.0 }
        }

        move: Transition {
            NumberAnimation { properties: "x,y"; easing.type: Easing.OutBounce }

            // ensure opacity and scale values return to 1.0
            NumberAnimation { property: "opacity"; to: 1.0 }
            NumberAnimation { property: "scale"; to: 1.0 }
        }

        remove: Transition {
            // ensure opacity and scale values return to 0.0
            NumberAnimation { property: "opacity"; to: 0.0 }
            NumberAnimation { property: "scale"; to: 0.0 }
        }

    }


    //
    // Header
    //
    Rectangle {
        id: header

        anchors {
            top: parent.top
            left: parent.left
            right: parent.right
        }

        height: childrenRect.height

        color: MasticTheme.agentsListHeaderBackgroundColor


        Row {
            id: headerRow1

            height: 30

            anchors {
                top: parent.top
                left: parent.left
                right: parent.right
            }

            Text {
                id: txtSearch

                text: qsTr("Rechercher...")

                color: MasticTheme.agentsListLabelColor

                font: MasticTheme.normalFont
            }

            Text {
                id: txtFilter

                text: qsTr("Filtrer...")

                color:MasticTheme.agentsListLabelColor

                font: MasticTheme.normalFont
            }
        }

        Row {
            id: headerRow2

            height: 30

            anchors {
                top: headerRow1.bottom
                left: parent.left
                right: parent.right
            }

            Button {
                id: btnAddAgent
                text: qsTr("Nouvel Agent")
                onClicked: {
                    console.log("Nouvel Agent")
                    // TODO
                }
            }

            Text {
                text: qsTr("Importer...")

                color: MasticTheme.agentsListLabelColor

                font: MasticTheme.normalFont
            }

            Text {
                text: qsTr("Exporter...")

                color: MasticTheme.agentsListLabelColor

                font: MasticTheme.normalFont
            }
        }


        Rectangle {
            anchors {
                bottom: headerRow2.bottom
                left: parent.left
                right: parent.right
            }

            color: "#17191F"

            height: 1
        }
    }



    //
    // Visual representation of an agent in our list
    //
    Component {
        id: componentAgentListItem

        Item {
            id : agentListItem

            width: MasticTheme.leftPanelWidth
            height: model.isFictitious ? 90 : 130

            Rectangle {

                anchors {
                    fill: parent
                    leftMargin: 4
                    rightMargin: 4
                    topMargin: 4
                    bottomMargin: 4
                }
                radius: 5
                color: MasticTheme.agentsListItemBackgroundColor
                border {
                    width: 1
                    color: MasticTheme.whiteColor
                }

                Item {
                    id: agentRow

                    anchors {
                        fill: parent
                        leftMargin: 5
                        topMargin: 2
                    }

                    Button {
                        id: btnDeleteAgent

                        anchors {
                            left: agentRow.right
                            top: agentRow.top
                        }

                        //visible: model.isFictitious
                        visible: (model.status === AgentStatus.OFF)

                        text: "X"

                        onClicked: {
                            if (controller)
                            {
                                console.log("Delete agent from the list : " + model.name)
                                controller.deleteAgentFromList(model.QtObject);
                            }
                        }
                    }

                    Column {
                        width: 175
                        anchors {
                            left : parent.left
                        }

                        Text {
                            id: agentName
                            text: model.name

                            height: 25
                            color: MasticTheme.agentsListLabelColor
                            font: MasticTheme.normalFont
                        }

                        Text {
                            text: model.models.count + " clone(s)"

                            height: 25
                            color: MasticTheme.agentsListLabelColor
                            font: MasticTheme.normalFont
                        }

                        Text {
                            id: agentStatus
                            text: "Status: " + AgentStatus.enumToString(model.status)

                            height: 25
                            color: MasticTheme.agentsListLabelColor
                            font: MasticTheme.normalFont
                        }

                        Text {
                            id: agentAddresses
                            text: model.addresses

                            height: 25
                            color: MasticTheme.agentsListLabelColor
                            font: MasticTheme.normalFont
                        }
                    }

                    Column {
                        width: 175
                        anchors {
                            right: parent.right
                        }

                        Button {
                            id: btnDefinition
                            text: model.definition ? model.definition.name : ""

                            onClicked: {
                                if (model.definition) {
                                    console.log("Open the definition of " + model.definition.name)
                                }
                            }
                        }

                        Text {
                            text: model.definition ? model.definition.description : ""

                            width: 175
                            elide: Text.ElideRight

                            height: 25
                            color: MasticTheme.agentsListLabelColor
                            font: MasticTheme.normalFont
                        }

                        Text {
                            text: model.definition ? model.definition.version : ""

                            height: 25
                            color: MasticTheme.agentsListLabelColor
                            font: MasticTheme.normalFont
                        }
                    }

                    Row {
                        visible: !model.isFictitious

                        anchors {
                            right: agentRow.right
                            bottom: agentRow.bottom
                        }

                        Button {
                            id: btnMute
                            text: "Mute"

                            onClicked: {
                                //console.log("QML: Mute " + model.name);
                                model.QtObject.mute();
                            }
                        }

                        Button {
                            id: btnFreeze
                            text: "Freeze"
                            visible: model.canBeFrozen

                            onClicked: {
                                //console.log("QML: Freeze " + model.name);
                                model.QtObject.freeze();
                            }
                        }
                    }
                }
            }
        }
    }
}
