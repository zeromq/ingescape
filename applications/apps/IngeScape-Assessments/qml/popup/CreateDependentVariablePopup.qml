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

    height: 721
    width: 674

    anchors.centerIn: parent

    title: "NEW DEPENDENT VARIABLE"


    //--------------------------------------------------------
    //
    //
    // Properties
    //
    //
    //--------------------------------------------------------

    property TasksController taskController: null;

    // Protocol model the new dependent variable will be in
    property TaskM protocolM: null


    //--------------------------------
    //
    //
    // Signals
    //
    //
    //--------------------------------


    //--------------------------------
    //
    //
    // Slots
    //
    //
    //--------------------------------

    onOpened: {
        comboBoxAgent.selectedIndex = -1;
        comboBoxOutput.selectedIndex = -1;
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
        txtDependentVariableName.text = "";
        txtDependentVariableDescription.text = "";
        comboBoxAgent.selectedIndex = -1;
        comboBoxOutput.selectedIndex = -1;

        // Close the popup
        rootPopup.close();
    }


    //--------------------------------
    //
    // Content
    //
    //--------------------------------

    // Name
    Item {
        id: itemName

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
            text: qsTr("Name:")

            height: 30

            verticalAlignment: Text.AlignVCenter

            color: IngeScapeAssessmentsTheme.regularDarkBlueHeader
            font {
                family: IngeScapeTheme.textFontFamily
                weight: Font.Medium
                pixelSize: 16
            }
        }

        TextField {
            id: txtDependentVariableName

            anchors {
                left: parent.left
                leftMargin: 112
                right: parent.right
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
                    pixelSize: 16
                    family: IngeScapeTheme.textFontFamily
                }
            }
        }
    }

    // Description
    Item {
        id: itemDescription

        anchors {
            top: itemName.bottom
            topMargin: 31
            left: parent.left
            leftMargin: 28
            right: parent.right
            rightMargin: 28
        }

        height: 115

        Text {
            height: 30

            text: qsTr("Description:")

            verticalAlignment: Text.AlignVCenter

            color: IngeScapeAssessmentsTheme.regularDarkBlueHeader
            font {
                family: IngeScapeTheme.textFontFamily
                weight: Font.Medium
                pixelSize: 16
            }
        }

        TextArea {
            id: txtDependentVariableDescription

            anchors {
                left: parent.left
                leftMargin: 112
                top: parent.top
                right: parent.right
            }
            height: 115

            text: ""
            wrapMode: Text.WordWrap

            style: IngeScapeAssessmentsTextAreaStyle {}
        }
    }

    // Agent
    Item {
        id: itemAgent

        anchors {
            top: itemDescription.bottom
            topMargin: 31
            left: parent.left
            leftMargin: 28
            right: parent.right
            rightMargin: 28
        }

        height: 30

        Text {
            height: 30

            text: qsTr("Agent:")

            verticalAlignment: Text.AlignVCenter

            color: IngeScapeAssessmentsTheme.regularDarkBlueHeader
            font {
                family: IngeScapeTheme.textFontFamily
                weight: Font.Medium
                pixelSize: 16
            }
        }

        I2ComboboxStringList {
            id: comboBoxAgent

            anchors {
                top: parent.top
                left: parent.left
                leftMargin: 112
            }

            model: rootPopup.protocolM ? rootPopup.protocolM.hashFromAgentNameToSimplifiedAgent.keys : []

            style: IngeScapeAssessmentsComboboxStyle {
                frameVisible: false
                listBackgroundColorIdle: IngeScapeTheme.veryLightGreyColor
            }
            scrollViewStyle: IngeScapeAssessmentsScrollViewStyle {
                scrollBarSize: 4
                verticalScrollbarMargin: 2
            }
        }
    }

    // Output
    Item {
        id: itemOutput

        anchors {
            top: itemAgent.bottom
            topMargin: 31
            left: parent.left
            leftMargin: 28
            right: parent.right
            rightMargin: 28
        }

        height: 30

        Text {
            height: 30

            text: qsTr("Output:")

            verticalAlignment: Text.AlignVCenter

            color: IngeScapeAssessmentsTheme.regularDarkBlueHeader
            font {
                family: IngeScapeTheme.textFontFamily
                weight: Font.Medium
                pixelSize: 16
            }
        }

        I2ComboboxStringList {
            id: comboBoxOutput

            anchors {
                top: parent.top
                left: parent.left
                leftMargin: 112
            }

            model: rootPopup.protocolM && comboBoxAgent.selectedItem
                   && rootPopup.protocolM.hashFromAgentNameToSimplifiedAgent.containsKey(comboBoxAgent.selectedItem)
                   ? rootPopup.protocolM.hashFromAgentNameToSimplifiedAgent.value(comboBoxAgent.selectedItem).outputNamesList
                   : []

            style: IngeScapeAssessmentsComboboxStyle {
                frameVisible: false
                listBackgroundColorIdle: IngeScapeTheme.veryLightGreyColor
            }
            scrollViewStyle: IngeScapeAssessmentsScrollViewStyle {
                scrollBarSize: 4
                verticalScrollbarMargin: 2
            }
        }
    }


    //
    // Buttons
    //
    Row {
        id: buttons

        anchors {
            right: parent.right
            rightMargin: 28
            bottom : parent.bottom
            bottomMargin: 28
        }
        spacing : 15

        // Cancel button
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

        // OK button
        Button {
            id: okButton

            property var boundingBox: IngeScapeTheme.svgFileIngeScape.boundsOnElement("button");

            anchors {
                verticalCenter: parent.verticalCenter
            }

            height: boundingBox.height
            width: boundingBox.width

            activeFocusOnPress: true

            enabled: if (rootPopup.taskController && (txtDependentVariableName.text.length > 0) && comboBoxAgent.selectedItem && comboBoxOutput.selectedItem)
                     {
                         rootPopup.taskController.canCreateDependentVariableWithName(txtDependentVariableName.text);
                     }
                     else {
                         false;
                     }

            style: IngeScapeAssessmentsButtonStyle {
                text: "OK"
            }

            onClicked: {
                if (rootPopup.taskController)
                {
                    console.log("QML: create a new Dependent Variable " + txtDependentVariableName.text + " on output " + comboBoxOutput.selectedItem + " of agent " + comboBoxAgent.selectedItem);

                    rootPopup.taskController.createNewDependentVariable(txtDependentVariableName.text,
                                                                        txtDependentVariableDescription.text,
                                                                        comboBoxAgent.selectedItem,
                                                                        comboBoxOutput.selectedItem);

                    // Reset all user inputs and close the popup
                    rootPopup.resetInputsAndClosePopup();
                }
            }
        }
    }
}