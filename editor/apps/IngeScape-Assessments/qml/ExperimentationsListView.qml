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
    // Functions
    //
    //
    //--------------------------------------------------------

    function openExperimentation(experimentation)
    {
        if (experimentation && IngeScapeAssessmentsC.experimentationC)
        {
            console.log("QML: Open " + experimentation.name);

            // Open the experimentation of the group
            IngeScapeAssessmentsC.experimentationC.currentExperimentation = experimentation;
        }
    }


    function deleteExperimentation(experimentation, group)
    {
        if (experimentation && rootItem.controller)
        {
            console.log("QML: Delete " + experimentation.name);

            // Delete the experimentation of the group
            rootItem.controller.deleteExperimentationOfGroup(experimentation, group);
        }
    }



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

        color: IngeScapeTheme.veryLightGreyColor
    }

    Item {
        id: title

        anchors {
            top: parent.top
            topMargin: 10
            horizontalCenter: parent.horizontalCenter
        }

        width: childrenRect.width
        height: childrenRect.height

        I2SvgItem {
            id: logo

            anchors {
                top: parent.top
                horizontalCenter: parent.horizontalCenter
            }

            svgFileCache: IngeScapeAssessmentsTheme.svgFileIngeScapeAssessments
            svgElementId: "logo-home"
        }

        Row {
            id: titleRow
            anchors {
                top: logo.bottom
                topMargin: 36
                horizontalCenter: parent.horizontalCenter
            }

            spacing: 15
            Row {
                id: ingeRow
                spacing: 0
                Text {
                    id: ingeText
                    text: "INGE"

                    color: IngeScapeAssessmentsTheme.ingescapeTitleDarkGrey

                    verticalAlignment: Text.AlignBottom

                    font {
                        family: IngeScapeTheme.loadingFontFamily
                        pixelSize: 54
                    }
                }

                Text {
                    text: "SCAPE"

                    color: IngeScapeAssessmentsTheme.ingescapeTitleMediumGrey
                    verticalAlignment: Text.AlignBottom

                    font {
                        family: IngeScapeTheme.loadingFontFamily
                        pixelSize: 54
                    }
                }
            }

            Text {
                anchors {
                    leftMargin: 15
                    top: ingeRow.top
                    topMargin: 14

                }

                text: "ASSESSMENTS"
                verticalAlignment: Text.AlignBottom

                color: IngeScapeAssessmentsTheme.ingescapeTitleMediumGrey

                font {
                    family: IngeScapeTheme.loadingFontFamily
                    pixelSize: 36
                }
            }
        }

        Text {
            anchors {
                top: titleRow.bottom
                topMargin: 24
                horizontalCenter: titleRow.horizontalCenter
            }

            text: "FOR ERGONOMICS & HUMAN FACTORS"

            color: IngeScapeAssessmentsTheme.ingescapeTitleLightGrey

            font {
                family: IngeScapeTheme.loadingFontFamily
                pixelSize: 24
            }
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

        style: IngeScapeAssessmentsButtonStyle {
            text: "NEW EXPERIMENT"
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

            IngeScapeAssessmentsListHeader {
                id: headerBackground
                anchors {
                    top: parent.top
                    left: parent.left
                    right: parent.right
                }

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

            ListView {
                id: listViewExperimentations

                anchors {
                    top: headerBackground.bottom
                    left: parent.left
                    right: parent.right
                }

                property real itemHeight: 38
                property real bottomSeparatorHeight: 2

                height: (count > 0) ? (itemHeight * count) - bottomSeparatorHeight : 0

                model: rootExperimentationsGroup.experimentationsGroup ? rootExperimentationsGroup.experimentationsGroup.experimentations : null

                delegate: Rectangle {
                    id: rootExperimentation

                    property ExperimentationM experimentation: model.QtObject
                    property bool isMouseHovering: itemMouseArea.containsMouse || btnDelete.hovered || btnOpen.hovered

                    width: parent.width
                    height: listViewExperimentations.itemHeight
                    color: rootExperimentation.isMouseHovering ? IngeScapeTheme.veryLightGreyColor : IngeScapeTheme.whiteColor

                    MouseArea {
                        id: itemMouseArea
                        anchors.fill: parent
                        hoverEnabled: true
                        onDoubleClicked: {
                            rootItem.openExperimentation(rootExperimentation.experimentation)
                        }
                    }

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
                                    color: btnDelete.hovered ? IngeScapeAssessmentsTheme.blueButton_pressed : IngeScapeAssessmentsTheme.blueButton
                                    radius: 5
                                }
                            }

                            width: 40
                            height: 30

                            onClicked: {
                                rootItem.deleteExperimentation(rootExperimentation.experimentation, rootExperimentationsGroup.experimentationsGroup)
                            }
                        }

                        Button {
                            id: btnOpen

                            anchors.verticalCenter: parent.verticalCenter

                            width: 86
                            height: 30

                            onClicked: {
                                rootItem.openExperimentation(rootExperimentation.experimentation)
                            }

                            //FIXME correct font
                            style: IngeScapeAssessmentsButtonStyle {
                                text: "OPEN"
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
                        height: listViewExperimentations.bottomSeparatorHeight
                        color: IngeScapeTheme.veryLightGreyColor
                    }
                }
            }

            Rectangle {
                id: bottomShadow
                anchors {
                    top: listViewExperimentations.bottom
                    left: parent.left
                    right: parent.right
                }
                height: 4
                gradient: Gradient {
                    GradientStop { position: 0.0; color: IngeScapeTheme.whiteColor; }
                    GradientStop { position: 1.0; color: IngeScapeTheme.darkGreyColor; }
                }
            }
        }
    }
}
