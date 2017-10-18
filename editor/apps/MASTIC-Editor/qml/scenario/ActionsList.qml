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
 *      Vincent Peyruqueou <peyruqueou@ingenuity.io>
 *      Alexandre Lemort   <lemort@ingenuity.io>
 *
 */

import QtQuick 2.8
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4

import I2Quick 1.0

import MASTIC 1.0


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


    //
    // List of actions
    //
    ListView {
        id: actionsList

        anchors {
            top: header.bottom
            bottom: parent.bottom
            left: parent.left
            right: parent.right
        }

        model: controller.actionsList

        delegate: componentActionsListItem

        /*onCurrentIndexChanged: {
            //console.log("onCurrentIndexChanged " + agentsList.currentIndex);
            console.log("onCurrentIndexChanged " + model.get(agentsList.currentIndex).name);
        }
        onCurrentItemChanged: {
            console.log("onCurrentItemChanged " + agentsList.currentItem);
        }*/

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


    //
    // Header
    //
    Rectangle {
        id: header

        anchors {
            top: parent.top
            left: parent.left
            right: parent.right
        }

        height: childrenRect.height

        color: MasticTheme.agentsListHeaderBackgroundColor


        Row {
            id: headerRow1

            height: 30

            anchors {
                top: parent.top
                left: parent.left
                right: parent.right
            }

            Text {
                id: txtSearch

                text: qsTr("Rechercher...")

                color: MasticTheme.agentsListLabelColor

                font: MasticTheme.normalFont
            }

            Text {
                id: txtFilter

                text: qsTr("Filtrer...")

                color:MasticTheme.agentsListLabelColor

                font: MasticTheme.normalFont
            }
        }

        Row {
            id: headerRow2

            height: 30

            anchors {
                top: headerRow1.bottom
                left: parent.left
                right: parent.right
            }

            Button {
                id: btnAddAction
                text: qsTr("Créer action")
                onClicked: {
                    console.log("Creation d'action")
                    if(controller)
                    {
                        controller.openActionEditor(null);
                    }
                }
            }

            Text {
                text: qsTr("Importer...")

                color: MasticTheme.agentsListLabelColor

                font: MasticTheme.normalFont
            }

            Text {
                text: qsTr("Exporter...")

                color: MasticTheme.agentsListLabelColor

                font: MasticTheme.normalFont
            }
        }


        Rectangle {
            anchors {
                bottom: headerRow2.bottom
                left: parent.left
                right: parent.right
            }

            color: "#17191F"

            height: 1
        }
    }



    //
    // Visual representation of an action in our list
    //
    Component {
        id: componentActionsListItem


        Item {
            id : actionListItem

            width: MasticTheme.leftPanelWidth
            height: model.hasOnlyDefinition ? 85 : 135

            Behavior on height {
                NumberAnimation {}
            }

            Rectangle {

                anchors {
                    fill: parent
                    leftMargin: 4
                    rightMargin: 4
                    topMargin: 4
                    bottomMargin: 4
                }
                radius: 5
                border {
                    width: 1
                    color: MasticTheme.whiteColor
                }
                color: actionListItem.ListView.isCurrentItem ? "blue" : MasticTheme.agentsListItemBackgroundColor

                Item {
                    id: actionRow

                    anchors {
                        fill: parent
                        leftMargin: 5
                        topMargin: 2
                    }

                    Button {
                        id: btnDeleteAction

                        anchors {
                            left: agentRow.right
                            top: agentRow.top
                        }

                        visible: true

                        text: "X"

                        onClicked: {
                            if (controller)
                            {
                                // Delete our action
                                controller.deleteAction(model.QtObject);
                            }
                        }
                    }

                    Column {
                        width: 175
                        anchors {
                            left : parent.left
                        }

                        Text {
                            id: actionName
                            text: model.actionModel.name

                            height: 25
                            color: MasticTheme.agentsListLabelColor
                            font: MasticTheme.heading2Font
                        }

                        Text {
                            id: actionStartTime
                            text: model.actionModel.startTime.toLocaleTimeString(Qt.locale(), "HH'h'mm")
                            visible: !model.hasOnlyDefinition

                            height: 25
                            color: MasticTheme.agentsListLabelColor
                            font: MasticTheme.normalFont
                        }  
                    }

                    MouseArea {
                        id: mouseAreaForSelection
                        anchors.fill: parent

                        onPressed: {
                            actionsList.currentIndex = index
                        }
                    }

                    Button {
                        id: btnEdition

                        text: "Edit"

                        anchors {
                            top: parent.top
                            right: parent.right
                        }
                        width: 175

                        onClicked: {
                            if (controller) {
                                // Open the action editor of our agent
                                controller.openActionEditor(model.QtObject);
                            }
                        }
                    }
                }
            }
        }
    }
}
