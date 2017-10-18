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
            top: parent.top
            topMargin: 110
            bottom: parent.bottom
            left: parent.left
            right: parent.right
        }

        model: controller.agentsList

        delegate: componentAgentListItem

        /*onCurrentIndexChanged: {
            //console.log("onCurrentIndexChanged " + agentsList.currentIndex);
            console.log("onCurrentIndexChanged " + model.get(agentsList.currentIndex).name);
        }
        onCurrentItemChanged: {
            console.log("onCurrentItemChanged " + agentsList.currentItem);
        }*/

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
            bottom : agentsList.top
        }

        color : MasticTheme.selectedTabsBackgroundColor

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
    }



    //
    // Separator
    //
    Rectangle {
        anchors {
            bottom: agentsList.top
            left: parent.left
            right: parent.right
        }

        color: MasticTheme.leftPanelBackgroundColor

        height: 1
    }



    //
    // Visual representation of an agent in our list
    //
    Component {
        id: componentAgentListItem

        Item {
            id : agentListItem

            width: MasticTheme.leftPanelWidth
            height: 85

            Behavior on height {
                NumberAnimation {}
            }

            Rectangle {
                anchors {
                    fill: parent
                }

                border {
                    width: 0
                }

                color: MasticTheme.agentsListItemBackgroundColor

                Rectangle {
                    anchors {
                        bottom: parent.bottom
                        left: parent.left
                        right: parent.right
                    }

                    color: MasticTheme.leftPanelBackgroundColor

                    height: 1
                }


                Item {
                    id: agentRow

                    anchors {
                        fill: parent
                    }

                    MouseArea {
                        id: mouseAreaForSelection
                        anchors.fill: parent

                        onClicked: {
                            //agentsList.currentIndex = index

                            if (controller) {
                                controller.selectedAgent = model.QtObject;
                            }
                        }
                    }


                    Rectangle {
                        anchors.fill: parent

                        visible : controller && (controller.selectedAgent === model.QtObject);
                        color : "transparent"
                        radius : 5
                        border {
                            width : 2
                            color : MasticTheme.selectedAgentColor
                        }


                    }

                    //                    Button {
                    //                        id: btnDeleteAgent

                    //                        anchors {
                    //                            left: agentRow.right
                    //                            top: agentRow.top
                    //                        }

                    //                        visible: (agentListItem.ListView.isCurrentItem && (model.status === AgentStatus.OFF))

                    //                        text: "X"

                    //                        onClicked: {
                    //                            if (controller)
                    //                            {
                    //                                // Delete our agent
                    //                                controller.deleteAgent(model.QtObject);
                    //                            }
                    //                        }
                    //                    }


                    Column {
                        id : columnName

                        // TO DO : anchors on the right
                        width: 175

                        anchors {
                            left : parent.left
                            leftMargin: 28
                            top: parent.top
                            topMargin: 12
                        }
                        height : childrenRect.height

                        spacing : 4

                        // Name
                        Text {
                            id: agentName

                            anchors {
                                left : parent.left
                                right : parent.right
                            }
                            elide: Text.ElideRight

                            text: model.name
                            color: ((model.status === AgentStatus.ON) && !model.hasOnlyDefinition)? MasticTheme.agentsListLabelColor : MasticTheme.agentOFFLabelColor
                            font: MasticTheme.headingFont
                        }

                        // Definition name and version
                        MouseArea {
                            id : definitionNameBtn

                            anchors {
                                left : parent.left
                            }

                            height : definitionNameTxt.height
                            width : childrenRect.width

                            hoverEnabled: true
                            onClicked: {
                                if (controller) {
                                    // Open the definition of our agent
                                    controller.openDefinition(model.QtObject);
                                }
                            }

                            TextMetrics {
                                id : definitionName

                                elideWidth: (columnName.width - versionName.width)
                                elide: Text.ElideRight

                                text: model.definition ? model.definition.name : ""
                            }

                            Text {
                                id : definitionNameTxt

                                anchors {
                                    left : parent.left
                                }

                                text : definitionName.elidedText
                                color: ((model.status === AgentStatus.ON) && !model.hasOnlyDefinition)? MasticTheme.agentsListLabelColor : MasticTheme.agentOFFLabelColor
                                font: MasticTheme.heading2Font
                            }

                            Text {
                                id : versionName
                                anchors {
                                    bottom: definitionNameTxt.bottom
                                    bottomMargin : 2
                                    left : definitionNameTxt.right
                                    leftMargin: 5
                                }

                                text: model.definition ? "(v" + model.definition.version + ")" : ""

                                color: ((model.status === AgentStatus.ON) && !model.hasOnlyDefinition)? MasticTheme.agentsListLabelColor : MasticTheme.agentOFFLabelColor
                                font {
                                    family: MasticTheme.textFontFamily
                                    pixelSize : 10
                                    italic : true
                                }
                            }

                            // underline
                            Rectangle {
                                visible: definitionNameBtn.containsMouse

                                anchors {
                                    left : definitionNameTxt.left
                                    right : versionName.right
                                    bottom : parent.bottom
                                }

                                height : 1

                                color : definitionNameTxt.color
                            }

                        }

                        // Address(es) on the network of our agent(s)
                        Text {
                            id: agentAddresses
                            anchors {
                                left : parent.left
                                right : parent.right
                            }
                            elide: Text.ElideRight

                            text: model.addresses

                            color: ((model.status === AgentStatus.ON) && !model.hasOnlyDefinition)? MasticTheme.agentsListTextColor : MasticTheme.agentOFFTextColor
                            font: MasticTheme.normalFont
                        }

                    }



                    //                        Text {
                    //                            //text: model.models ? model.models.count + " clone(s)" : ""
                    //                            //visible: model.models && (model.models.count > 1)
                    //                            text: (model && model.models) ? model.models.count + " clone(s)" : ""
                    //                            visible: (model && model.models) ? (model.models.count > 1) : false

                    //                            color: "red"
                    //                            font: MasticTheme.normalFont
                    //                        }

                    //                        Text {
                    //                            id: agentStatus
                    //                            text: "Status: " + AgentStatus.enumToString(model.status)
                    //                            visible: !model.hasOnlyDefinition

                    //                            height: 25
                    //                            color: MasticTheme.agentsListLabelColor
                    //                            font: MasticTheme.normalFont
                    //                        }


                    //                    Column {
                    //                        width: 175
                    //                        anchors {
                    //                            top: parent.top
                    //                            topMargin: 30
                    //                            bottom: parent.bottom
                    //                            right: parent.right
                    //                        }

                    //                        Text {
                    //                            text: "Variante"
                    //                            visible: model.definition ? model.definition.isVariant : false

                    //                            height: 25
                    //                            color: "red"
                    //                            font: MasticTheme.normalFont
                    //                        }

                    //                        Text {
                    //                            text: model.definition ? model.definition.description : ""

                    //                            width: 175
                    //                            elide: Text.ElideRight

                    //                            height: 25
                    //                            color: MasticTheme.agentsListLabelColor
                    //                            font: MasticTheme.normalFont
                    //                        }

                    // }


                    //                    Switch {
                    //                        checked: (model.status === AgentStatus.ON)
                    //                        visible: !model.hasOnlyDefinition

                    //                        anchors {
                    //                            left: agentRow.left
                    //                            leftMargin: 2
                    //                            bottom: agentRow.bottom
                    //                            bottomMargin: 5
                    //                        }
                    //                    }

                    Row {
                        visible: !model.hasOnlyDefinition && (model.status === AgentStatus.ON)

                        anchors {
                            right: agentRow.right
                            bottom: agentRow.bottom
                        }

                        Button {
                            id: btnMuteAll
                            text: model.isMuted ? "UN-mute all" : "Mute all"

                            width: 110

                            onClicked: {
                                model.QtObject.updateMuteAllOutputs(!model.isMuted);
                            }
                        }

                        Button {
                            id: btnFreeze
                            text: model.isFrozen ? "UN-freeze" : "Freeze"

                            visible: model.canBeFrozen

                            width: 90

                            onClicked: {
                                model.QtObject.updateFreeze(!model.isFrozen);
                            }
                        }
                    }
                }
            }
        }
    }
}
