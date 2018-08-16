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

import I2Quick 1.0

import INGESCAPE 1.0


// theme sub-directory
import "../theme" as Theme;

// parent-directory
import ".." as Editor;


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

    property var hostDeltaY: 0


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

            anchors
            {
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
            id : hostItem

            property var agentmodel: model.listOfAgents

            property int margin: 5

            height: hostInfos.height + margin*2

            anchors {
                left : parent.left
                right : parent.right
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

            Column
            {
                id:hostInfos
                y: margin

                anchors {
                    left : parent.left
                    right : parent.right
                    leftMargin: 28
                    rightMargin: 12
                }

                spacing : 4

                // Name
                Text {
                    id: hostName

                    anchors {
                        left : parent.left
                        right : parent.right
                    }
                    elide: Text.ElideRight

                    text: model.hostModel.name
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

                    text: model.hostModel.ipAddress
                    color: IngeScapeTheme.agentsListTextColor
                    font: IngeScapeTheme.heading2Font
                }

                // List of associated agents
                ListView {
                    id:listAgents
                    model: agentmodel
                    interactive: false

                    delegate: Text {

                        anchors {
                            left : parent.left
                            leftMargin: 10
                        }
                        elide: Text.ElideRight

                        text: qsTr("- %1").arg(model.name)
                        color: model.isON ? IngeScapeTheme.agentsListLabelColor : IngeScapeTheme.agentOFFLabelColor
                        font: IngeScapeTheme.normalFont
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
                    console.log("QML: Open options...");

                    var position = hostItem.mapToItem(rootItem, 0, 0);
                    //console.log("position = " + position);
                    rootItem.hostDeltaY = position.y

                    // Open the popup with options
                    popupOptions.open();
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
                    if(model.isStreaming)
                    {
                        streamPopup.close();
                        receiver.stop();
                    }
                    else
                    {
                        streamPopup.show();
                        streamPopup.title = model.hostModel.name;
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


    I2PopupBase {
        id : popupOptions

        anchors {
            top: rootItem.top
            topMargin: rootItem.hostDeltaY
            left: rootItem.right
            leftMargin: 2
        }

        property int optionHeight: 30

        // 2 options x optionHeight
        height: 2 * optionHeight
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
            anchors {
                fill: parent
            }
            color: IngeScapeTheme.veryDarkGreyColor
            radius: 5
            border {
                color: IngeScapeTheme.blueGreyColor2
                width: 1
            }

            Column {
                anchors {
                    fill: parent
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




