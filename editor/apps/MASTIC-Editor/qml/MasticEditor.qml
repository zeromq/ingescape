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
    // Left panel: TODO: create tabs
    //
    I2CustomRectangle {
        id: leftPanel

        anchors {
            left: parent.left
            top: parent.top
            bottom: parent.bottom
        }

        width: MasticTheme.leftPanelWidth

        color: MasticTheme.agentsListBackgroundColor
        fuzzyRadius: 8

        TabView {
            id: tabs

            anchors {
                fill: parent
            }

            Tab {
                title: "Agents"

                    Agent.AgentsList {
                        id: agentsList

                        anchors.fill: parent

                        controller: MasticEditorC.agentsSupervisionC
                    }

            }

            Tab {
                title: "Actions"

                Scenario.ActionsList {
                    id: actionsList

                    anchors.fill: parent

                    controller: MasticEditorC.scenarioC
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
            }

            onBringToFront: {
                agentDefinitionEditor.z = rootItem.popupTopmostZIndex;
                rootItem.popupTopmostZIndex = rootItem.popupTopmostZIndex + 1;
            }

            onClosed: {
                MasticEditorC.closeDefinition(model.QtObject);
            }
        }
    }

    // List of "Actions Editor(s)"
    Repeater {
        model: MasticEditorC.scenarioC ? MasticEditorC.scenarioC.openedActionsEditors : 0;

        delegate: Scenario.ActionEditor {
            id: actionEditor

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




    // AgentMappingHistory ?
}
