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

    height: 674
    width: 592

    anchors.centerIn: parent

    title: "NEW CHARACTERISTIC"


    //--------------------------------------------------------
    //
    //
    // Properties
    //
    //
    //--------------------------------------------------------

    property SubjectsController subjectController: null;

    property int selectedType: -1;

    property var enumTexts: [];



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
        txtCharacteristicName.text = "";
        rootPopup.selectedType = -1;
        spinBoxValuesNumber.value = 2;
        enumTexts = [];

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
            topMargin: 30
            left: parent.left
            leftMargin: 28
            right: parent.right
            rightMargin: 28
        }

        height: 30

        Text {
            id: nameLabel
            anchors {
                left: parent.left
                verticalCenter: parent.verticalCenter
            }

            height: 30

            text: qsTr("Name :")

            horizontalAlignment: Text.AlignRight
            verticalAlignment: Text.AlignVCenter

            color: IngeScapeAssessmentsTheme.regularDarkBlueHeader
            font {
                family: IngeScapeTheme.textFontFamily
                weight: Font.Medium
                pixelSize: 16
            }
        }

        TextField {
            id: txtCharacteristicName

            anchors {
                left: parent.left
                leftMargin: 140
                right: parent.right
                verticalCenter: parent.verticalCenter
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

    Item {
        anchors {
            top: rowName.bottom
            topMargin: 24
            left: parent.left
            leftMargin: 28
            right: parent.right
            rightMargin: 50
        }

        height: 370

        Text {
            id: txtTypesTitle
            anchors {
                top: parent.top
                left: parent.left
            }

            height: 30

            text: qsTr("Type :")

            horizontalAlignment: Text.AlignRight

            color: IngeScapeAssessmentsTheme.regularDarkBlueHeader
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
                left: parent.left
                leftMargin: 140
                right: parent.right
            }

            spacing: 4

            ExclusiveGroup {
                id: exclusiveGroupTypes
            }

            Repeater {
                model: rootPopup.subjectController ? rootPopup.subjectController.allCharacteristicValueTypes : null

                delegate: RadioButton {
                    id: radioCharacteristicValueType

                    text: model.name
                    height: 30

                    exclusiveGroup: exclusiveGroupTypes

                    checked: ((rootPopup.selectedType > -1) && (rootPopup.selectedType === model.value))

                    style: Theme.IngeScapeRadioButtonStyle { }

                    onCheckedChanged: {
                        if (checked) {
                            console.log("Select Characteristic Value Type: " + model.name + " (" + model.value + ")");

                            rootPopup.selectedType = model.value;
                        }
                    }

                    Binding {
                        target: radioCharacteristicValueType
                        property: "checked"
                        value: ((rootPopup.selectedType > -1) && (rootPopup.selectedType === model.value))
                    }
                }
            }

            Item {
                id: specialEnumItem
                anchors {
                    left: parent.left
                    right: parent.right
                }

                height: childrenRect.height

                RadioButton {
                    id: enumRadioButton

                    height: 30
                    text: "Enum"

                    exclusiveGroup: exclusiveGroupTypes

                    checked: ((rootPopup.selectedType > -1) && (rootPopup.selectedType === CharacteristicValueTypes.CHARACTERISTIC_ENUM))

                    style: Theme.IngeScapeRadioButtonStyle { }

                    onCheckedChanged: {
                        if (checked) {
                            console.log("Select Characteristic Value Type: Enum (" + CharacteristicValueTypes.CHARACTERISTIC_ENUM + ")");

                            rootPopup.selectedType = CharacteristicValueTypes.CHARACTERISTIC_ENUM;
                        }
                    }

                    Binding {
                        target: enumRadioButton
                        property: "checked"
                        value: ((rootPopup.selectedType > -1) && (rootPopup.selectedType === CharacteristicValueTypes.CHARACTERISTIC_ENUM))
                    }
                }

                Rectangle {
                    anchors {
                        top: enumRadioButton.top
                        left: enumRadioButton.right
                        leftMargin: 40
                        right: parent.right
                    }

                    height: 274
                    radius: 5

                    // Selected type is "Enum"
                    visible: (rootPopup.selectedType === CharacteristicValueTypes.CHARACTERISTIC_ENUM)

                    Rectangle {
                        id: enumValuesBackground

                        anchors {
                            fill: parent
                            topMargin: -10
                            bottomMargin: -15
                            leftMargin: -25
                            rightMargin: -22
                        }
                        radius: 5

                        color: IngeScapeTheme.veryLightGreyColor
                    }

                    Row {
                        id: headerNewEnum

                        anchors {
                            top: parent.top
                            left: parent.left
                        }

                        height: 30
                        spacing: 10

                        Text {
                            anchors {
                                top: parent.top
                                bottom: parent.bottom
                            }

                            text: "Number of values:"
                            verticalAlignment: Text.AlignVCenter

                            color: IngeScapeAssessmentsTheme.regularDarkBlueHeader
                            font {
                                family: IngeScapeTheme.textFontFamily
                                weight: Font.Medium
                                pixelSize: 16
                            }
                        }

                        SpinBox {
                            id: spinBoxValuesNumber

                            anchors {
                                top: parent.top
                                bottom: parent.bottom
                            }

                            width: 44

                            horizontalAlignment: Text.AlignHCenter

                            style: SpinBoxStyle {
                                textColor: control.enabled ? IngeScapeAssessmentsTheme.regularDarkBlueHeader : IngeScapeAssessmentsTheme.lighterDarkBlueHeader
                                background: Rectangle {
                                    radius: 5
                                    border {
                                        color: IngeScapeAssessmentsTheme.blueButton
                                        width: control.enabled && control.activeFocus ? 2 : 0
                                    }

                                    color: IngeScapeTheme.whiteColor
                                }
                            }

                            minimumValue: 2
                            value: 2

                        }
                    }

                    ScrollView {
                        id: enumValueScrollView

                        anchors {
                            top: headerNewEnum.bottom
                            topMargin: 20
                            left: parent.left
                            right: parent.right
                            rightMargin: -scrollBarSize - verticalScrollbarMargin
                            bottom: parent.bottom
                        }

                        property int scrollBarSize: 11
                        property int verticalScrollbarMargin: 3

                        style: IngeScapeAssessmentsScrollViewStyle {
                            scrollBarSize: enumValueScrollView.scrollBarSize
                            verticalScrollbarMargin: enumValueScrollView.verticalScrollbarMargin
                        }

                        // Prevent drag overshoot on Windows
                        flickableItem.boundsBehavior: Flickable.OvershootBounds

                        contentItem: Column {
                            width: enumValueScrollView.width - enumValueScrollView.scrollBarSize - enumValueScrollView.verticalScrollbarMargin
                            height: childrenRect.height
                            spacing: 12

                            Repeater {
                                model: spinBoxValuesNumber.value

                                delegate: TextField {
                                    id: enumText

                                    anchors {
                                        left: parent.left
                                        right: parent.right
                                    }

                                    height: 30

                                    text: rootPopup.enumTexts[index] ? rootPopup.enumTexts[index] : ""

                                    style: I2TextFieldStyle {
                                        backgroundColor: IngeScapeTheme.whiteColor
                                        borderColor: IngeScapeAssessmentsTheme.blueButton
                                        borderErrorColor: IngeScapeTheme.redColor
                                        radiusTextBox: 5
                                        borderWidth: 0;
                                        borderWidthActive: 2
                                        textIdleColor: IngeScapeAssessmentsTheme.regularDarkBlueHeader;
                                        textDisabledColor: IngeScapeAssessmentsTheme.lighterDarkBlueHeader

                                        placeholderCustomText: qsTr("Name of the value %1").arg(index + 1)
                                        placeholderMarginLeft: 15
                                        placeholderColor: IngeScapeTheme.lightGreyColor
                                        placeholderFont {
                                            pixelSize: 16
                                            family: IngeScapeTheme.textFontFamily
                                            italic: true
                                        }

                                        padding.left: 15
                                        padding.right: 15

                                        font {
                                            pixelSize: 16
                                            family: IngeScapeTheme.textFontFamily
                                        }
                                    }

                                    Component.onCompleted: {
                                        // If this index is not defined, initialize it with empty string
                                        if (typeof rootPopup.enumTexts[index] === 'undefined') {
                                            rootPopup.enumTexts[index] = "";
                                        }
                                    }

                                    onTextChanged: {
                                        // Update the strings array for this index
                                        rootPopup.enumTexts[index] = enumText.text;
                                    }
                                }
                            }
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

            enabled: ( (txtCharacteristicName.text.length > 0) && (rootPopup.selectedType > -1)
                      && subjectController && subjectController.canCreateCharacteristicWithName(txtCharacteristicName.text) )

            style: IngeScapeAssessmentsButtonStyle {
                text: "OK"
            }

            onClicked: {
                console.log("QML: create new Characteristic " + txtCharacteristicName.text + " of type " + rootPopup.selectedType);

                if (subjectController)
                {
                    // Selected type is ENUM
                    if (rootPopup.selectedType === CharacteristicValueTypes.CHARACTERISTIC_ENUM)
                    {
                        // Use only the N first elements of the array (the array may be longer than the number of displayed TextFields
                        // if the user decreases the value of the spin box after edition the last TextField)
                        // Where N = spinBoxValuesNumber.value (the value of the spin box)
                        var displayedEnumTexts = rootPopup.enumTexts.slice(0, spinBoxValuesNumber.value);

                        // Check for duplicates
                        let singles = []
                        displayedEnumTexts.forEach(function(element, index) {
                            // If the element is already in 'singles', it's a duplicate
                            if (singles.indexOf(element) > -1) {
                                //FIXME Show appropriate error message
                            }

                            // Add the element to 'singles'
                            singles.push(element);
                        })

                        var index = 0;
                        var isEmptyValue = false;

                        displayedEnumTexts.forEach(function(element) {
                            if (element === "") {
                                isEmptyValue = true;
                                console.log("value at " + index + " is empty, edit it !");
                            }
                            index++;
                        });

                        console.log("QML: Enum with " + spinBoxValuesNumber.value + " strings: " + displayedEnumTexts);

                        if (isEmptyValue === false)
                        {
                            rootPopup.subjectController.createNewCharacteristicEnum(txtCharacteristicName.text, displayedEnumTexts);

                            // Reset all user inputs and close the popup
                            rootPopup.resetInputsAndClosePopup();
                        }
                        else
                        {
                            console.warn("Some values of the enum are empty, edit them !");
                        }
                    }
                    // Selected type is NOT ENUM
                    else
                    {
                        rootPopup.subjectController.createNewCharacteristic(txtCharacteristicName.text, rootPopup.selectedType);

                        // Reset all user inputs and close the popup
                        rootPopup.resetInputsAndClosePopup();
                    }
                }
            }
        }
    }

}
