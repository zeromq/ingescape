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
    height: parent.height


    //--------------------------------------------------------
    //
    //
    // Properties
    //
    //
    //--------------------------------------------------------

    property DependentVariableM variable: null;
    //property IndependentVariableM variable: null;

    // Keep type "var" because the C++ use a QVariant
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

    // Variable Value Updated
    signal variableValueUpdated(var value);

    // Delete Variable
    //signal deleteVariable();


    //--------------------------------
    //
    //
    // Slots
    //
    //
    //--------------------------------

    /*onVariableChanged: {
        console.log("QML: on Variable changed " + variable.name);
    }*/

    /*onVariableValueChanged: {
        console.log("QML: on Variable Value changed " + variableValue);
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
            //leftMargin: (variable && variable.isSubjectName) ? 105 : 0
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
            anchors {
                fill: parent
                margins: 1
            }

            visible: !rootItem.isCurrentlyEditing

            text: (typeof rootItem.variableValue !== 'undefined') ? rootItem.variableValue : ""

            verticalAlignment: Text.AlignVCenter
        }

        TextField {
            id: txtEditor

            anchors {
                fill: parent
                margins: 1
            }

            visible: rootItem.isCurrentlyEditing

            text: (typeof rootItem.variableValue !== 'undefined') ? rootItem.variableValue : ""

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

                // Emit the signal "Variable Value Updated"
                rootItem.variableValueUpdated(txtEditor.text);
            }
        }

    }
}
