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
 *      Vincent Peyruqueou <peyruqueou@ingenuity.io>
 *
 */

import QtQuick 2.8
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4

import I2Quick 1.0

import MASTIC 1.0

// agentsmapping sub-directory
import "agentsmapping" as AgentsMapping


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

    Item {
        id: content

        anchors.fill: parent


        //
        // Node graph view
        //
        AgentsMapping.NodeGraphView {
            id: nodeGraphView

            anchors.fill: parent

            controller: rootItem.controller
        }

        Button {
            id: btnActivateMapping

            anchors {
                bottom: parent.bottom
                left: parent.left
                margins: 10
            }
            text: checked ? "DES-active le mapping" : "ACTIVE le mapping"

            checkable: true
            checked: MasticEditorC.modelManager.isActivatedMapping

            onCheckedChanged: {
                MasticEditorC.modelManager.isActivatedMapping = checked;
            }
        }
    }
}
