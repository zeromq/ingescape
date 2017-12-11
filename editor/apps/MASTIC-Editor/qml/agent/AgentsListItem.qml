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
 *      Justine Limoges    <limoges@ingenuity.io>
 *      Vincent Peyruqueou <peyruqueou@ingenuity.io>
 *
 */

import QtQuick 2.8
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4

import I2Quick 1.0

import MASTIC 1.0
import "../theme" as Theme


Item {
    id : root

    //--------------------------------
    //
    // Properties
    //
    //--------------------------------

    // Controller associated to our view
    property var controller : null;

    // my agent
    property var agent: null;

    // true if agent Item contains the mouse (rollover)
    property bool agentItemIsHovered : false

    width: MasticTheme.leftPanelWidth
    height: 85



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

        color: agentItemIsHovered? MasticTheme.agentsListItemRollOverBackgroundColor : MasticTheme.agentsListItemBackgroundColor

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


            // Selected Agent
            Item {
                anchors.fill: parent
                visible : controller && root.agent && (controller.selectedAgent === root.agent);

                Rectangle {
                    anchors {
                        left : parent.left
                        top : parent.top
                        bottom: parent.bottom
                    }

                    width : 6
                    color : MasticTheme.selectedAgentColor
                }

                Button {
                    id: removeButton

                    visible : (model.isON === false)
                    activeFocusOnPress: true

                    anchors {
                        top: parent.top
                        topMargin: 10
                        right : parent.right
                        rightMargin: 12
                    }

                    style: Theme.LabellessSvgButtonStyle {
                        fileCache: MasticTheme.svgFileMASTIC

                        pressedID: releasedID + "-pressed"
                        releasedID: "supprimer"
                        disabledID : releasedID
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

                anchors {
                    left : parent.left
                    leftMargin: 28
                    top: parent.top
                    topMargin: 12
                    right : freezeButton.left
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
                    color: (root.agent && (root.agent.isON === true)) ? MasticTheme.agentsListLabelColor : MasticTheme.agentOFFLabelColor


                    font: MasticTheme.headingFont


                    // clones
                    Rectangle {
                        height : 16
                        width : height
                        radius : height/2

                        visible: (model.clonesNumber > 0)

                        anchors {
                            verticalCenter: parent.verticalCenter
                            verticalCenterOffset: 1
                            right : parent.left
                            rightMargin: 5
                        }

                        color : MasticTheme.redColor

                        Text {
                            anchors.centerIn : parent
                            text: model.clonesNumber

                            color : MasticTheme.whiteColor
                            font {
                                family: MasticTheme.labelFontFamily
                                weight : Font.Black
                                pixelSize : 12
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
                        color: if (root.agent && (root.agent.isON === true)) {
                                   ((model.definition && model.definition.isVariant) ?
                                          definitionNameBtn.pressed? MasticTheme.middleDarkRedColor : MasticTheme.redColor
                                    : definitionNameBtn.pressed? MasticTheme.agentsListPressedLabel2Color : MasticTheme.agentsListLabel2Color)
                               }
                               else {
                                   ((model.definition && model.definition.isVariant) ?
                                          definitionNameBtn.pressed? MasticTheme.darkRedColor : MasticTheme.middleDarkRedColor
                                    : definitionNameBtn.pressed? MasticTheme.agentOFFPressedLabel2Color : MasticTheme.agentOFFLabel2Color)
                               }
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
                        color: definitionNameTxt.color

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

                // Hostnames (on the network) of our agents
                Text {
                    id: agentHostnames
                    anchors {
                        left : parent.left
                        right : parent.right
                    }
                    elide: Text.ElideRight

                    text: root.agent ? root.agent.hostnames: ""

                    color: (root.agent && (root.agent.isON === true)) ? MasticTheme.agentsListTextColor : MasticTheme.agentOFFTextColor
                    font: MasticTheme.normalFont
                }

            }

            Button {
                id: offButton

                // Already appeared on the network AND (agent is "ON" OR agent can be restarted)
                visible: (root.agent && !root.agent.neverAppearedOnNetwork && (root.agent.isON || root.agent.canBeRestarted))

                activeFocusOnPress: true
                enabled: visible

                anchors {
                    bottom: muteButton.top
                    bottomMargin: 5
                    horizontalCenter: muteButton.horizontalCenter
                }

                style: Theme.LabellessSvgButtonStyle {
                    fileCache: MasticTheme.svgFileMASTIC

                    pressedID: releasedID + "-pressed"
                    releasedID: model.isON? "on" : "off"
                    disabledID : releasedID
                }

                onClicked: {
                    model.QtObject.changeState();
                }
            }

            Button {
                id: muteButton

                visible : (model.isON === true)
                activeFocusOnPress: true

                anchors {
                    bottom: parent.bottom
                    bottomMargin: 10
                    right : parent.right
                    rightMargin: 12
                }

                style: Theme.LabellessSvgButtonStyle {
                    fileCache: MasticTheme.svgFileMASTIC

                    pressedID: releasedID + "-pressed"
                    releasedID: model.isMuted? "muteactif" : "muteinactif"
                    disabledID : releasedID
                }

                onClicked: {
                    model.QtObject.changeMuteAllOutputs();
                }
            }

            Button {
                id: freezeButton

                visible: model.canBeFrozen && (model.isON === true)
                enabled : visible
                activeFocusOnPress: true

                anchors {
                    verticalCenter: muteButton.verticalCenter
                    right : muteButton.left
                    rightMargin: 5
                }

                style: Theme.LabellessSvgButtonStyle {
                    fileCache: MasticTheme.svgFileMASTIC

                    pressedID: releasedID + "-pressed"
                    releasedID: model.isFrozen? "freezeactif" : "freezeinactif"
                    disabledID : releasedID
                }

                onClicked: {
                    model.QtObject.changeFreeze();
                }
            }

        }
    }

}
