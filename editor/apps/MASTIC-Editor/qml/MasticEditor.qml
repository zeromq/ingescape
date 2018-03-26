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
import QtQuick.Window 2.3

import MASTIC 1.0


// agent sub-directory
import "agent" as Agent

// host sub-directory
import "host" as Host

// scenario sub-directory
import "scenario" as Scenario


Item {
    id: rootItem

    anchors.fill: parent


    //--------------------------------------------------------
    //
    //
    // Properties
    //
    //
    //--------------------------------------------------------

    property int popupTopmostZIndex: 1



    //--------------------------------------------------------
    //
    //
    // Functions
    //
    //
    //--------------------------------------------------------

    // function allowing to open the history panel
    function openHistory() {
        historyPanel.show();
    }



    //--------------------------------------------------------
    //
    //
    // Content
    //
    //
    //--------------------------------------------------------


    //
    // Center panel
    //
    Item {
        id: centerPanel

        anchors {
            left: leftPanel.right
            right: parent.right
            top: parent.top
            bottom: bottomPanel.top
        }


        AgentsMappingView {
            id: agentsMappingView

            anchors.fill: parent

            controller: MasticEditorC.agentsMappingC
        }
    }


    //
    // Bottom panel
    //
    Scenario.ScenarioTimeLine {
        id : bottomPanel
        anchors {
            left: leftPanel.right
            right: parent.right
            bottom: parent.bottom
        }
        height: 0
        controller: MasticEditorC.scenarioC;


    }




    //
    // Left panel
    //
    Rectangle {
        id: leftPanel

        width: MasticTheme.leftPanelWidth

        anchors {
            left: parent.left
            top: parent.top
            bottom: parent.bottom
        }

        color : MasticTheme.agentsMappingBackgroundColor;

        I2CustomRectangle {
            anchors {
                fill: parent
                topMargin: 9
            }
            color: MasticTheme.leftPanelBackgroundColor

            fuzzyRadius: 8
            topRightRadius : 5

            borderWidth: 1
            borderColor: MasticTheme.selectedTabsBackgroundColor

            // tabs of left panel
            I2TabView {
                id : leftPanelTabs

                anchors.fill :parent

                style: I2TabViewStyle {
                    frameOverlap: 1
                    tab: I2CustomRectangle {
                        color: styleData.selected ? MasticTheme.selectedTabsBackgroundColor : "transparent"
                        implicitWidth: leftPanelTabs.width/3
                        implicitHeight: 26
                        topRightRadius : 5

                        Text {
                            id: text
                            anchors.fill: parent
                            horizontalAlignment: Text.AlignHCenter
                            verticalAlignment: Text.AlignVCenter;
                            text: styleData.title
                            color: styleData.selected ? MasticTheme.agentsListLabelColor : MasticTheme.greyColor
                            wrapMode: Text.Wrap;

                            font {
                                family: MasticTheme.labelFontFamily;
                                weight: Font.ExtraBold;
                                pixelSize:18;
                                capitalization: Font.AllUppercase;
                            }
                        }
                    }
                    frame: Rectangle {
                        color: MasticTheme.selectedTabsBackgroundColor
                    }
                }

                currentIndex : 0

                onCurrentIndexChanged: {
                }


                Tab {
                    title: qsTr("Agents");
                    active : true
                    Agent.AgentsList {
                        id: agentsList

                        anchors.fill: parent

                        controller: MasticEditorC.agentsSupervisionC
                    }
                }

                Tab {
                    title: qsTr("Hosts");
                    Host.HostsList {
                        id: hostsList

                        anchors.fill: parent

                        controller: MasticEditorC.hostsSupervisionC
                    }
                }

                Tab {
                    title: qsTr("ACTIONS");
                    active : false

                    Item {
                        anchors.fill: parent

                        Scenario.ActionsList {
                            id: actionsList

                            anchors {
                                fill : parent
                                bottomMargin: actionsPanel.height
                            }

                            controller: MasticEditorC.scenarioC
                        }

                        Scenario.ActionsPanel {
                            id : actionsPanel
                            anchors {
                                left : parent.left
                                right: parent.right
                                bottom : parent.bottom
                            }
                            height : 320
                            controller: MasticEditorC.scenarioC
                        }
                    }
                }

                //                Tab {
                //                    title: qsTr("RECORDS");
                //                    active : false

                //                    Rectangle {
                //                        id: records
                //                        anchors.fill: parent
                //                        color : "transparent"
                //                    }
                //                }
            }

        }
    }


    // List of "Agent Definition Editor(s)"
    Repeater {
        model: MasticEditorC.modelManager.openedDefinitions

        delegate: Item {
            Agent.AgentDefinitionEditor {
                id: agentDefinitionEditor
                visible : true

                Component.onCompleted:  {
                    x = rootItem.Window.window.x + rootItem.Window.width/2 - agentDefinitionEditor.width/2 + (index * 40);
                    y = rootItem.Window.window.y + rootItem.Window.height/2  - agentDefinitionEditor.height / 2.0 + (index * 40);
                }

                onClosing: {
                    MasticEditorC.closeDefinition(model.QtObject);
                }

                onOpenHistory : {
                    historyPanel.show();
                }
            }
        }
    }


    // List of "Actions Editor(s)"
    Repeater {
        model : MasticEditorC.scenarioC ? MasticEditorC.scenarioC.openedActionsEditorsControllers : 0;

        delegate: Item {
            Scenario.ActionEditor {
                id: actionEditor

                controller : MasticEditorC.scenarioC
                panelController: model.QtObject
                visible : true

                Component.onCompleted:  {
                    x = rootItem.Window.window.x + rootItem.Window.width/2 - actionEditor.width/2  + (index * 40);
                    y = rootItem.Window.window.y + rootItem.Window.height/2 - actionEditor.height/2 + (index * 40);

                }

                onClosing: {
                    MasticEditorC.closeActionEditor(model.QtObject);
                }
            }
        }
    }




    // AgentMappingHistory
    Agent.HistoryPanel {
        id: historyPanel

        Component.onCompleted:  {
            // Center window
            x = rootItem.Window.window.x + rootItem.Window.width/2 - historyPanel.width/2;
            y = rootItem.Window.window.y + rootItem.Window.height/2 - historyPanel.height/2;
        }
    }


    // Overlay layer used to display streaming
    I2Layer {
        id: streamingLayer

        objectName: "streamingLayer"

        anchors
        {
            left:leftPanel.right
            top:parent.top
            bottom:bottomPanel.top
            right:parent.right
            fill: null
        }
    }
}
