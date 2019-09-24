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

                host: model.QtObject
                controller: rootItem.controller

                onNeedConfirmationToDeleteHostInList: {
                    // Set the host
                    deleteConfirmationPopup.host = model.QtObject;

                    // Open the popup
                    deleteConfirmationPopup.open();
                }

                onNeedRenameHostInList: {
                    // Set the host
                    renameHostPopup.host = model.QtObject;

                    // Open the popup
                    renameHostPopup.open();
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

        confirmationText: host ? "Do you want to remove " + host.name + "?"
                               : ""

        onConfirmed: {
            if (rootItem.controller && deleteConfirmationPopup.host) {
                rootItem.controller.deleteHost(deleteConfirmationPopup.host);
            }
        }
    }


    //
    // Popup about "Rename Host"
    //
    Popup.RenamePopup {
        id: renameHostPopup

        property HostVM host: null

        previousHostName: host ? host.name : ""

        /*onConfirmed: {
            console.log("host renamed " + renameHostPopup.newName);
        }*/
    }

}
