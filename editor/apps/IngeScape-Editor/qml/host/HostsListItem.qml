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

    anchors {
        left: parent.left
        right: parent.right
    }
    //width: IngeScapeEditorTheme.leftPanelWidth
    height: 5 + hostName.height + hostInfos.spacing + hostIP.height + hostInfos.spacing + listViewOfAgents.height + 6


    //--------------------------------
    //
    // Properties
    //
    //--------------------------------

    // Controller associated to our view
    property HostsSupervisionController controller : null;

    // View model of host associated to our view
    property HostVM host : null;


    //--------------------------------
    //
    // Signals
    //
    //--------------------------------

    // signal emitted when the user clicks on the "remove" button
    signal needConfirmationToDeleteHostInList();

    // signal emitted when the user clicks on the "remove" button of an agent in the list
    signal needConfirmationToDeleteAgentInHost(AgentM agent);

    // signal emitted when the user clicks on the "rename" option
    //signal needRenameHostInList();

    // signal emitted when the user clicks on the "edit command line" option of an agent in the list
    signal needEditAgentCommandLine(AgentM agent);


    //--------------------------------
    //
    // Content
    //
    //--------------------------------

    MouseArea {
        id: mouseAreaHost

        anchors.fill: parent

        hoverEnabled: true

        Rectangle {
            anchors.fill: parent

            border.width: 0

            //color: mouseAreaHost.containsMouse ? IngeScapeEditorTheme.agentsListItemRollOverBackgroundColor : IngeScapeEditorTheme.agentsListItemBackgroundColor
            color: IngeScapeEditorTheme.agentsListItemBackgroundColor

            // bottom separator
            Rectangle {
                anchors {
                    bottom: parent.bottom
                    left: parent.left
                    right: parent.right
                }

                color: IngeScapeTheme.blackColor

                height: 1
            }

            // Selection feedback
            Rectangle {
                id: selectionFeedback

                anchors {
                    left : parent.left
                    top : parent.top
                    bottom: parent.bottom
                }
                width : 6

                visible: controller && rootItem.host && (controller.selectedHost === rootItem.host)

                color: IngeScapeTheme.selectionColor
            }

            // Remove button
            /*LabellessSvgButton {
                id: removeButton

                anchors {
                    top: parent.top
                    topMargin: 10
                    right: parent.right
                    rightMargin: 10
                }

                visible: (host && !host.isON)
                opacity: mouseAreaHost.containsMouse ? 1 : 0

                releasedID: "delete"
                pressedID: releasedID + "-pressed"
                disabledID : releasedID


                onClicked: {
                    // Emit the signal to display a confirmation popup
                    rootItem.needConfirmationToDeleteHostInList();
                }
            }*/

            // Button Options
            LabellessSvgButton {
                id: btnOptions

                anchors {
                    top: parent.top
                    topMargin: 10
                    right : parent.right
                    rightMargin: 10
                }

                pressedID: releasedID + "-pressed"
                releasedID: "button-options"
                disabledID : releasedID


                onClicked: {
                    // console.log("QML: Open host options...");

                    // Parent must be host item and not the list to have good x and y value
                    popupHostOptions.parent = rootItem;

                    // Open the popup with options
                    popupHostOptions.openInScreen();
                }
            }

            // Stream button
            /*LabellessSvgButton {
                id: streamButton

                anchors {
                    right: parent.right
                    verticalCenter: parent.verticalCenter
                    rightMargin: 5
                }

                visible: model.canProvideStream

                enabled: visible


                fileCache: IngeScapeEditorTheme.svgFileIngeScapeEditor

                pressedID: releasedID + "-pressed"
                releasedID: model.isStreaming ? "on" : "off"
                disabledID : releasedID


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
                    model.QtObject.changeStreamState();
                }
            }*/
        }
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

        spacing: 8

        // Name
        Text {
            id: hostName

            anchors {
                left : parent.left
                right : parent.right
            }

            elide: Text.ElideRight

            text: host ? host.name : ""

            color: host && host.isON ? IngeScapeTheme.whiteColor : IngeScapeTheme.lightGreyColor

            font: IngeScapeTheme.headingFont
        }

        // IP address
        Text {
            id: hostIP

            anchors {
                left : parent.left
                right : parent.right
            }
            height: if (text == "") { 0 }

            elide: Text.ElideRight

            text: host && host.modelM ? host.modelM.ipAddress : ""

            color: IngeScapeTheme.lightBlueGreyColor
            font: IngeScapeTheme.heading2Font
        }


        //
        // List of agents on our host
        //
        ListView {
            id: listViewOfAgents

            anchors {
                left: parent.left
                right: parent.right
                //rightMargin: 30
            }
            height: contentHeight

            model: host ? host.sortedAgents : null

            interactive: false
            spacing: 5

            delegate: Rectangle {

                id: agentInHost

                anchors {
                    left: parent.left
                    right: parent.right
                }
                height: 20

                //color: "transparent"
                color: mouseAreaAgent.containsMouse ? IngeScapeEditorTheme.agentsListItemRollOverBackgroundColor : IngeScapeEditorTheme.agentsListItemBackgroundColor

                Text {
                    id: txtAgentName

                    anchors {
                        left : parent.left
                        leftMargin: 10
                        right: btnOptionsAgent.left
                        rightMargin: 10
                        verticalCenter: parent.verticalCenter
                    }

                    text: model.name

                    elide: Text.ElideRight

                    color: model.isON ? IngeScapeTheme.whiteColor : IngeScapeTheme.lightGreyColor

                    font: IngeScapeTheme.normalFont
                }

                MouseArea {
                    id: mouseAreaAgent

                    anchors.fill: parent

                    hoverEnabled: true
                    cursorShape: Qt.PointingHandCursor

                    onDoubleClicked: {
                        // ON
                        if (model.isON) {
                            //console.log("QML: Stop " + model.name);
                            host.stopAgent(model.QtObject);
                        }
                        // OFF
                        else {
                            //console.log("QML: Start " + model.name);
                            host.startAgent(model.QtObject);
                        }
                    }
                }

                Controls2.ToolTip {
                    delay: 400
                    visible: mouseAreaAgent.containsMouse

                    text: model ? model.name + " (" + model.peerId + ")\n" + model.commandLine
                                : ""
                }

                // Button Options
                LabellessSvgButton {
                    id: btnOptionsAgent

                    anchors {
                        right: parent.right
                        //right: (model && model.isON) ? parent.right : btnRemoveAgent.left
                        rightMargin: 25
                        verticalCenter: parent.verticalCenter
                    }

                    opacity: (btnOptionsAgent.hovered || mouseAreaAgent.containsMouse) ? 1.0 : 0.0

                    releasedID: "button-options"
                    pressedID: releasedID + "-pressed"
                    disabledID : releasedID

                    onClicked: {
                        //console.log("QML: Open agent options...");

                        // Parent must be agent item and not the list to have good x and y value
                        popupAgentOptions.parent = agentInHost;

                        popupAgentOptions.agent = model.QtObject;

                        // Open the popup with options
                        popupAgentOptions.openInScreen();
                    }
                }

                // Remove button
                LabellessSvgButton {
                    id: btnRemoveAgent

                    anchors {
                        right: parent.right
                        rightMargin: 5
                        verticalCenter: parent.verticalCenter
                    }

                    visible: !model.isON
                    opacity: (btnRemoveAgent.hovered || mouseAreaAgent.containsMouse) ? 1.0 : 0.0


                    pressedID: releasedID + "-pressed"
                    releasedID: "delete"
                    disabledID : releasedID


                    onClicked: {
                        if (controller)
                        {
                            //console.log("QML: Remove agent model " + model.name + " on " + host.name);

                            // Remove the model of agent from our host
                            controller.removeAgentModelFromHost(model.QtObject, host);
                        }
                    }
                }
            }
        }

    }


    //
    // Menu popup with options about our host
    //
    Popup.MenuPopup {
        id : popupHostOptions

        anchors {
            top: rootItem.top
            left: rootItem.right
            leftMargin: 2
        }

        readonly property int optionHeight: 30

        // Get height from children
        height: bgPopupHostOptions.y + bgPopupHostOptions.height
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
            id: bgPopupHostOptions

            height: hostOptions.y + hostOptions.height
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
                id: hostOptions

                anchors {
                    right: parent.right
                    left: parent.left
                }

                Button {
                    id: optionRename

                    height: popupHostOptions.optionHeight
                    width: parent.width

                    text: qsTr("Rename")

                    //enabled: (rootItem.host && !rootItem.host.isON)
                    enabled: false

                    style: Theme.ButtonStyleOfOption {
                    }

                    onClicked: {
                        // Emit the signal to display a popup about rename our host
                        rootItem.needRenameHostInList();

                        popupHostOptions.close();
                    }
                }

                Button {
                    id: optionRemove

                    height: popupHostOptions.optionHeight
                    width: parent.width

                    text: qsTr("Remove")

                    enabled: (rootItem.host && !rootItem.host.isON)

                    style: Theme.ButtonStyleOfOption {
                    }

                    onClicked: {
                        // Emit the signal to display a confirmation popup
                        rootItem.needConfirmationToDeleteHostInList();

                        popupHostOptions.close();
                    }
                }

                Button {
                    id: optionReboot

                    height: popupHostOptions.optionHeight
                    width: parent.width

                    text: qsTr("Reboot")
                    enabled: false

                    style: Theme.ButtonStyleOfOption {

                    }

                    onClicked: {
                        console.log("QML: click on option 'Reboot'");

                        popupHostOptions.close();
                    }
                }

                Button {
                    id: optionViewScreen

                    height: popupHostOptions.optionHeight
                    width: parent.width

                    text: qsTr("View Screen")
                    enabled: false

                    style: Theme.ButtonStyleOfOption {
                        isVisibleSeparation: false
                    }

                    onClicked: {
                        console.log("QML: click on option 'View Screen'");

                        popupHostOptions.close();
                    }
                }
            }
        }
    }


    //
    // Menu popup with options about an agent of our host
    //
    Popup.MenuPopup {
        id : popupAgentOptions

        anchors {
            top: rootItem.top
            left: rootItem.right
            leftMargin: -60
        }

        property AgentM agent: null
        readonly property int optionHeight: 30

        // Get height from children
        height: bgPopupAgentOptions.y + bgPopupAgentOptions.height
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
            id: bgPopupAgentOptions

            height: agentOptions.y + agentOptions.height
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
                id: agentOptions

                anchors {
                    right: parent.right
                    left: parent.left
                }

                Button {
                    id: optionStartStop

                    height: popupAgentOptions.optionHeight
                    width: parent.width

                    text: (popupAgentOptions.agent && popupAgentOptions.agent.isON) ? qsTr("Stop") : qsTr("Start")

                    style: Theme.ButtonStyleOfOption {
                    }

                    onClicked: {
                        if (rootItem.host && popupAgentOptions.agent)
                        {
                            // ON
                            if (popupAgentOptions.agent.isON)
                            {
                                console.log("QML: Stop " + popupAgentOptions.agent.name);
                                rootItem.host.stopAgent(popupAgentOptions.agent);
                            }
                            // OFF
                            else
                            {
                                console.log("QML: Start " + popupAgentOptions.agent.name);
                                rootItem.host.startAgent(popupAgentOptions.agent);
                            }
                        }

                        popupAgentOptions.close();
                    }
                }

                Button {
                    id: optionStartClone

                    height: popupAgentOptions.optionHeight
                    width: parent.width

                    text: qsTr("Start again")

                    visible: (popupAgentOptions.agent && popupAgentOptions.agent.isON)

                    style: Theme.ButtonStyleOfOption {

                    }

                    onClicked: {
                        if (rootItem.host && popupAgentOptions.agent)
                        {
                            rootItem.host.startAgent(popupAgentOptions.agent);
                        }

                        popupAgentOptions.close();
                    }
                }

                Button {
                    id: optionRemoveAgent

                    height: popupAgentOptions.optionHeight
                    width: parent.width

                    text: qsTr("Remove")

                    enabled: (popupAgentOptions.agent && !popupAgentOptions.agent.isON)

                    style: Theme.ButtonStyleOfOption {
                    }

                    onClicked: {
                        // Emit the signal to display a confirmation popup
                        rootItem.needConfirmationToDeleteAgentInHost(popupAgentOptions.agent);

                        popupAgentOptions.close();
                    }
                }

                Button {
                    id: optionEditCommandLine

                    height: popupAgentOptions.optionHeight
                    width: parent.width

                    text: qsTr("Edit command line")

                    enabled: (popupAgentOptions.agent && !popupAgentOptions.agent.isON)

                    style: Theme.ButtonStyleOfOption {
                        isVisibleSeparation: false
                    }

                    onClicked: {
                        // Emit the signal to display an edition popup
                        rootItem.needEditAgentCommandLine(popupAgentOptions.agent)

                        popupAgentOptions.close();
                    }
                }

            }
        }
    }

}


