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

// Needed to access to ToolTip (https://doc.qt.io/qt-5.11/qml-qtquick-controls2-tooltip.html)
import QtQuick.Controls 2.5 as Controls2

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

        Text {
            id: txtName

            anchors {
                left: parent.left
                leftMargin: 5
                top: parent.top
                bottom: parent.bottom
                margins: 1
            }
            width: 100

            text: rootItem.variable ? rootItem.variable.name + ":" : ""

            elide: Text.ElideRight
            verticalAlignment: Text.AlignVCenter

            color: IngeScapeTheme.whiteColor
            font {
                family: IngeScapeTheme.textFontFamily
                //weight: Font.Medium
                pixelSize: 12
            }

            MouseArea {
                id: mouseAreaName

                anchors.fill: parent

                hoverEnabled: true
            }

            Controls2.ToolTip {
                visible: mouseAreaName.containsMouse
                delay: 800
                text: rootItem.variable ? IndependentVariableValueTypes.enumToString(rootItem.variable.valueType) + "\n" + rootItem.variable.description : ""
            }
        }

        Text {
            id: txtValue

            anchors {
                left: txtName.right
                leftMargin: 5
                right: parent.right
                rightMargin: 1
                top: parent.top
                topMargin: 1
                bottom: parent.bottom
                bottomMargin: 1
            }

            visible: !rootItem.isCurrentlyEditing

            text: (typeof rootItem.variableValue !== 'undefined') ? rootItem.variableValue : ""

            verticalAlignment: Text.AlignVCenter

            color: IngeScapeTheme.whiteColor
            font {
                family: IngeScapeTheme.textFontFamily
                //weight: Font.Medium
                pixelSize: 12
            }
        }

        TextField {
            id: txtEditor

            property var intValidator: IntValidator {}
            property var doubleValidator: DoubleValidator {}

            anchors {
                left: txtName.right
                leftMargin: 5
                right: parent.right
                rightMargin: 1
                top: parent.top
                topMargin: 1
                bottom: parent.bottom
                bottomMargin: 1
            }

            visible: rootItem.isCurrentlyEditing && rootItem.variable && (rootItem.variable.valueType !== IndependentVariableValueTypes.INDEPENDENT_VARIABLE_ENUM)

            text: (typeof rootItem.variableValue !== 'undefined') ? rootItem.variableValue : ""

            //inputMethodHints: Qt.ImhFormattedNumbersOnly

            validator: if (rootItem.variable)
                       {
                           if (rootItem.variable.valueType === IndependentVariableValueTypes.INTEGER) {
                               return txtEditor.intValidator;
                           }
                           else if (rootItem.variable.valueType === IndependentVariableValueTypes.DOUBLE) {
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
                console.log("QML: on Text Changed " + txtEditor.text);

                // Emit the signal "Independent Variable Value Updated"
                rootItem.independentVariableValueUpdated(txtEditor.text);
            }
        }

        // FIXME: use a Loader instead of visible
        I2ComboboxStringList {
            id: cmbEditor

            anchors {
                left: txtName.right
                leftMargin: 5
                right: parent.right
                top: parent.top
                topMargin: 1
                bottom: parent.bottom
                bottomMargin: 1
            }

            visible: rootItem.isCurrentlyEditing && rootItem.variable && (rootItem.variable.valueType === IndependentVariableValueTypes.INDEPENDENT_VARIABLE_ENUM)

            model: rootItem.variable ? rootItem.variable.enumValues : null

            onSelectedItemChanged: {

                if (cmbEditor.selectedItem)
                {
                    console.log("QML: on Selected Item Changed " + cmbEditor.selectedItem);

                    // Emit the signal "Independent Variable Value Updated"
                    rootItem.independentVariableValueUpdated(cmbEditor.selectedItem);
                }
            }

        }
    }
}
