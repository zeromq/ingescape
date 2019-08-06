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

import "../theme" as Theme

AssessmentsPopupBase {
    id: rootPopup

    height: 395
    width: 674

    anchors.centerIn: parent

    title: "NEW EXPERIMENT"


    //--------------------------------------------------------
    //
    //
    // Properties
    //
    //
    //--------------------------------------------------------

    property ExperimentationsListController controller: null;

    property ExperimentationsGroupVM selectedExperimentationsGroup: null


    //--------------------------------
    //
    //
    // Slots
    //
    //
    //--------------------------------

    onOpened: {

        // By default, select the default group "Other"
        if (controller) {
            rootPopup.selectedExperimentationsGroup = controller.defaultGroupOther;
        }
    }


    //--------------------------------
    //
    //
    // Functions
    //
    //
    //--------------------------------

    //
    // Reset all user inputs and close the popup
    //
    function resetInputsAndClosePopup() {
        //console.log("QML: Reset all user inputs and close popup");

        // Reset all user inputs
        txtExperimentationName.text = "";
        txtNewExperimentationsGroupName.text = "";
        rootPopup.selectedExperimentationsGroup = null;

        // Close the popup
        rootPopup.close();
    }


    //--------------------------------
    //
    // Content
    //
    //--------------------------------

    Item {
        id: rowName

        anchors {
            top: parent.top
            topMargin: 34
            left: parent.left
            leftMargin: 28
            right: parent.right
            rightMargin: 28
        }

        height: 30

        Text {
            id: nameLabel

            anchors {
                top: parent.top
                bottom: parent.bottom
                left: parent.left
            }

            text: qsTr("Name :")

            verticalAlignment: Text.AlignVCenter

            color: IngeScapeAssessmentsTheme.regularDarkBlueHeader
            font {
                family: IngeScapeTheme.textFontFamily
                weight: Font.Medium
                pixelSize: 16
            }
        }

        TextField {
            id: txtExperimentationName

            anchors {
                top: parent.top
                bottom: parent.bottom
                left: parent.left
                leftMargin: 132
                right: parent.right
            }

            text: ""

            style: I2TextFieldStyle {
                backgroundColor: IngeScapeTheme.veryLightGreyColor
                borderColor: IngeScapeAssessmentsTheme.blueButton
                borderErrorColor: IngeScapeTheme.redColor
                radiusTextBox: 5
                borderWidth: 0;
                borderWidthActive: 2
                textIdleColor: IngeScapeAssessmentsTheme.regularDarkBlueHeader;
                textDisabledColor: IngeScapeAssessmentsTheme.lighterDarkBlueHeader

                padding.left: 16
                padding.right: 16

                font {
                    pixelSize: 16
                    family: IngeScapeTheme.textFontFamily
                }
            }
        }
    }

    Rectangle {
        anchors {
            top: rowName.bottom
            topMargin: 24
            left: parent.left
            leftMargin: 28
            right: parent.right
            rightMargin: 28
        }

        Text {
            id: txtGroupsTitle

            anchors {
                left: parent.left
                top: parent.top
            }
            width: 75
            height: 30

            text: qsTr("Group :")

            color: IngeScapeAssessmentsTheme.regularDarkBlueHeader
            font {
                family: IngeScapeTheme.textFontFamily
                weight: Font.Medium
                pixelSize: 16
            }
        }

        Column {
            anchors {
                top: parent.top
                left: parent.left
                leftMargin: 160
                right: parent.right
            }
            spacing: 4

            ExclusiveGroup {
                id: exclusiveExperimentationsGroup
            }

            Repeater {
                model: controller ? controller.allExperimentationsGroups : null

                delegate: RadioButton {
                    id: radioExperimentationsGroup

                    anchors {
                        left: parent.left
                        right: parent.right
                    }

                    height: 30

                    text: model.name

                    exclusiveGroup: exclusiveExperimentationsGroup

                    checked: (rootPopup.selectedExperimentationsGroup && (rootPopup.selectedExperimentationsGroup === model.QtObject))

                    style: Theme.IngeScapeRadioButtonStyle { }

                    onCheckedChanged: {
                        if (checked) {
                            console.log("Select experimentations group: " + model.name);

                            rootPopup.selectedExperimentationsGroup = model.QtObject;
                        }
                    }

                    Binding {
                        target: radioExperimentationsGroup
                        property: "checked"
                        value: (rootPopup.selectedExperimentationsGroup && (rootPopup.selectedExperimentationsGroup === model.QtObject))
                    }
                }
            }

            Item {

                anchors {
                    left: parent.left
                    right: parent.right
                }

                height: 30

                RadioButton {
                    id: radioNewExperimentationsGroup

                    anchors {
                        left: parent.left
                        top: parent.top
                        bottom: parent.bottom
                    }

                    text: (controller && controller.newGroup) ? controller.newGroup.name : ""

                    exclusiveGroup: exclusiveExperimentationsGroup

                    checked: (rootPopup.selectedExperimentationsGroup && (rootPopup.selectedExperimentationsGroup === controller.newGroup))

                    style: Theme.IngeScapeRadioButtonStyle { }

                    onCheckedChanged: {
                        if (checked) {
                            console.log("Select new experimentations group: ...");

                            rootPopup.selectedExperimentationsGroup = controller.newGroup;
                        }
                    }

                    Binding {
                        target: radioNewExperimentationsGroup
                        property: "checked"
                        value: (rootPopup.selectedExperimentationsGroup && (rootPopup.selectedExperimentationsGroup === controller.newGroup))
                    }
                }

                TextField {
                    id: txtNewExperimentationsGroupName

                    anchors {
                        left: radioNewExperimentationsGroup.right
                        leftMargin: 20
                        right: parent.right
                        verticalCenter: radioNewExperimentationsGroup.verticalCenter
                    }

                    height: 30

                    text: ""

                    style: I2TextFieldStyle {
                        backgroundColor: IngeScapeTheme.veryLightGreyColor
                        borderColor: IngeScapeAssessmentsTheme.blueButton
                        borderErrorColor: IngeScapeTheme.redColor
                        radiusTextBox: 5
                        borderWidth: 0;
                        borderWidthActive: 2
                        textIdleColor: IngeScapeAssessmentsTheme.regularDarkBlueHeader;
                        textDisabledColor: IngeScapeAssessmentsTheme.lighterDarkBlueHeader

                        padding.left: 16
                        padding.right: 16

                        font {
                            pixelSize:16
                            family: IngeScapeTheme.textFontFamily
                        }
                    }
                }
            }
        }
    }


    Row {
        anchors {
            right: parent.right
            rightMargin: 28
            bottom : parent.bottom
            bottomMargin: 28
        }
        spacing : 15

        Button {
            id: cancelButton

            property var boundingBox: IngeScapeTheme.svgFileIngeScape.boundsOnElement("button");

            anchors {
                verticalCenter: parent.verticalCenter
            }

            height: boundingBox.height
            width: boundingBox.width

            activeFocusOnPress: true

            style: ButtonStyle {
                background: Rectangle {
                    anchors.fill: parent
                    radius: 5
                    color: control.pressed ? IngeScapeTheme.lightGreyColor : (control.hovered ? IngeScapeTheme.veryLightGreyColor : "transparent")
                }

                label: Text {
                    text: "Cancel"
                    verticalAlignment: Text.AlignVCenter
                    horizontalAlignment: Text.AlignHCenter
                    color: IngeScapeAssessmentsTheme.regularDarkBlueHeader

                    font {
                        family: IngeScapeTheme.textFontFamily
                        weight: Font.Medium
                        pixelSize: 16
                    }
                }
            }

            onClicked: {
                // Reset all user inputs and close the popup
                rootPopup.resetInputsAndClosePopup();
            }
        }

        Button {
            id: okButton

            property var boundingBox: IngeScapeTheme.svgFileIngeScape.boundsOnElement("button");

            anchors {
                verticalCenter: parent.verticalCenter
            }

            height: boundingBox.height
            width: boundingBox.width

            activeFocusOnPress: true

            enabled: ((txtExperimentationName.text.length > 0) && controller && controller.newGroup
                      && ( (rootPopup.selectedExperimentationsGroup !== controller.newGroup)
                          || controller.canCreateExperimentationsGroupWithName(txtNewExperimentationsGroupName.text) ) )

            style: IngeScapeAssessmentsButtonStyle {
                text: "OK"
            }

            onClicked: {
                //console.log("QML: create new Experimentation " + txtExperimentationName.text + " in group " + rootPopup.selectedExperimentationsGroup.name);

                if (controller)
                {
                    // Selected group is the special one to create a new group
                    if (rootPopup.selectedExperimentationsGroup === controller.newGroup)
                    {
                        controller.createNewExperimentationInNewGroup(txtExperimentationName.text, txtNewExperimentationsGroupName.text);
                    }
                    // Selected group already exist
                    else
                    {
                        controller.createNewExperimentationInGroup(txtExperimentationName.text, rootPopup.selectedExperimentationsGroup);
                    }
                }

                // Reset all user inputs and close the popup
                rootPopup.resetInputsAndClosePopup();
            }
        }
    }


}
