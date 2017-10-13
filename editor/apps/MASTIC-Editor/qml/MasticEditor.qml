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

        Agent.AgentsList {
            id: agentsList

            anchors.fill: parent

            controller: MasticEditorC.agentsSupervisionC
        }
    }


    //
    // Foreground: to add AgentDefinitionEditor(s), AgentMappingHistory ?
    //
    Repeater {
        model: MasticEditorC.modelManager.openedDefinitions

        delegate: Agent.AgentDefinitionEditor {

            // Slot on signal "Clicked on Close Button"
            onClickedOnCloseButton: {
                MasticEditorC.closeDefinition(model.QtObject);
            }
        }
    }
}
