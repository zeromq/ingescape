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

    property ProtocolsController protocolsController: null;

    // Protocol model the new dependent variable will be in
    property ProtocolM protocolM: null

    // Our popup is used to edit temporaly dependent variable of our protocols controller, after we can :
    // - create a new dependent variable
    // OR
    // - edit an existing dependent variable
    property DependentVariableM dependentVariableToEdit: null;


    //
    // Private properties
    //
    QtObject {
        id: rootPrivate

        property string originalName : ""
    }


    //--------------------------------
    //
    //
    // Signals
    //
    //
    //--------------------------------

    signal dependentVariableIsEdited();

    //--------------------------------
    //
    //
    // Slots
    //
    //
    //--------------------------------

    onOpened: {
        if (rootPopup.dependentVariableToEdit)
        {
            // Save name at the origin to know if it is already set
            rootPrivate.originalName = rootPopup.dependentVariableToEdit.name;

            // Update controls
            txtDependentVariableName.text = rootPopup.dependentVariableToEdit.name;
            txtDependentVariableDescription.text = rootPopup.dependentVariableToEdit.description;

            comboBoxAgent.selectedIndex = rootPopup.protocolM ? rootPopup.protocolM.hashFromAgentNameToSimplifiedAgent.keys.indexOf(rootPopup.dependentVariableToEdit.agentName)
                                                              : -1

            comboBoxOutput.selectedIndex = rootPopup.protocolM && comboBoxAgent.selectedItem && rootPopup.protocolM.hashFromAgentNameToSimplifiedAgent.containsKey(comboBoxAgent.selectedItem)
                                           ? rootPopup.protocolM.hashFromAgentNameToSimplifiedAgent.value(comboBoxAgent.selectedItem).outputNamesList.indexOf(rootPopup.dependentVariableToEdit.outputName)
                                           : -1
        }
    }

    onClosed: {
        // Reset all user inputs
        rootPopup.resetInputs();
    }


    //--------------------------------
    //
    //
    // Functions
    //
    //
    //--------------------------------

    //
    // Reset all user inputs
    //
    function resetInputs() {
        //console.log("QML: Reset all user inputs");

        // Reset all user inputs
        txtDependentVariableName.text = "";
        txtDependentVariableDescription.text = "";
        comboBoxAgent.selectedIndex = -1;
        comboBoxOutput.selectedIndex = -1;
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
                // Close the popup
                rootPopup.close();
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

            enabled: if ((txtDependentVariableName.text.length > 0) && comboBoxAgent.selectedItem && comboBoxOutput.selectedItem)
                     {
                         ((txtDependentVariableName.text === rootPrivate.originalName) // Same name that when we opened the popup : "edition" mode
                         || (rootPopup.protocolsController && rootPopup.protocolsController.canCreateDependentVariableWithName(txtDependentVariableName.text))); // No other dependent variable with the same name
                     }
                     else {
                         false;
                     }


            style: IngeScapeAssessmentsButtonStyle {
                text: "OK"
            }

            onClicked: {
                if (rootPopup.dependentVariableToEdit)
                {
                    console.log("QML: Edit a Dependent Variable " + txtDependentVariableName.text + " on output " + comboBoxOutput.selectedItem + " of agent " + comboBoxAgent.selectedItem);

                    rootPopup.dependentVariableToEdit.name = txtDependentVariableName.text;
                    rootPopup.dependentVariableToEdit.description = txtDependentVariableDescription.text;
                    rootPopup.dependentVariableToEdit.agentName = comboBoxAgent.selectedItem;
                    rootPopup.dependentVariableToEdit.outputName = comboBoxOutput.selectedItem;

                    // Emit the signal "Dependent variable is edited"
                    rootPopup.dependentVariableIsEdited();

                    // Close the popup
                    rootPopup.close();
                }
            }
        }
    }
}
