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
 *      Vincent Peyruqueou <peyruqueou@ingenuity.io>
 *
 */

import QtQuick 2.8
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4

import I2Quick 1.0

import INGESCAPE 1.0

// agentsmapping sub-directory
import "agentsmapping" as AgentsMapping
//import "theme" as Theme

// Popups
import "./popup/" as Popups;

Item {
    id: rootItem

    anchors.fill: parent

    //--------------------------------
    //
    // Properties
    //
    //--------------------------------

    // Controller associated to our view
    property AgentsMappingController controller : null;

    //-----------------------------------------
    //
    // Behaviors
    //
    //-----------------------------------------

    Component.onCompleted: {
        if (rootItem.controller)
        {
            rootItem.controller.isLoadedView = true;
        }
    }

    //--------------------------------
    //
    // Content
    //
    //--------------------------------

    Item {
        id: content
        anchors.fill: parent

        onWidthChanged: {
            if (controller)
            {
                controller.viewWidth = width;
            }
        }
        onHeightChanged: {
            if (controller)
            {
                controller.viewHeight = height;
            }
        }

        //
        // Node graph view
        //
        AgentsMapping.NodeGraphView {
            id: nodeGraphView
            anchors.fill: parent
            controller: rootItem.controller
        }
    }
}
