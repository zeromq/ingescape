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

import QtQuick 2.9
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4

import I2Quick 1.0

import INGESCAPE 1.0


I2PopupBase {
    id: rootPopup

    height: 500
    width: 500

    anchors.centerIn: parent

    isModal: true
    dismissOnOutsideTap: false
    keepRelativePositionToInitialParent: false


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
    // Signals
    //
    //
    //--------------------------------

    //
    //signal cancelTODO();


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

    Rectangle {

        anchors {
            fill: parent
        }
        radius: 5
        border {
            width: 2
            color: IngeScapeTheme.editorsBackgroundBorderColor
        }
        color: IngeScapeTheme.editorsBackgroundColor


        Text {
            id: title

            anchors {
                left: parent.left
                top: parent.top
                leftMargin: 20
                topMargin: 20
            }
            height: 25

            //horizontalAlignment: Text.AlignHCenter

            text: qsTr("New experimentation:")

            color: IngeScapeTheme.whiteColor
            font {
                family: IngeScapeTheme.textFontFamily
                pixelSize: 16
            }
        }

        Row {
            id: rowName

            anchors {
                top: title.bottom
                topMargin: 50
                left: parent.left
                leftMargin: 10
            }

            spacing: 10

            Text {
                text: qsTr("Name:")
                height: 30

                color: IngeScapeTheme.whiteColor
                font {
                    family: IngeScapeTheme.textFontFamily
                    weight : Font.Medium
                    pixelSize : 16
                }
            }

            TextField {
                id: txtExperimentationName

                height: 30
                width: 250 // parent.width

                //verticalAlignment: TextInput.AlignVCenter
                text: ""

                style: I2TextFieldStyle {
                    backgroundColor: IngeScapeTheme.darkBlueGreyColor
                    borderColor: IngeScapeTheme.whiteColor
                    borderErrorColor: IngeScapeTheme.redColor
                    radiusTextBox: 1
                    borderWidth: 0;
                    borderWidthActive: 1
                    textIdleColor: IngeScapeTheme.whiteColor;
                    textDisabledColor: IngeScapeTheme.darkGreyColor

                    padding.left: 3
                    padding.right: 3

                    font {
                        pixelSize:15
                        family: IngeScapeTheme.textFontFamily
                    }
                }

                //Binding {
                //    target: txtLogFilePath
                //    property: "text"
                //    value: rootItem.agent.logFilePath
                //}

                /*onTextChanged: {
                    console.log("onTextChanged " + txtExperimentationName.text);
                }*/
            }
        }

        Rectangle {
            anchors {
                left: parent.left
                leftMargin: 10
                top: rowName.bottom
                topMargin: 20
            }

            Text {
                id: txtGroupsTitle

                anchors {
                    left: parent.left
                    top: parent.top
                }

                text: qsTr("Groups:")
                color: "white"
            }

            Column {
                anchors {
                    top: parent.top
                    left: txtGroupsTitle.right
                    leftMargin: 10
                    right: parent.right
                }
                spacing: 10

                ExclusiveGroup {
                    id: exclusiveExperimentationsGroup
                }

                Repeater {
                    model: controller ? controller.allExperimentationsGroups : null

                    delegate: RadioButton {
                        id: radioExperimentationsGroup

                        text: model.name

                        exclusiveGroup: exclusiveExperimentationsGroup

                        checked: (rootPopup.selectedExperimentationsGroup && (rootPopup.selectedExperimentationsGroup === model.QtObject))

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

                Row {

                    RadioButton {
                        id: radioNewExperimentationsGroup

                        text: (controller && controller.newGroup) ? controller.newGroup.name
                                                                  : ""

                        exclusiveGroup: exclusiveExperimentationsGroup

                        checked: (rootPopup.selectedExperimentationsGroup && (rootPopup.selectedExperimentationsGroup === controller.newGroup))

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

                        height: 30
                        width: 250 // parent.width

                        //verticalAlignment: TextInput.AlignVCenter
                        text: ""

                        style: I2TextFieldStyle {
                            backgroundColor: IngeScapeTheme.darkBlueGreyColor
                            borderColor: IngeScapeTheme.whiteColor
                            borderErrorColor: IngeScapeTheme.redColor
                            radiusTextBox: 1
                            borderWidth: 0;
                            borderWidthActive: 1
                            textIdleColor: IngeScapeTheme.whiteColor;
                            textDisabledColor: IngeScapeTheme.darkGreyColor

                            padding.left: 3
                            padding.right: 3

                            font {
                                pixelSize:15
                                family: IngeScapeTheme.textFontFamily
                            }
                        }

                        //Binding {
                        //    target: txtLogFilePath
                        //    property: "text"
                        //    value: rootItem.agent.logFilePath
                        //}

                        /*onTextChanged: {
                            console.log("onTextChanged " + txtExperimentationName.text);
                        }*/
                    }
                }
            }
        }


        Row {
            anchors {
                horizontalCenter: parent.horizontalCenter
                bottom : parent.bottom
                bottomMargin: 16
            }
            spacing : 15

            Button {
                id: cancelButton

                property var boundingBox: IngeScapeTheme.svgFileINGESCAPE.boundsOnElement("button");

                anchors {
                    verticalCenter: parent.verticalCenter
                }

                height: boundingBox.height
                width: boundingBox.width

                activeFocusOnPress: true
                text: "Cancel"

                style: I2SvgButtonStyle {
                    fileCache: IngeScapeTheme.svgFileINGESCAPE

                    pressedID: releasedID + "-pressed"
                    releasedID: "button"
                    disabledID: releasedID + "-disabled"

                    font {
                        family: IngeScapeTheme.textFontFamily
                        weight : Font.Medium
                        pixelSize : 16
                    }
                    labelColorPressed: IngeScapeTheme.blackColor
                    labelColorReleased: IngeScapeTheme.whiteColor
                    labelColorDisabled: IngeScapeTheme.whiteColor

                }

                onClicked: {
                    //console.log("QML: cancel");

                    // Reset all user inputs and close the popup
                    rootPopup.resetInputsAndClosePopup();
                }
            }

            Button {
                id: okButton

                property var boundingBox: IngeScapeTheme.svgFileINGESCAPE.boundsOnElement("button");

                anchors {
                    verticalCenter: parent.verticalCenter
                }

                height: boundingBox.height
                width: boundingBox.width

                activeFocusOnPress: true
                text: "OK"

                enabled: ((txtExperimentationName.text.length > 0) && controller && controller.newGroup
                          && ( (rootPopup.selectedExperimentationsGroup !== controller.newGroup)
                               || controller.canCreateExperimentationsGroupWithName(txtNewExperimentationsGroupName.text) ) )

                style: I2SvgButtonStyle {
                    fileCache: IngeScapeTheme.svgFileINGESCAPE

                    pressedID: releasedID + "-pressed"
                    releasedID: "button"
                    disabledID: releasedID + "-disabled"

                    font {
                        family: IngeScapeTheme.textFontFamily
                        weight : Font.Medium
                        pixelSize : 16
                    }
                    labelColorPressed: IngeScapeTheme.blackColor
                    labelColorReleased: IngeScapeTheme.whiteColor
                    labelColorDisabled: IngeScapeTheme.greyColor

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

}
