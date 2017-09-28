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
    Rectangle {
        id: bottomPanel

        anchors {
            left: leftPanel.right
            right: parent.right
            bottom: parent.bottom
        }

        height: MasticTheme.bottomPanelHeight

        color: "#AAFFAA"

        Text {
            anchors.centerIn: parent

            text: qsTr("Bottom Panel")

            font: MasticTheme.headingFont

            color: MasticTheme.whiteColor
        }
    }



    //
    // Left panel: TODO: create tabs
    //
    Rectangle {
        id: leftPanel

        anchors {
            left: parent.left
            top: parent.top
            bottom: parent.bottom
        }

        width: MasticTheme.leftPanelWidth

        color: "#444444"

        Agent.AgentsList {
            id: agentsList

            anchors.fill: parent

            controller: MasticEditorC.agentsSupervisionC
        }
    }


    //
    // Foreground: to add AgentDefinitionEditor(s), AgentMappingHistory ?
    //
}
