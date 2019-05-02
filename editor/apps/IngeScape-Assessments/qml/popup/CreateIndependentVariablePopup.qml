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

I2PopupBase {
    id: rootPopup

    height: 600
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

    property TasksController controller: null;

    // property IndependentVariableValueTypes selectedType
    property int selectedType: -1;

    property var enumTexts: [];

    // Our popup is used:
    // - to create a new independent variable
    // OR
    // - to edit an existing independent variable...in this case, this property must be set
    property IndependentVariableM independentVariableCurrentlyEdited: null;


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
        if (rootPopup.independentVariableCurrentlyEdited)
        {
            // Update controls
            txtIndependentVariableName.text = rootPopup.independentVariableCurrentlyEdited.name;
            txtIndependentVariableDescription.text = rootPopup.independentVariableCurrentlyEdited.description;
            spinBoxValuesNumber.value = rootPopup.independentVariableCurrentlyEdited.enumValues.length;

            rootPopup.selectedType = rootPopup.independentVariableCurrentlyEdited.valueType;
            rootPopup.enumTexts = rootPopup.independentVariableCurrentlyEdited.enumValues;
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
        txtIndependentVariableName.text = "";
        txtIndependentVariableDescription.text = "";
        spinBoxValuesNumber.value = 2;

        // Reset properties
        rootPopup.selectedType = -1;
        rootPopup.enumTexts = [];
        rootPopup.independentVariableCurrentlyEdited = null;

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
                right: parent.right
                top: parent.top
                topMargin: 20
            }

            horizontalAlignment: Text.AlignHCenter

            text: qsTr("New independent variable")

            color: IngeScapeTheme.whiteColor
            font {
                family: IngeScapeTheme.textFontFamily
                weight: Font.Medium
                pixelSize: 20
            }
        }

        Row {
            id: rowName

            anchors {
                top: title.bottom
                topMargin: 30
                left: parent.left
                leftMargin: 10
            }

            spacing: 10

            Text {
                width: 125
                height: 30

                text: qsTr("Name:")

                horizontalAlignment: Text.AlignRight
                verticalAlignment: Text.AlignVCenter

                color: IngeScapeTheme.whiteColor
                font {
                    family: IngeScapeTheme.textFontFamily
                    weight: Font.Medium
                    pixelSize: 16
                }
            }

            TextField {
                id: txtIndependentVariableName

                height: 30
                width: 250

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
            }
        }

        Row {
            id: rowDescription

            anchors {
                top: rowName.bottom
                topMargin: 20
                left: parent.left
                leftMargin: 10
            }

            spacing: 10

            Text {
                width: 125
                height: 30

                text: qsTr("Description:")

                horizontalAlignment: Text.AlignRight
                verticalAlignment: Text.AlignVCenter

                color: IngeScapeTheme.whiteColor
                font {
                    family: IngeScapeTheme.textFontFamily
                    weight: Font.Medium
                    pixelSize: 16
                }
            }

            TextField {
                id: txtIndependentVariableDescription

                height: 30
                width: 250

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
            }
        }

        Item {
            anchors {
                top: rowDescription.bottom
                topMargin: 20
                left: parent.left
                leftMargin: 10
                right: parent.right
                rightMargin: 10
            }

            Text {
                id: txtTypesTitle

                anchors {
                    left: parent.left
                    top: parent.top
                }
                width: 125
                height: 30

                text: qsTr("Type:")

                horizontalAlignment: Text.AlignRight

                color: IngeScapeTheme.whiteColor
                font {
                    family: IngeScapeTheme.textFontFamily
                    weight: Font.Medium
                    pixelSize: 16
                }
            }

            Column {
                id: columnTypes

                anchors {
                    top: parent.top
                    left: txtTypesTitle.right
                    leftMargin: 10
                }

                spacing: 10

                ExclusiveGroup {
                    id: exclusiveGroupTypes
                }

                Repeater {
                    model: controller ? controller.allIndependentVariableValueTypes : null

                    delegate: RadioButton {
                        id: radioIndependentVariableValueType

                        text: model.name

                        exclusiveGroup: exclusiveGroupTypes

                        checked: ((rootPopup.selectedType > -1) && (rootPopup.selectedType === model.value))

                        style: Theme.IngeScapeRadioButtonStyle { }

                        onCheckedChanged: {
                            if (checked) {
                                console.log("Select IndependentVariable Value Type: " + model.name + " (" + model.value + ")");

                                rootPopup.selectedType = model.value;
                            }
                        }

                        Binding {
                            target: radioIndependentVariableValueType
                            property: "checked"
                            value: ((rootPopup.selectedType > -1) && (rootPopup.selectedType === model.value))
                        }
                    }
                }
            }


            // FIXME TODO
            /*Loader {
                id: loaderEnum
            }*/

            Rectangle {
                anchors {
                    //top: parent.top
                    left: columnTypes.right
                    leftMargin: 10
                    right: parent.right
                    rightMargin: 10
                    top: columnTypes.bottom
                    topMargin: -20
                }
                height: 200

                // Selected type is "Enum"
                visible: (rootPopup.selectedType === IndependentVariableValueTypes.INDEPENDENT_VARIABLE_ENUM)

                color: "transparent"
                border {
                    color: "white"
                    width: 1
                }

                Row {
                    id: headerNewEnum

                    anchors {
                        top: parent.top
                        topMargin: 10
                        left: parent.left
                        leftMargin: 10
                    }

                    spacing: 10

                    Text {
                        text: "Number of values:"

                        color: IngeScapeTheme.whiteColor
                        font {
                            family: IngeScapeTheme.textFontFamily
                            weight: Font.Medium
                            pixelSize: 14
                        }
                    }

                    SpinBox {
                        id: spinBoxValuesNumber

                        minimumValue: 2
                        value: 2
                    }
                }

                Column {
                    anchors {
                        top: headerNewEnum.bottom
                        topMargin: 10
                        left: parent.left
                        leftMargin: 10
                        right: parent.right
                        rightMargin: 10
                    }

                    Repeater {
                        model: spinBoxValuesNumber.value

                        delegate: TextField {
                            id: enumText

                            anchors {
                                left: parent.left
                                right: parent.right
                            }

                            text: rootPopup.enumTexts[index] ? rootPopup.enumTexts[index] : ""

                            Component.onCompleted: {
                                // If this index is not defined, initialize it with empty string
                                if (typeof rootPopup.enumTexts[index] === 'undefined') {
                                    rootPopup.enumTexts[index] = "";
                                }
                            }

                            onTextChanged: {
                                //console.log(index + ": text changed to " + enumText.text);

                                // Update the strings array for this index
                                rootPopup.enumTexts[index] = enumText.text;
                            }
                        }
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
                        weight: Font.Medium
                        pixelSize: 16
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

                enabled: if (rootPopup.controller && (txtIndependentVariableName.text.length > 0) && (rootPopup.selectedType > -1))
                         {
                             // Edit an existing independent variable
                             if (rootPopup.independentVariableCurrentlyEdited)
                             {
                                 rootPopup.controller.canEditIndependentVariableWithName(rootPopup.independentVariableCurrentlyEdited, txtIndependentVariableName.text);
                             }
                             // Create a new independent variable
                             else
                             {
                                 rootPopup.controller.canCreateIndependentVariableWithName(txtIndependentVariableName.text);
                             }
                         }
                         else {
                             false;
                         }

                style: I2SvgButtonStyle {
                    fileCache: IngeScapeTheme.svgFileINGESCAPE

                    pressedID: releasedID + "-pressed"
                    releasedID: "button"
                    disabledID: releasedID + "-disabled"

                    font {
                        family: IngeScapeTheme.textFontFamily
                        weight: Font.Medium
                        pixelSize: 16
                    }
                    labelColorPressed: IngeScapeTheme.blackColor
                    labelColorReleased: IngeScapeTheme.whiteColor
                    labelColorDisabled: IngeScapeTheme.greyColor

                }

                onClicked: {

                    if (rootPopup.controller)
                    {
                        // Selected type is ENUM
                        if (rootPopup.selectedType === IndependentVariableValueTypes.INDEPENDENT_VARIABLE_ENUM)
                        {
                            // Use only the N first elements of the array (the array may be longer than the number of displayed TextFields
                            // if the user decreases the value of the spin box after edition the last TextField)
                            // Where N = spinBoxValuesNumber.value (the value of the spin box)
                            var displayedEnumTexts = rootPopup.enumTexts.slice(0, spinBoxValuesNumber.value);

                            var isEmptyValue = false;
                            var index = 0;

                            displayedEnumTexts.forEach(function(element) {
                                if (element === "") {
                                    isEmptyValue = true;
                                    console.log("value at " + index + " is empty, edit it !");
                                }
                                index++;
                            });

                            console.log("QML: Enum with " + spinBoxValuesNumber.value + " strings: " + displayedEnumTexts);

                            if (isEmptyValue === true)
                            {
                                console.warn("Some values of the enum are empty, edit them !");

                                // FIXME TODO: display warning message
                            }
                            else
                            {
                                // Edit an existing independent variable
                                if (rootPopup.independentVariableCurrentlyEdited)
                                {
                                    //console.log("QML: edit an existing Independent Variable " + txtIndependentVariableName.text + " of type " + rootPopup.selectedType);

                                    rootPopup.controller.saveModificationsOfIndependentVariableEnum(rootPopup.independentVariableCurrentlyEdited,
                                                                                                    txtIndependentVariableName.text,
                                                                                                    txtIndependentVariableDescription.text,
                                                                                                    displayedEnumTexts);
                                }
                                // Create a new independent variable
                                else
                                {
                                    //console.log("QML: create a new Independent Variable " + txtIndependentVariableName.text + " of type " + rootPopup.selectedType);

                                    rootPopup.controller.createNewIndependentVariableEnum(txtIndependentVariableName.text,
                                                                                          txtIndependentVariableDescription.text,
                                                                                          displayedEnumTexts);
                                }

                                // Reset all user inputs and close the popup
                                rootPopup.resetInputsAndClosePopup();
                            }
                        }
                        // Selected type is NOT ENUM
                        else
                        {
                            // Edit an existing independent variable
                            if (rootPopup.independentVariableCurrentlyEdited)
                            {
                                //console.log("QML: edit an existing Independent Variable " + txtIndependentVariableName.text + " of type " + rootPopup.selectedType);

                                rootPopup.controller.saveModificationsOfIndependentVariable(rootPopup.independentVariableCurrentlyEdited,
                                                                                            txtIndependentVariableName.text,
                                                                                            txtIndependentVariableDescription.text,
                                                                                            rootPopup.selectedType);
                            }
                            // Create a new independent variable
                            else
                            {
                                //console.log("QML: create a new Independent Variable " + txtIndependentVariableName.text + " of type " + rootPopup.selectedType);

                                rootPopup.controller.createNewIndependentVariable(txtIndependentVariableName.text,
                                                                                  txtIndependentVariableDescription.text,
                                                                                  rootPopup.selectedType);
                            }

                            // Reset all user inputs and close the popup
                            rootPopup.resetInputsAndClosePopup();
                        }
                    }
                }
            }
        }

    }

}
