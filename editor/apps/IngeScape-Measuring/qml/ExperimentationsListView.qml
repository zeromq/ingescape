/*
 *	IngeScape Measuring
 *
 *  Copyright © 2019 Ingenuity i/o. All rights reserved.
 *
 *	See license terms for the rights and conditions
 *	defined by copyright holders.
 *
 *
 *	Contributors:
 *      Vincent Peyruqueou <peyruqueou@ingenuity.io>
 *
 */

import QtQuick 2.8
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4

import I2Quick 1.0

import INGESCAPE 1.0

//import "theme" as Theme
import "popup" as Popup


Item {

    id: rootItem

    //anchors.fill: parent


    //--------------------------------------------------------
    //
    //
    // Properties
    //
    //
    //--------------------------------------------------------

    property ExperimentationsListController controller: null;


    //--------------------------------------------------------
    //
    //
    // Content
    //
    //
    //--------------------------------------------------------

    Rectangle {
        id: background

        anchors.fill: parent

        color: "#EEAAAA"
    }

    Text {
        id: title

        anchors {
            top: parent.top
            horizontalCenter: parent.horizontalCenter
        }

        text: "Experimentations"
    }

    Button {
        id: btnNewExpe

        anchors {
            top: title.bottom
            topMargin: 20
            horizontalCenter: parent.horizontalCenter
        }

        text: "New Experimentation"

        onClicked: {
            createExperimentationPopup.open();
        }
    }


    Column {
        id: columnExperimentationsGroups

        anchors {
            top: btnNewExpe.bottom
            topMargin: 20
            left: parent.left
            leftMargin: 20
            right: parent.right
            rightMargin: 20
            //bottom: parent.bottom
        }
        spacing: 10

        Repeater {
            model: controller ? controller.allExperimentationsGroups : null

            delegate: componentExperimentationsGroup
        }
    }


    //
    // Create Experimentation Popup
    //
    Popup.CreateExperimentationPopup {
        id: createExperimentationPopup

        //anchors.centerIn: parent

        controller: rootItem.controller
    }


    //
    // Component for "Experimentations Group (View Model)"
    //
    Component {
        id: componentExperimentationsGroup

        Rectangle {
            id: rootExperimentationsGroup

            property ExperimentationsGroupVM experimentationsGroup: model.QtObject

            width: parent.width
            height: txtName.height + columnExperimentations.anchors.topMargin + columnExperimentations.height

            color: "#44333333"
            border {
                color: "black"
                width: 1
            }

            Text {
                id: txtName

                anchors {
                    left: parent.left
                    top: parent.top
                }

                text: rootExperimentationsGroup.experimentationsGroup ? rootExperimentationsGroup.experimentationsGroup.name : ""

                color: IngeScapeTheme.whiteColor
                font {
                    family: IngeScapeTheme.textFontFamily
                    weight : Font.Medium
                    pixelSize : 16
                }
            }

            Column {
                id: columnExperimentations

                anchors {
                    top: txtName.bottom
                    topMargin: 10
                    left: parent.left
                    leftMargin: 30
                    right: parent.right
                }

                Repeater {
                    model: experimentationsGroup ? experimentationsGroup.experimentations : null

                    delegate: Rectangle {
                        id: rootExperimentation

                        property ExperimentationM experimentation: model.QtObject

                        width: parent.width
                        height: 30

                        color: "#443333AA"
                        border {
                            color: "black"
                            width: 1
                        }

                        Row {
                            spacing: 20

                            anchors {
                                left: parent.left
                                verticalCenter: parent.verticalCenter
                            }

                            Text {
                                text: rootExperimentation.experimentation ? rootExperimentation.experimentation.name : ""

                                color: IngeScapeTheme.whiteColor
                                font {
                                    family: IngeScapeTheme.textFontFamily
                                    weight : Font.Medium
                                    pixelSize : 12
                                }
                            }

                            Text {
                                text: rootExperimentation.experimentation ? rootExperimentation.experimentation.creationDate.toLocaleString(Qt.locale(), "dd/MM/yyyy hh:mm:ss") : ""

                                color: IngeScapeTheme.whiteColor
                                font {
                                    family: IngeScapeTheme.textFontFamily
                                    weight : Font.Medium
                                    pixelSize : 12
                                }
                            }
                        }

                        /*MouseArea {
                            id: mouseAreaExperimentation

                            anchors.fill: parent

                            hoverEnabled: true
                        }*/

                        Row {
                            spacing: 20

                            anchors {
                                right: parent.right
                            }
                            height: parent.height

                            //visible: mouseAreaExperimentation.containsMouse

                            Button {
                                id: btnOpen

                                text: "Open"

                                width: 100
                                height: parent.height

                                onClicked: {
                                    if (rootExperimentation.experimentation && rootItem.controller)
                                    {
                                        console.log("QML: Open " + rootExperimentation.experimentation.name);

                                        // Open the experimentation of the group
                                        rootItem.controller.openExperimentationOfGroup(rootExperimentation.experimentation, rootExperimentationsGroup.experimentationsGroup);
                                    }
                                }
                            }

                            Button {
                                id: btnDelete

                                text: "Delete"

                                width: 100
                                height: parent.height

                                onClicked: {
                                    if (rootExperimentation.experimentation && rootItem.controller)
                                    {
                                        console.log("QML: Delete " + rootExperimentation.experimentation.name);

                                        // Delete the experimentation of the group
                                        rootItem.controller.deleteExperimentationOfGroup(rootExperimentation.experimentation, rootExperimentationsGroup.experimentationsGroup);
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}
