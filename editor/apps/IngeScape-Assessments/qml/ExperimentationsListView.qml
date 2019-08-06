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
                    font {
                        family: IngeScapeTheme.labelFontFamily
                        weight: Font.Black
                        pixelSize: 20
                    }
                }
            }

            Column {
                id: listViewExperimentations

                anchors {
                    top: headerBackground.bottom
                    left: parent.left
                    right: parent.right
                }

                property real itemHeight: 38
                property real bottomSeparatorHeight: 2

                height: (expeRepeater.count > 0) ? (itemHeight * expeRepeater.count) - bottomSeparatorHeight : 0

                Repeater {
                    id: expeRepeater
                    model: rootExperimentationsGroup.experimentationsGroup ? rootExperimentationsGroup.experimentationsGroup.experimentations : null

                    delegate: Rectangle {
                        id: rootExperimentation

                        property ExperimentationM experimentation: model.QtObject
                        property bool isMouseHovering: itemMouseArea.containsMouse || btnDelete.containsMouse || btnOpen.containsMouse

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

                            spacing: 10

                            Text {
                                text: rootExperimentation.experimentation ? rootExperimentation.experimentation.name : ""

                                width: 350
                                elide: Text.ElideRight
                                color: IngeScapeAssessmentsTheme.regularDarkBlueHeader
                                font {
                                    family: IngeScapeTheme.textFontFamily
                                    weight: Font.Bold
                                    pixelSize: 16
                                }
                            }

                            Text {
                                text: rootExperimentation.experimentation ? rootExperimentation.experimentation.creationDate.toLocaleString(Qt.locale(), "dd/MM/yyyy - hh:mm:ss") : ""

                                color: IngeScapeAssessmentsTheme.regularDarkBlueHeader
                                font {
                                    family: IngeScapeTheme.textFontFamily
                                    weight: Font.Medium
                                    pixelSize: 16
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

                                property bool containsMouse: __behavior.containsMouse

                                opacity: rootExperimentation.isMouseHovering ? 1 : 0
                                enabled: opacity > 0

                                width: 40
                                height: 30

                                style: IngeScapeAssessmentsSvgButtonStyle {
                                    releasedID: "delete-blue"
                                    disabledID: releasedID
                                }

                                onClicked: {
                                    deleteExperimentationPopup.experimentation = rootExperimentation.experimentation
                                    deleteExperimentationPopup.open()
                                }
                            }

                            Button {
                                id: btnOpen

                                anchors.verticalCenter: parent.verticalCenter

                                property bool containsMouse: __behavior.containsMouse

                                opacity: rootExperimentation.isMouseHovering ? 1 : 0
                                enabled: opacity > 0

                                width: 85
                                height: 30

                                style: IngeScapeAssessmentsButtonStyle {
                                    text: "OPEN"
                                }

                                onClicked: {
                                    rootItem.openExperimentation(rootExperimentation.experimentation)
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
                            visible: index < (expeRepeater.count - 1)
                        }
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

    Popup.DeleteConfirmationPopup {
        id: deleteExperimentationPopup

        property var experimentation: null

        showPopupTitle: false
        anchors.centerIn: parent

        text: experimentation ? qsTr("Are you sure you want to delete the experimentation %1 ?").arg(experimentation.name) : ""

        height: 160
        width: 470

        onValidated: {
            // Emit the signal "Delete Characteristic"
            rootItem.deleteExperimentation(rootExperimentation.experimentation, rootExperimentationsGroup.experimentationsGroup)
            deleteExperimentationPopup.close()
        }

        onCanceled: {
            deleteExperimentationPopup.close()
        }

    }
}
