/*
 *	IngeScape Assessments
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

import QtQuick 2.9
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4

import I2Quick 1.0

import INGESCAPE 1.0

//import "theme" as Theme
import "popup" as Popup


Item {

    id: rootItem


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

        color: IngeScapeTheme.veryDarkGreyColor
    }

    Text {
        id: title

        anchors {
            top: parent.top
            topMargin: 10
            horizontalCenter: parent.horizontalCenter
        }

        text: "Experimentations"

        color: IngeScapeTheme.whiteColor
        font {
            family: IngeScapeTheme.headingFont
            weight : Font.Medium
            pixelSize : 20
        }
    }

    Button {
        id: btnNewExpe

        anchors {
            right: columnExperimentationsGroups.right
            bottom: columnExperimentationsGroups.top
            bottomMargin: 9
        }
        height: 40
        width: 200

        style: ButtonStyle {
            label: Text {
                text: "NEW EXPERIMENT"
                verticalAlignment: Text.AlignVCenter
                horizontalAlignment: Text.AlignHCenter
                color: IngeScapeTheme.whiteColor

                //FIXME derived from headerFont
                font {
                    family: IngeScapeTheme.labelFontFamily
                    pixelSize: 18
                    weight: Font.Black
                }
            }

            background: Rectangle {
                //FIXME Move colors to IngeScapeTheme
                color: btnNewExpe.hovered ? "#239eb3" : "#007b90"
                radius: 5
            }
        }

        onClicked: {
            // Open the popup
            createExperimentationPopup.open();
        }
    }


    Column {
        id: columnExperimentationsGroups

        anchors {
            top: title.bottom
            topMargin: 74
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

        Item {
            id: rootExperimentationsGroup

            property ExperimentationsGroupVM experimentationsGroup: model.QtObject

            width: parent.width
            height: childrenRect.height

            //FIXME Rounded only on top corners
            Rectangle {
                id: headerBackground

                anchors {
                    top: parent.top
                    left: parent.left
                    right: parent.right
                }

                height: 40

                color: "#38444F" //TODO Move to IngeScapeTheme.qml

                Text {
                    id: textGoupName

                    anchors {
                        left: parent.left
                        leftMargin: 25
                        top: parent.top
                        bottom: parent.bottom
                    }

                    text: rootExperimentationsGroup.experimentationsGroup ? rootExperimentationsGroup.experimentationsGroup.name : ""
                    verticalAlignment: Text.AlignVCenter

                    color: IngeScapeTheme.whiteColor
                    font: IngeScapeTheme.headingFont
                }
            }

            Column {
                id: columnExperimentations

                anchors {
                    top: headerBackground.bottom
                    left: parent.left
                    right: parent.right
                }

                Repeater {
                    model: experimentationsGroup ? experimentationsGroup.experimentations : null

                    delegate: Rectangle {
                        id: rootExperimentation

                        property ExperimentationM experimentation: model.QtObject

                        width: parent.width
                        height: 38

                        Row {
                            anchors {
                                left: parent.left
                                leftMargin: 25
                                verticalCenter: parent.verticalCenter
                            }

                            Text {
                                text: rootExperimentation.experimentation ? rootExperimentation.experimentation.name : ""

                                width: 350
                                color: IngeScapeTheme.blackColor
                                font {
                                    family: IngeScapeTheme.textFontFamily
                                    weight: Font.Medium
                                    pixelSize: 14
                                }
                            }

                            Text {
                                text: rootExperimentation.experimentation ? rootExperimentation.experimentation.creationDate.toLocaleString(Qt.locale(), "dd/MM/yyyy - hh:mm:ss") : ""

                                color: IngeScapeTheme.blackColor
                                font {
                                    family: IngeScapeTheme.textFontFamily
                                    //weight: Font.Medium
                                    pixelSize: 14
                                }
                            }
                        }

                        Row {
                            spacing: 14

                            anchors {
                                right: parent.right
                                rightMargin: 18
                            }

                            height: parent.height

                            Button {
                                id: btnDelete

                                anchors.verticalCenter: parent.verticalCenter

                                //FIXME replace with trash can picto
                                style: ButtonStyle {
                                    label: Text {
                                        verticalAlignment: Text.AlignVCenter
                                        horizontalAlignment: Text.AlignHCenter
                                        color: IngeScapeTheme.whiteColor
                                        font {
                                            family: IngeScapeTheme.textFontFamily
                                            //weight: Font.Medium
                                            pixelSize: 14
                                            bold: true
                                        }
                                        text: "DEL"
                                    }

                                    background: Rectangle {
                                        //FIXME Move colors to IngeScapeTheme
                                        color: btnDelete.hovered ? "#239eb3" : "#007b90"
                                        radius: 5
                                    }
                                }

                                width: 40
                                height: 30

                                onClicked: {
                                    if (rootExperimentation.experimentation && rootItem.controller)
                                    {
                                        console.log("QML: Delete " + rootExperimentation.experimentation.name);

                                        // Delete the experimentation of the group
                                        rootItem.controller.deleteExperimentationOfGroup(rootExperimentation.experimentation, rootExperimentationsGroup.experimentationsGroup);
                                    }
                                }
                            }

                            Button {
                                id: btnOpen

                                anchors.verticalCenter: parent.verticalCenter

                                width: 86
                                height: 30

                                onClicked: {
                                    if (rootExperimentation.experimentation && IngeScapeAssessmentsC.experimentationC)
                                    {
                                        console.log("QML: Open " + rootExperimentation.experimentation.name);

                                        // Open the experimentation of the group
                                        IngeScapeAssessmentsC.experimentationC.currentExperimentation = rootExperimentation.experimentation;
                                    }
                                }


                                //FIXME correct font
                                style: ButtonStyle {
                                    label: Text {
                                        text: "OPEN"
                                        verticalAlignment: Text.AlignVCenter
                                        horizontalAlignment: Text.AlignHCenter
                                        color: IngeScapeTheme.whiteColor

                                        //FIXME derived from headerFont
                                        font {
                                            family: IngeScapeTheme.labelFontFamily
                                            pixelSize: 18
                                            weight: Font.Black
                                        }
                                    }

                                    background: Rectangle {
                                        //FIXME Move colors to IngeScapeTheme
                                        color: btnOpen.hovered ? "#239eb3" : "#007b90"
                                        radius: 5
                                    }
                                }
                            }
                        }

                        Rectangle {
                            id: bottomSeparator
                            anchors {
                                left: parent.left
                                right: parent.right
                                bottom: parent.bottom
                            }
                            height: 2
                            color: IngeScapeTheme.veryLightGreyColor
                        }
                    }
                }
            }

            Rectangle {
                id: bottomShade
                anchors {
                    left: parent.left
                    right: parent.right
                    bottom: parent.bottom
                }
                height: 4
                gradient: Gradient {
                    GradientStop { position: 0.0; color: IngeScapeTheme.whiteColor; }
                    GradientStop { position: 1.0; color: IngeScapeTheme.blackColor; }
                }
            }
        }
    }
}
