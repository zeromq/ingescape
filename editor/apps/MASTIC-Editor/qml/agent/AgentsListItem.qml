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
                    color: (root.agent && (root.agent.isON === true) && !root.agent.hasOnlyDefinition)? MasticTheme.agentsListLabelColor : MasticTheme.agentOFFLabelColor
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
                        color: (root.agent && (root.agent.isON === true) && !root.agent.hasOnlyDefinition)? MasticTheme.agentsListLabelColor : MasticTheme.agentOFFLabelColor
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

                        color: (root.agent && (root.agent.isON === true) && !root.agent.hasOnlyDefinition)? MasticTheme.agentsListLabelColor : MasticTheme.agentOFFLabelColor
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

                    color: (root.agent && (root.agent.isON === true) && !root.agent.hasOnlyDefinition)? MasticTheme.agentsListTextColor : MasticTheme.agentOFFTextColor
                    font: MasticTheme.normalFont
                }

            }
        }
    }

}
