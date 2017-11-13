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


// agent sub-directory
import "agent" as Agent

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
    I2CustomRectangle {
        id: bottomPanel

        anchors {
            left: leftPanel.right
            right: parent.right
            bottom: parent.bottom
        }

        height: MasticTheme.bottomPanelHeight

        color: MasticTheme.scenarioBackgroundColor
        fuzzyRadius: 8


        Text {
            anchors.centerIn: parent

            text: qsTr("Scenario")

            font: MasticTheme.headingFont

            color: MasticTheme.whiteColor
        }
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
                        implicitWidth: 107
                        implicitHeight: 26
                        topRightRadius : 5

                        Text {
                            id: text
                            anchors.fill: parent
                            horizontalAlignment: Text.AlignHCenter
                            verticalAlignment: Text.AlignVCenter;
                            text: styleData.title
                            color: styleData.selected ? MasticTheme.agentsListLabelColor : MasticTheme.selectedTabsBackgroundColor
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
                    title: qsTr("SUPERVISION");
                    active : true

                    Agent.AgentsList {
                        id: agentsList

                        anchors.fill: parent

                        controller: MasticEditorC.agentsSupervisionC
                    }
                }

                Tab {
                    title: qsTr("ACTIONS");
                    active : false

                    Scenario.ActionsList {
                        id: actionsList

                        anchors.fill: parent

                        controller: MasticEditorC.scenarioC
                    }
                }

                Tab {
                    title: qsTr("RECORDS");
                    active : false

                    Rectangle {
                        id: records
                        anchors.fill: parent
                        color : "blue"
                    }
                }
            }

        }
    }


    // List of "Agent Definition Editor(s)"
    Repeater {
        model: MasticEditorC.modelManager.openedDefinitions

        delegate: Agent.AgentDefinitionEditor {
            id: agentDefinitionEditor

            // Center popup
            x: (parent.width - agentDefinitionEditor.width) / 2.0
            y: (parent.height - agentDefinitionEditor.height) / 2.0

            onOpened: {
                agentDefinitionEditor.z = rootItem.popupTopmostZIndex;
                rootItem.popupTopmostZIndex = rootItem.popupTopmostZIndex + 1;

                x = x + (index * 40);
                y = y + (index * 40);
            }

            onBringToFront: {
                agentDefinitionEditor.z = rootItem.popupTopmostZIndex;
                rootItem.popupTopmostZIndex = rootItem.popupTopmostZIndex + 1;
            }

            onClosed: {
                MasticEditorC.closeDefinition(model.QtObject);
            }

            onOpenHistory : {
                historyPanel.open();
            }
        }
    }
    
    // List of "Actions Editor(s)"
    Repeater {
        model: MasticEditorC.scenarioC ? MasticEditorC.scenarioC.openedActionsEditorsControllers : 0;
        
        delegate: Scenario.ActionEditor {
            id: actionEditor
            
            controller : MasticEditorC.scenarioC
            panelController: model.QtObject

            // Center popup
            x: (parent.width - actionEditor.width) / 2.0
            y: (parent.height - actionEditor.height) / 2.0
            
            onOpened: {
                actionEditor.z = rootItem.popupTopmostZIndex;
                rootItem.popupTopmostZIndex = rootItem.popupTopmostZIndex + 1;
            }
            
            onBringToFront: {
                actionEditor.z = rootItem.popupTopmostZIndex;
                rootItem.popupTopmostZIndex = rootItem.popupTopmostZIndex + 1;
            }
            
            onClosed: {
                MasticEditorC.closeActionEditor(model.QtObject);
            }
        }
    }

    // AgentMappingHistory
    Agent.HistoryPanel {
        id: historyPanel

        // Center popup
        x: (parent.width - historyPanel.width) / 2.0
        y: (parent.height - historyPanel.height) / 2.0

        onOpened: {
            historyPanel.z = rootItem.popupTopmostZIndex;
            rootItem.popupTopmostZIndex = rootItem.popupTopmostZIndex + 1;
        }

        onBringToFront: {
            historyPanel.z = rootItem.popupTopmostZIndex;
            rootItem.popupTopmostZIndex = rootItem.popupTopmostZIndex + 1;
        }
    }
}
