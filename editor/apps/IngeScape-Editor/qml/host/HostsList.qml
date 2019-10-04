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
import QtQuick.Window 2.3
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4
import QtQuick.Controls 2.0 as Controls2

import I2Quick 1.0

import INGESCAPE 1.0


// theme sub-directory
import "../theme" as Theme;

// parent-directory
import ".." as Editor;
import "../popup" as Popup


Item {
    id: rootItem

    anchors.fill: parent

    //--------------------------------
    //
    // Properties
    //
    //--------------------------------

    // Controller associated to our view
    property HostsSupervisionController controller : null;

    //--------------------------------
    //
    // Content
    //
    //--------------------------------

    // allowing to deselect selected host
    MouseArea {
        anchors.fill: parent
        onClicked: {
            if (controller.selectedHost)
            {
                controller.selectedHost = null;
            }
        }
    }

    //
    // List of hosts
    //
    ScrollView {
        id : hostsListScrollView

        anchors {
            top: parent.top
            topMargin: 78
            bottom: parent.bottom
            left: parent.left
            right: parent.right
        }

        // Prevent drag overshoot on Windows
        flickableItem.boundsBehavior: Flickable.OvershootBounds

        style: IngeScapeScrollViewStyle {
        }

        // Content of our scrollview
        ListView {
            id: hostsList

            anchors {
                left: parent.left
                right: parent.right
            }
            height: contentHeight

            model: controller.hostsList


            delegate: HostsListItem {
                id: hostsListItem

                //
                // Properties
                //
                host: model.QtObject
                controller: rootItem.controller


                //
                // Slots
                //
                onNeedConfirmationToDeleteHostInList: {
                    // Set the host
                    deleteConfirmationPopup.host = model.QtObject;
                    deleteConfirmationPopup.agent = null;

                    // Open the popup
                    deleteConfirmationPopup.open();
                }

                onNeedConfirmationToDeleteAgentInHost: {
                    // Set the host and the agent
                    deleteConfirmationPopup.host = model.QtObject;
                    deleteConfirmationPopup.agent = agent;

                    // Open the popup
                    deleteConfirmationPopup.open();
                }

                /*onNeedRenameHostInList: {
                    // Set the host
                    renameHostPopup.host = model.QtObject;

                    // Open the popup
                    renameHostPopup.open();
                }*/

                onNeedEditAgentCommandLine: {
                    // Set the agent
                    editCommandLinePopup.agent = agent;
                    editCommandLinePopup.haveToDuplicate = false;

                    // Open the popup
                    editCommandLinePopup.open();
                }

                onNeedDuplicateAgentWithOtherCommandLine: {
                    // Set the agent
                    editCommandLinePopup.agent = agent;
                    editCommandLinePopup.haveToDuplicate = true;

                    // Open the popup
                    editCommandLinePopup.open();
                }
            }


            //
            // Transition animations
            //
            add: Transition {
                NumberAnimation { property: "opacity"; from: 0.0; to: 1.0 }
                NumberAnimation { property: "scale"; from: 0.0; to: 1.0 }
            }

            displaced: Transition {
                NumberAnimation { properties: "x,y"; easing.type: Easing.OutBounce }

                // ensure opacity and scale values return to 1.0
                NumberAnimation { property: "opacity"; to: 1.0 }
                NumberAnimation { property: "scale"; to: 1.0 }
            }

            move: Transition {
                NumberAnimation { properties: "x,y"; easing.type: Easing.OutBounce }

                // ensure opacity and scale values return to 1.0
                NumberAnimation { property: "opacity"; to: 1.0 }
                NumberAnimation { property: "scale"; to: 1.0 }
            }

            remove: Transition {
                // ensure opacity and scale values return to 0.0
                NumberAnimation { property: "opacity"; to: 0.0 }
                NumberAnimation { property: "scale"; to: 0.0 }
            }

        }
    }


    //
    // Header
    //
    Rectangle {
        id: header

        anchors {
            top: parent.top
            left: parent.left
            right: parent.right
            bottom : hostsListScrollView.top
        }

        color : IngeScapeEditorTheme.selectedTabsBackgroundColor
    }


    //
    // Separator
    //
    Rectangle {
        anchors {
            bottom: hostsListScrollView.top
            left: parent.left
            right: parent.right
        }

        color: IngeScapeTheme.blackColor

        height: 1
    }


    //
    // Popup about "Delete Host" confirmation
    //
    ConfirmationPopup {
        id: deleteConfirmationPopup

        property HostVM host: null
        property AgentM agent: null

        confirmationText: host ? ("Do you want to remove " + (agent ? agent.name + " on " : "") + host.name + "?")
                               : ""

        onConfirmed: {
            if (rootItem.controller && deleteConfirmationPopup.host)
            {
                // Agent is defined, remove/delete it
                if (deleteConfirmationPopup.agent)
                {
                    // Remove the model of agent from the host
                    rootItem.controller.removeAgentModelFromHost(deleteConfirmationPopup.agent, deleteConfirmationPopup.host);
                }
                // Agent is NOT defined, remove/delete the host
                else
                {
                    rootItem.controller.deleteHost(deleteConfirmationPopup.host);
                }
            }
        }
    }


    //
    // Popup about "Rename Host"
    //
    /*Popup.RenamePopup {
        id: renameHostPopup

        property HostVM host: null

        previousHostName: host ? host.name : ""

        onNameValidated: {
            // Call the setter in C++ with event "nameChanged"
            host.name = newName;
        }
    }*/


    //
    // Popup about "Edit Command Line"
    //
    Popup.EditCommandLinePopup {
        id: editCommandLinePopup

        property AgentM agent: null

        agentName: agent ? agent.name : ""
        previousCommandLine: agent ? agent.commandLine : ""

        onCommandLineValidated: {
            if (agent)
            {
                // If we have to duplicate the agent
                if (editCommandLinePopup.haveToDuplicate)
                {
                    if (IngeScapeEditorC.modelManager) {
                        IngeScapeEditorC.modelManager.duplicateAgentWithNewCommandLine(agent, newCommandLine);
                    }
                }
                // Else, simply update its command line
                else
                {
                    // Call the setter in C++ (with event "commandLineChanged")
                    agent.commandLine = newCommandLine;
                }
            }
        }
    }

}
