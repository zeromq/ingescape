/*
 *	IngeScape Editor
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

import INGESCAPE 1.0


// agent sub-directory
import "agent" as Agent

// host sub-directory
import "host" as Host

// record sub-directory
import "record" as Record

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

    // function allowing to open the network configuration popup
    function openNetworkConfiguration() {
        networkConfigurationPopup.open();
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

            controller: IngeScapeEditorC.agentsMappingC
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
        controller: IngeScapeEditorC.scenarioC;


    }




    //
    // Left panel
    //
    Rectangle {
        id: leftPanel

        width: IngeScapeTheme.leftPanelWidth

        anchors {
            left: parent.left
            top: parent.top
            bottom: parent.bottom
        }

        color : IngeScapeTheme.agentsMappingBackgroundColor;

        I2CustomRectangle {
            anchors {
                fill: parent
                topMargin: 9
            }
            color: IngeScapeTheme.leftPanelBackgroundColor

            fuzzyRadius: 8
            topRightRadius : 5

            borderWidth: 1
            borderColor: IngeScapeTheme.selectedTabsBackgroundColor

            // tabs of left panel
            I2TabView {
                id : leftPanelTabs

                anchors.fill :parent

                style: I2TabViewStyle {
                    frameOverlap: 1
                    tab: I2CustomRectangle {
                        color: styleData.selected ? IngeScapeTheme.selectedTabsBackgroundColor : "transparent"
                        implicitWidth: leftPanelTabs.width/4
                        implicitHeight: 26
                        topRightRadius : 5

                        Text {
                            id: text
                            anchors.fill: parent
                            horizontalAlignment: Text.AlignHCenter
                            verticalAlignment: Text.AlignVCenter;
                            text: styleData.title
                            color: styleData.selected ? IngeScapeTheme.agentsListLabelColor : IngeScapeTheme.greyColor
                            wrapMode: Text.Wrap;

                            font {
                                family: IngeScapeTheme.labelFontFamily;
                                weight: Font.ExtraBold;
                                pixelSize:18;
                                capitalization: Font.AllUppercase;
                            }
                        }
                    }
                    frame: Rectangle {
                        color: IngeScapeTheme.selectedTabsBackgroundColor
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

                        controller: IngeScapeEditorC.agentsSupervisionC
                    }
                }

                Tab {
                    title: qsTr("Hosts");
                    Host.HostsList {
                        id: hostsList

                        anchors.fill: parent

                        controller: IngeScapeEditorC.hostsSupervisionC
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

                            controller: IngeScapeEditorC.scenarioC
                        }

                        Scenario.ActionsPanel {
                            id : actionsPanel
                            anchors {
                                left : parent.left
                                right: parent.right
                                bottom : parent.bottom
                            }
                            height : 320
                            controller: IngeScapeEditorC.scenarioC
                        }
                    }
                }

                Tab {
                    title: qsTr("Records");

                    Record.RecordsList {
                        controller: IngeScapeEditorC.recordsSupervisionC
                        id: recordsList
                        anchors.fill: parent
                    }
                }
            }

        }
    }


    // List of "Agent Definition Editor(s)"
    Repeater {
        model: IngeScapeEditorC.modelManager.openedDefinitions

        delegate: Item {
            Agent.AgentDefinitionEditor {
                id: agentDefinitionEditor
                visible : true

                Component.onCompleted:  {
                    x = rootItem.Window.window.x + rootItem.Window.width/2 - agentDefinitionEditor.width/2 + (index * 40);
                    y = rootItem.Window.window.y + rootItem.Window.height/2  - agentDefinitionEditor.height / 2.0 + (index * 40);
                }

                onClosing: {
                    IngeScapeEditorC.closeDefinition(model.QtObject);
                }

                onOpenHistory : {
                    historyPanel.show();
                }
            }
        }
    }


    // List of "Actions Editor(s)"
    Repeater {
        model : IngeScapeEditorC.scenarioC ? IngeScapeEditorC.scenarioC.openedActionsEditorsControllers : 0;

        delegate: Item {
            Scenario.ActionEditor {
                id: actionEditor

                controller : IngeScapeEditorC.scenarioC
                panelController: model.QtObject
                visible : true

                Component.onCompleted:  {
                    x = rootItem.Window.window.x + rootItem.Window.width/2 - actionEditor.width/2  + (index * 40);
                    y = rootItem.Window.window.y + rootItem.Window.height/2 - actionEditor.height/2 + (index * 40);

                }

                onClosing: {
                    IngeScapeEditorC.closeActionEditor(model.QtObject);
                }
            }
        }
    }


    //
    // History Panel
    //
    Agent.HistoryPanel {
        id: historyPanel

        Component.onCompleted:  {
            // Center window
            x = rootItem.Window.window.x + rootItem.Window.width/2 - historyPanel.width/2;
            y = rootItem.Window.window.y + rootItem.Window.height/2 - historyPanel.height/2;
        }
    }


    //
    // Network Configuration (Popup)
    //
    NetworkConfiguration {
        id: networkConfigurationPopup

        anchors.centerIn: parent
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
