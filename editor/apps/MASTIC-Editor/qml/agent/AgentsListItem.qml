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
    id : root

    //--------------------------------
    //
    // Properties
    //
    //--------------------------------

    // Controller associated to our view
    property var controller : null;

    property var agent: null;

    width: MasticTheme.leftPanelWidth
    height: 85


    Behavior on height {
        NumberAnimation {}
    }

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

            Rectangle {
                anchors.fill: parent

                visible : controller && root.agent && (controller.selectedAgent === root.agent);
                color : "transparent"
                radius : 5
                border {
                    width : 2
                    color : MasticTheme.selectedAgentColor
                }

                Button {
                    id: removeButton

                    property var boundingBox: MasticTheme.svgFileMASTIC.boundsOnElement("supprimer");
                    height : boundingBox.height
                    width :  boundingBox.width

                    visible : (model.status === AgentStatus.OFF)

                    anchors {
                        verticalCenter: parent.verticalCenter
                    }

                    style: I2SvgButtonStyle {
                        fileCache: MasticTheme.svgFileMASTIC

                        pressedID: releasedID + "-pressed"
                        releasedID: "supprimer"
                    }

                    onClicked: {
                        if (controller)
                        {
                            // Delete our agent
                            controller.deleteAgent(model.QtObject);
                        }
                    }
                }

            }

            Column {
                id : columnName

                // TODO : anchors on the right
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

                    text: root.agent? root.agent.name : ""
                    color: (root.agent && (root.agent.status === AgentStatus.ON) && !root.agent.hasOnlyDefinition)? MasticTheme.agentsListLabelColor : MasticTheme.agentOFFLabelColor
                    font: MasticTheme.headingFont


                    // clones
                    Rectangle {
                        height : 16
                        width : height
                        radius : height/2

                        visible: (model && model.models) ? (model.models.count > 1) : false

                        anchors {
                            verticalCenter: parent.verticalCenter
                            verticalCenterOffset: 1
                            right : parent.left
                            rightMargin: 5
                        }

                        color : MasticTheme.redColor

                        Text {
                            anchors.centerIn : parent
                            text: (model && model.models) ? model.models.count : ""

                            color : MasticTheme.whiteColor
                            font {
                                family: MasticTheme.labelFontFamilyBlack
                                weight: Font.Black
                                pixelSize : 10
                            }
                        }


                    }
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
                        if (controller && root.agent) {
                            // Open the definition of our agent
                            controller.openDefinition(root.agent);
                        }
                    }

                    TextMetrics {
                        id : definitionName

                        elideWidth: (columnName.width - versionName.width)
                        elide: Text.ElideRight

                        text: root.agent && root.agent.definition ? root.agent.definition.name : ""
                    }

                    Text {
                        id : definitionNameTxt

                        anchors {
                            left : parent.left
                        }

                        text : definitionName.elidedText
                        color: (model.definition.isVariant )? MasticTheme.redColor : ((root.agent && (root.agent.status === AgentStatus.ON) && !root.agent.hasOnlyDefinition)? MasticTheme.agentsListLabelColor : MasticTheme.agentOFFLabelColor)
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

                        text: root.agent && root.agent.definition ? "(v" + root.agent.definition.version + ")" : ""

                        color: (root.agent && model.definition.isVariant )? MasticTheme.redColor : ((root.agent && (root.agent.status === AgentStatus.ON) && !root.agent.hasOnlyDefinition)? MasticTheme.agentsListLabelColor : MasticTheme.agentOFFLabelColor)
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

                    text: root.agent ? root.agent.addresses: ""

                    color: (root.agent && (root.agent.status === AgentStatus.ON) && !root.agent.hasOnlyDefinition)? MasticTheme.agentsListTextColor : MasticTheme.agentOFFTextColor
                    font: MasticTheme.normalFont
                }

            }

            Button {
                id: offButton

                property var boundingBox: MasticTheme.svgFileMASTIC.boundsOnElement("on");
                height : boundingBox.height
                width :  boundingBox.width

                anchors {
                    bottom: muteButton.top
                    bottomMargin: 5
                    horizontalCenter: muteButton.horizontalCenter
                }

                style: I2SvgButtonStyle {
                    fileCache: MasticTheme.svgFileMASTIC

                    pressedID: releasedID + "-pressed"
                    releasedID: model.isOFF? "on" : "off"
                }

                onClicked: {
                    model.QtObject.updateMuteAllOutputs(!model.isMuted);
                }
            }

            Button {
                id: muteButton

                property var boundingBox: MasticTheme.svgFileMASTIC.boundsOnElement("muteactif");
                height : boundingBox.height
                width :  boundingBox.width

                anchors {
                    bottom: parent.bottom
                    bottomMargin: 10
                    right : parent.right
                    rightMargin: 10
                }

                style: I2SvgButtonStyle {
                    fileCache: MasticTheme.svgFileMASTIC

                    pressedID: releasedID + "-pressed"
                    releasedID: model.isMuted? "muteactif" : "muteinactif"
                }

                onClicked: {
                    model.QtObject.updateMuteAllOutputs(muteButton.checked);
                }
            }

            Button {
                id: freezeButton

                property var boundingBox: MasticTheme.svgFileMASTIC.boundsOnElement("freezeactif");
                height : boundingBox.height
                width :  boundingBox.width

                visible: model.canBeFrozen
                enabled : visible

                anchors {
                    verticalCenter: muteButton.verticalCenter
                    right : muteButton.left
                    rightMargin: 5
                }

                style: I2SvgButtonStyle {
                    fileCache: MasticTheme.svgFileMASTIC

                    pressedID: releasedID + "-pressed"
                    releasedID: model.isFrozen? "freezeactif" : "freezeinactif"
                }

                onClicked: {
                    model.QtObject.updateFreeze(!model.isFrozen);
                }
            }

        }
    }

}
