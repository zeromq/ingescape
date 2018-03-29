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
 *      Justine Limoges    <limoges@ingenuity.io>
 *      Vincent Peyruqueou <peyruqueou@ingenuity.io>
 *
 */

import QtQuick 2.8
import QtQuick.Window 2.3
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4

import I2Quick 1.0

import MASTIC 1.0


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




    //--------------------------------
    //
    // Content
    //
    //--------------------------------

    // allowing to deselect selected host
    MouseArea {
        anchors.fill: parent
        onClicked:  {
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

        style: MasticScrollViewStyle {
        }

        // Content of our scrollview
        ListView {
            id: hostsList

            model: controller.hostsList

            delegate: componentHostListItem

            height: contentHeight

            anchors
            {
                left:parent.left
                right:parent.right
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

        color : MasticTheme.selectedTabsBackgroundColor
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

        color: MasticTheme.leftPanelBackgroundColor

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

                color: MasticTheme.leftPanelBackgroundColor

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
                    color: MasticTheme.agentsListLabelColor
                    font: MasticTheme.headingFont
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
                    color: MasticTheme.agentsListTextColor
                    font: MasticTheme.heading2Font
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
                        color: model.isON ? MasticTheme.agentsListLabelColor : MasticTheme.agentOFFLabelColor
                        font: MasticTheme.normalFont
                    }

                    height: contentHeight

                    anchors
                    {
                        left:parent.left
                        right:parent.right
                    }
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
                    fileCache: MasticTheme.svgFileMASTIC

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

            color: MasticTheme.editorsBackgroundColor

            GstVideoReceiver{
                id:receiver
                anchors.margins: 10
                anchors.fill: parent
                endpoint: "tcp://10.0.0.10:5660"
            }
        }
    }
    */

}




