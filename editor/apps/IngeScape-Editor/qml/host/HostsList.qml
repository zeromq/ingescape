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
import "../popup" as PopUp


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

                host: model.QtObject
                controller: rootItem.controller
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
    // Menu popup with options
    //
    /*PopUp.MenuPopup {
        id : popupOptions

        anchors {
            top: rootItem.top
            left: rootItem.right
            leftMargin: 2
        }

        readonly property int optionHeight: 30

        // Get height from children
        height: popUpBackground.y + popUpBackground.height
        width: 200

        isModal: true;
        layerColor: "transparent"
        dismissOnOutsideTap : true;

        keepRelativePositionToInitialParent : true;

        onClosed: {

        }
        onOpened: {

        }

        Rectangle {
            id: popUpBackground
            height: buttons.y + buttons.height
            anchors {
                right: parent.right
                left: parent.left
            }
            color: IngeScapeTheme.veryDarkGreyColor
            radius: 5
            border {
                color: IngeScapeTheme.blueGreyColor2
                width: 1
            }

            Column {
                id: buttons
                anchors {
                    right: parent.right
                    left: parent.left
                }

                Button {
                    id: optionReboot

                    height: popupOptions.optionHeight
                    width: parent.width

                    text: qsTr("Reboot")
                    enabled: false

                    style: Theme.ButtonStyleOfOption {

                    }

                    onClicked: {
                        console.log("QML: click on option 'Reboot'");

                        popupOptions.close();
                    }
                }

                Button {
                    id: optionViewScreen

                    height: popupOptions.optionHeight
                    width: parent.width

                    text: qsTr("View Screen")
                    enabled: false

                    style: Theme.ButtonStyleOfOption {

                    }

                    onClicked: {
                        console.log("QML: click on option 'View Screen'");

                        popupOptions.close();
                    }
                }

                Button {
                    id: optionRemove

                    height: popupOptions.optionHeight
                    width: parent.width

                    text: qsTr("Remove")

                    // FIXME: option "remove host"
                    //enabled: (rootItem.host && rootItem.host.isON)
                    enabled: false

                    style: Theme.ButtonStyleOfOption {
                        isVisibleSeparation: false
                    }

                    onClicked: {
                        console.log("QML: click on option 'Remove Host'");

                        popupOptions.close();
                    }
                }
            }
        }
    }*/

}
