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

    property IndependentVariableM variable: null;

    property var variableValue: "";

    property bool isSelected: false;

    property bool isCurrentlyEditing: false;



    //--------------------------------
    //
    //
    // Signals
    //
    //
    //--------------------------------

    // Signal emitted when the user clicks on the toggle button to edit the variable
    signal editVariable();

    // Signal emitted when the user clicks on the toggle button to stop the edition of the variable
    signal stopEditionOfVariable();

    // Independent Variable Value Updated
    signal independentVariableValueUpdated(var value);


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

    /*onIsCurrentlyEditingChanged: {
        if (isCurrentlyEditing === false) {
            console.log("QML: is Currently Editing from true to false " + characteristicValue);
        }
    }*/


    //--------------------------------------------------------
    //
    //
    // Content
    //
    //
    //--------------------------------------------------------

    Rectangle {
        id: background

        anchors {
            left: parent.left
            right: parent.right
            top: parent.top
            bottom: parent.bottom
        }

        color: "transparent"
        border {
            color: "silver"
            width: 1
        }

        Row {

            Text {
                id: txtName

                width: 100

                text: rootItem.variable ? rootItem.variable.name + ":" : ""
            }

            Text {
                anchors {
                    fill: parent
                    margins: 1
                }

                visible: !rootItem.isCurrentlyEditing

                text: (typeof rootItem.characteristicValue !== 'undefined') ? rootItem.characteristicValue : ""
            }

            TextField {
                id: txtEditor

                property var intValidator: IntValidator {}
                property var doubleValidator: DoubleValidator {}

                anchors {
                    fill: parent
                    margins: 1
                }

                visible: rootItem.isCurrentlyEditing && rootItem.characteristic && (rootItem.characteristic.valueType !== CharacteristicValueTypes.CHARACTERISTIC_ENUM)

                text: (typeof rootItem.characteristicValue !== 'undefined') ? rootItem.characteristicValue : ""

                //inputMethodHints: Qt.ImhFormattedNumbersOnly

                validator: if (rootItem.characteristic)
                           {
                               if (rootItem.characteristic.valueType === CharacteristicValueTypes.INTEGER) {
                                   return txtEditor.intValidator;
                               }
                               else if (rootItem.characteristic.valueType === CharacteristicValueTypes.DOUBLE) {
                                   return txtEditor.doubleValidator;
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
                    //console.log("QML: on Text Changed " + txtEditor.text);

                    // Emit the signal "Characteristic Value Updated"
                    rootItem.characteristicValueUpdated(txtEditor.text);
                }
            }

            // FIXME: use a Loader instead of visible
            I2ComboboxStringList {
                id: cmbEditor

                anchors {
                    fill: parent
                    margins: 1
                }

                visible: rootItem.isCurrentlyEditing && rootItem.characteristic && (rootItem.characteristic.valueType === CharacteristicValueTypes.CHARACTERISTIC_ENUM)

                model: rootItem.characteristic ? rootItem.characteristic.enumValues : null

                onSelectedItemChanged: {

                    if (cmbEditor.selectedItem)
                    {
                        //console.log("QML: on Selected Item Changed " + cmbEditor.selectedItem);

                        // Emit the signal "Characteristic Value Updated"
                        rootItem.characteristicValueUpdated(cmbEditor.selectedItem);
                    }
                }
            }

        }
    }


    /*Button {
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
    }*/
}
