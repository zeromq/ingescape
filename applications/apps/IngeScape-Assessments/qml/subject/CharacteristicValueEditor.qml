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


Item {
    id: rootItem

    //anchors.fill: parent

    width: parent.width
    height: 30


    //--------------------------------------------------------
    //
    //
    // Properties
    //
    //
    //--------------------------------------------------------

    // Model of our characteristic
    property CharacteristicM characteristic: null;

    // Keep type "var" because the C++ use a QVariant
    property var characteristicValue: "";

    // FLag indicating if our characteristic is selected
    property bool isSelected: false;

    // FLag indicating if the user is currently editing our characteristic
    property bool isCurrentlyEditing: false;

    // FLag indicating if our cell is in the first column
    property bool isFirstColumn: (styleData.column === 0)



    //--------------------------------
    //
    //
    // Signals
    //
    //
    //--------------------------------

    // Signal emitted when the user clicks on the toggle button to edit the subject
    signal editSubject();

    // Signal emitted when the user clicks on the toggle button to stop the edition of the subject
    signal stopEditionOfSubject();

    // Characteristic Value Updated
    signal characteristicValueUpdated(var value);

    // Delete Subject
    signal deleteSubject();


    //--------------------------------
    //
    //
    // Slots
    //
    //
    //--------------------------------

    /*onCharacteristicChanged: {
        console.log("QML: on Characteristic changed " + characteristic.name);
    }*/

    /*onCharacteristicValueChanged: {
        console.log("QML: on Characteristic Value changed " + characteristicValue);
    }*/


    //--------------------------------------------------------
    //
    //
    // Content
    //
    //
    //--------------------------------------------------------

    Loader {
        id: loaderOptions

        sourceComponent: rootItem.isFirstColumn ? componentOptions : null
    }

    Rectangle {
        id: background

        anchors {
            fill: parent
            leftMargin: rootItem.isFirstColumn ? 105 : 0
        }

        color: "transparent"
        /*border {
            color: "silver"
            width: 1
        }*/

        Rectangle {
            id: leftSeparator

            visible: rootItem.isFirstColumn

            anchors {
                left: parent.left
                top: parent.top
                bottom: parent.bottom
            }
            width: 1
            color: "silver"
        }

        Text {
            id: txtValue

            anchors {
                fill: parent
                margins: 5
            }

            visible: !rootItem.isCurrentlyEditing

            text: (typeof rootItem.characteristicValue !== 'undefined') ? rootItem.characteristicValue : ""

            verticalAlignment: Text.AlignVCenter
            //color: styleData.selected ? IngeScapeTheme.whiteColor : IngeScapeTheme.blackColor
            font {
                family: IngeScapeTheme.textFontFamily
                //weight: Font.Medium
                pixelSize: 12
            }
        }

        Loader {
            id: loaderEditor

            anchors {
                fill: parent
                margins: 1
            }

            visible: rootItem.isCurrentlyEditing

            // Load editor in function of the value type:
            // - Enum --> combobox
            // - NOT enum --> text field
            sourceComponent: (rootItem.characteristic && (rootItem.characteristic.valueType === CharacteristicValueTypes.CHARACTERISTIC_ENUM)) ? componentComboboxEditor
                                                                                                                                               : componentTextFieldEditor
        }

        Rectangle {
            id: rightSeparator

            anchors {
                right: parent.right
                top: parent.top
                bottom: parent.bottom
            }
            width: 1
            color: "silver"
        }
    }


    //
    // component Options
    //
    Component {
        id: componentOptions

        Row {
            id: rowOptions

            anchors {
                left: parent.left
                top: parent.top
                bottom: parent.bottom
            }

            spacing: 0

            visible: rootItem.isSelected

            Button {
                id: btnDelete

                anchors {
                    top: parent.top
                }
                width: 50
                height: 30

                text: "DEL"

                onClicked: {
                    // Emit the signal "Delete Subject"
                    rootItem.deleteSubject();
                }
            }

            Button {
                id: btnEdit

                anchors {
                    top: parent.top
                }
                width: 50
                height: 30

                checkable: true

                checked: rootItem.isCurrentlyEditing

                text: checked ? "SAVE" : "EDIT"

                onClicked: {

                    if (checked) {
                        // Emit the signal "Edit Subject"
                        rootItem.editSubject();
                    }
                    else {
                        // Emit the signal "Stop Edition of Subject"
                        rootItem.stopEditionOfSubject();
                    }
                }

                Binding {
                    target: btnEdit
                    property: "checked"
                    value: rootItem.isCurrentlyEditing
                }
            }

        }
    }


    //
    // component Combobox Editor
    //
    Component {
        id: componentComboboxEditor

        I2ComboboxStringList {
            id: comboboxEditor

            model: rootItem.characteristic ? rootItem.characteristic.enumValues : null

            onSelectedItemChanged: {

                if (comboboxEditor.selectedItem)
                {
                    //console.log("QML: on Selected Item Changed " + comboboxEditor.selectedItem);

                    // Emit the signal "Characteristic Value Updated"
                    rootItem.characteristicValueUpdated(comboboxEditor.selectedItem);
                }
            }

            /*Component.onCompleted: {

                //console.log("onCompleted: selectedIndex=" + comboboxEditor.selectedIndex + " -- characteristicValue=" + rootItem.characteristicValue);

                if ((comboboxEditor.selectedIndex < 0) && (typeof rootItem.characteristicValue !== 'undefined'))
                {
                    var index = comboboxEditor.model.indexOf(rootItem.characteristicValue);
                    if (index > -1) {
                        comboboxEditor.selectedIndex = index;
                    }
                }
            }*/

            onVisibleChanged: {

                //console.log("onVisibleChanged: selectedIndex=" + comboboxEditor.selectedIndex + " -- characteristicValue=" + rootItem.characteristicValue);

                if (visible && (comboboxEditor.selectedIndex < 0) && (typeof rootItem.characteristicValue !== 'undefined'))
                {
                    var index = comboboxEditor.model.indexOf(rootItem.characteristicValue);
                    if (index > -1) {
                        comboboxEditor.selectedIndex = index;
                    }
                }
            }
        }
    }


    //
    // component TextField Editor
    //
    Component {
        id: componentTextFieldEditor

        TextField {
            id: textFieldEditor

            property var intValidator: IntValidator {}
            property var doubleValidator: DoubleValidator {}

            text: (typeof rootItem.characteristicValue !== 'undefined') ? rootItem.characteristicValue : ""

            //inputMethodHints: Qt.ImhFormattedNumbersOnly

            validator: if (rootItem.characteristic)
                       {
                           if (rootItem.characteristic.valueType === CharacteristicValueTypes.INTEGER) {
                               return textFieldEditor.intValidator;
                           }
                           else if (rootItem.characteristic.valueType === CharacteristicValueTypes.DOUBLE) {
                               return textFieldEditor.doubleValidator;
                           }
                           else {
                               return null;
                           }
                       }
                       else {
                           return null;
                       }


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

            onTextChanged: {
                //console.log("QML: on Text Changed " + textFieldEditor.text);

                // Emit the signal "Characteristic Value Updated"
                rootItem.characteristicValueUpdated(textFieldEditor.text);
            }

            /*Component.onCompleted: {
                console.log("onCompleted: text=" + textFieldEditor.text + " -- characteristicValue=" + rootItem.characteristicValue);
            }*/
        }
    }
}
