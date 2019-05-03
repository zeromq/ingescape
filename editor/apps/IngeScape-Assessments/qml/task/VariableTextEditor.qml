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

    // Model of our (Dependent) Variable
    property DependentVariableM variable: null;

    // Keep type "var" because the C++ use a QVariant
    property var variableValue: "";

    // FLag indicating
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

    // Signal emitted when the user clicks on the toggle button to edit the variable
    signal editVariable();

    // Signal emitted when the user clicks on the toggle button to stop the edition of the variable
    signal stopEditionOfVariable();

    // Variable Value Updated
    signal variableValueUpdated(var value);

    // Delete Dependent Variable
    signal deleteDependentVariable();


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

    // FIXME: Loader instead of visible on "rootItem.isFirstColumn" ?
    // --> What is the least expensive between a Loader and a Button ?

    Button {
        id: btnDeleteDependentVariable

        anchors {
            left: parent.left
            top: parent.top
        }
        width: 50
        height: 30

        text: "DEL"

        //visible: styleData.selected
        visible: styleData.selected && rootItem.isFirstColumn

        onClicked: {
            // Emit the signal "Delete Dependent Variable"
            rootItem.deleteDependentVariable();
        }
    }

    Rectangle {
        id: leftSeparator

        anchors {
            left: parent.left
            leftMargin: 50
            top: parent.top
            bottom: parent.bottom
        }
        width: 1
        color: "silver"

        visible: rootItem.isFirstColumn
    }

    /*Rectangle {
        id: background

        anchors {
            fill: parent
        }

        color: "transparent"
        border {
            color: "silver"
            width: 1
        }
    }*/

    Text {
        anchors {
            fill: parent
            leftMargin: rootItem.isFirstColumn ? 55 : 5
        }

        visible: !rootItem.isCurrentlyEditing

        text: (typeof rootItem.variableValue !== 'undefined') ? rootItem.variableValue : ""

        verticalAlignment: Text.AlignVCenter
        //color: styleData.selected ? IngeScapeTheme.whiteColor : IngeScapeTheme.blackColor
        font {
            family: IngeScapeTheme.textFontFamily
            //weight: Font.Medium
            pixelSize: 12
        }
    }

    TextField {
        id: txtEditor

        anchors {
            fill: parent
            leftMargin: rootItem.isFirstColumn ? 50 : 0
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
