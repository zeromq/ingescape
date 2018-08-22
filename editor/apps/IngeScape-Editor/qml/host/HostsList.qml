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
    property var controller : null;

    //--------------------------------
    //
    // Content
    //
    //--------------------------------

    // allowing to deselect selected host
    MouseArea {
        anchors.fill: parent
        onClicked: {
            if(controller.selectedHost)
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

            model: controller.hostsList

            delegate: componentHostListItem

            height: contentHeight

            anchors {
                left: parent.left
                right: parent.right
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

        color : IngeScapeTheme.selectedTabsBackgroundColor
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

        color: IngeScapeTheme.leftPanelBackgroundColor

        height: 1
    }



    //
    // Visual representation of an host in our list
    //
    Component {
        id: componentHostListItem

        Item {
            id: hostItem

            property var model_listOfAgents: model.listOfAgents

            //height: 5 + hostInfos.height + 6
            height: 5 + hostName.height + hostInfos.spacing + hostIP.height + hostInfos.spacing + listViewOfAgents.height + 6

            anchors {
                left: parent.left
                right: parent.right
            }

            // separator
            Rectangle {
                anchors {
                    bottom: parent.bottom
                    left: parent.left
                    right: parent.right
                }

                color: IngeScapeTheme.leftPanelBackgroundColor

                height: 1
            }

            Column {
                id: hostInfos

                anchors {
                    top: parent.top
                    topMargin: 5
                    left: parent.left
                    right: parent.right
                    leftMargin: 28
                    rightMargin: 12
                }

                spacing: 5

                // Name
                Text {
                    id: hostName

                    anchors {
                        left : parent.left
                        right : parent.right
                    }
                    elide: Text.ElideRight

                    text: model.name
                    color: IngeScapeTheme.agentsListLabelColor
                    font: IngeScapeTheme.headingFont
                }

                // IP address
                Text {
                    id: hostIP

                    anchors {
                        left : parent.left
                        right : parent.right
                    }
                    elide: Text.ElideRight

                    text: model.modelM.ipAddress
                    color: IngeScapeTheme.agentsListTextColor
                    font: IngeScapeTheme.heading2Font
                }

                // List of associated agents
                ListView {
                    id: listViewOfAgents

                    model: hostItem.model_listOfAgents

                    interactive: false

                    delegate: Text {

                        anchors {
                            left : parent.left
                            leftMargin: 10
                        }

                        text: model.name
                        elide: Text.ElideRight

                        color: model.isON ? IngeScapeTheme.agentsListLabelColor : IngeScapeTheme.agentOFFLabelColor

                        font: IngeScapeTheme.normalFont

                        MouseArea {
                            id: mouseAreaToolTip
                            anchors.fill: parent
                            acceptedButtons: Qt.NoButton
                            hoverEnabled: true
                            cursorShape: Qt.PointingHandCursor
                        }

                        // Add tooltip on name
                        Controls2.ToolTip.delay: 300
                        Controls2.ToolTip.visible: mouseAreaToolTip.containsMouse
                        Controls2.ToolTip.text: (model ? model.commandLine : "")
                    }

                    height: contentHeight

                    anchors {
                        left: parent.left
                        right: parent.right
                    }
                }

            }

            // Button Options
            Button {
                id: btnOptions

                anchors {
                    bottom: parent.bottom
                    bottomMargin: 10
                    right : parent.right
                    rightMargin: 10
                }

                activeFocusOnPress: true

                style: Theme.LabellessSvgButtonStyle {
                    fileCache: IngeScapeTheme.svgFileINGESCAPE

                    pressedID: releasedID + "-pressed"
                    releasedID: "button-options"
                    disabledID : releasedID
                }

                onClicked: {
                    // console.log("QML: Open options...");

                    // Parent must be host item and not the list to have good x and y value
                    popupOptions.parent = hostItem;
                    // Open the popup with options
                    popupOptions.openInScreen();
                }
            }

            Button {
                id: streamButton

                visible: model.canProvideStream

                activeFocusOnPress: true
                enabled: visible

                anchors {
                    right: parent.right
                    verticalCenter: parent.verticalCenter
                    rightMargin: 5
                }

                style: Theme.LabellessSvgButtonStyle {
                    fileCache: IngeScapeTheme.svgFileINGESCAPE

                    pressedID: releasedID + "-pressed"
                    releasedID: model.isStreaming ? "on" : "off"
                    disabledID : releasedID
                }

                onClicked: {
                    if (model.isStreaming)
                    {
                        streamPopup.close();
                        receiver.stop();
                    }
                    else
                    {
                        streamPopup.show();
                        streamPopup.title = model.name;
                        receiver.start();
                    }
                    model.QtObject.changeState();
                }
            }

            // GST not included in master branch
//            Connections {
//                    target: streamPopup
//                    onClosing: {
//                        receiver.stop();
//                        model.QtObject.changeState();
//                        close.accepted = true;
//                    }
//            }
       }


    }

    //
    // popup displaying stream: GST not included in master branch
    //
    /*
    Window {
        id: streamPopup
        width: 400
        height: 250

        Rectangle {

            anchors.fill: parent

            color: IngeScapeTheme.editorsBackgroundColor

            GstVideoReceiver{
                id:receiver
                anchors.margins: 10
                anchors.fill: parent
                endpoint: "tcp://10.0.0.10:5660"
            }
        }
    }
    */


    PopUp.MenuPopup  {
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

                    style: Theme.ButtonStyleOfOption {

                    }

                    onClicked: {
                        console.log("QML: clik on option 'Reboot'");

                        popupOptions.close();
                    }
                }

                Button {
                    id: optionViewScreen

                    height: popupOptions.optionHeight
                    width: parent.width

                    text: qsTr("View Screen")

                    style: Theme.ButtonStyleOfOption {
                        isVisibleSeparation: false
                    }

                    onClicked: {
                        console.log("QML: clik on option 'View Screen'");

                        popupOptions.close();
                    }
                }
            }
        }
    }

}




