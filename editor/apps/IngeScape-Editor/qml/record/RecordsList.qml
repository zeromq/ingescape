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
 *      Bruno Lemenicier  <lemenicier@ingenuity.io>
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
    // List of records
    //
    ScrollView {
        id : recordsListScrollView

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
            id: recordsList

            model: controller.recordsList

            delegate: componentRecordListItem

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
            bottom : recordsListScrollView.top
        }

        color : IngeScapeTheme.selectedTabsBackgroundColor
    }


    //
    // Separator
    //
    Rectangle {
        anchors {
            bottom: recordsListScrollView.top
            left: parent.left
            right: parent.right
        }

        color: IngeScapeTheme.leftPanelBackgroundColor

        height: 1
    }



    //
    // Visual representation of an record in our list
    //
    Component {
        id: componentRecordListItem


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

                    text: model.recordModel.name
                    color: IngeScapeTheme.agentsListLabelColor
                    font: IngeScapeTheme.headingFont
                }

//                // IP address
//                Text {
//                    id: hostIP

//                    anchors {
//                        left : parent.left
//                        right : parent.right
//                    }
//                    elide: Text.ElideRight

//                    text: model.hostModel.ipAddress
//                    color: IngeScapeTheme.agentsListTextColor
//                    font: IngeScapeTheme.heading2Font
//                }


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

}




