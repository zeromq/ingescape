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

    property CharacteristicM characteristic: null;

    //property CharacteristicValueTypes characteristicValueType: CharacteristicValueTypes.UNKNOWN;

    property var characteristicValue: "";

    property bool isCurrentlyEditing: false;


    //--------------------------------
    //
    //
    // Signals
    //
    //
    //--------------------------------

    // Delete Subject
    //signal deleteSubject();


    //--------------------------------
    //
    //
    // Slots
    //
    //
    //--------------------------------

    /*onCharacteristicValueChanged: {
        console.log("QML: on Characteristic Value " + characteristicValue);
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

        anchors.fill: parent

        color: "transparent"
        border {
            color: "black"
            width: 1
        }

        Text {
            anchors {
                fill: parent
                margins: 1
            }

            visible: !rootItem.isCurrentlyEditing

            text: (typeof rootItem.characteristicValue !== 'undefined') ? rootItem.characteristicValue
                                                                        : ""
        }

        TextField {
            id: txtEditor

            anchors {
                fill: parent
                margins: 1
            }

            visible: rootItem.isCurrentlyEditing && rootItem.characteristic && (rootItem.characteristic.valueType !== CharacteristicValueTypes.CHARACTERISTIC_ENUM)

            text: (typeof rootItem.characteristicValue !== 'undefined') ? rootItem.characteristicValue : ""

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
                console.log("QML: on Text Changed " + txtEditor.text);
                rootItem.characteristicValue = txtEditor.text;
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
                if (cmbEditor.selectedItem && rootItem.characteristic)
                {
                    console.log("QML on Selected Item Changed " + cmbEditor.selectedItem);
                    rootItem.characteristicValue = cmbEditor.selectedItem;
                }
            }
        }

    }
}
