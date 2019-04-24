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

    property var variableValue: "";

    //property bool isSelected: false;

    property bool isCurrentlyEditing: false;

    property var models: []



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

    /*onIsCurrentlyEditingChanged: {
        if (isCurrentlyEditing === false) {
            console.log("QML: is Currently Editing from true to false " + variableValue);
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
        }

        // FIXME: use a Loader instead of visible
        I2ComboboxStringList {
            id: cmbEditor

            anchors {
                fill: parent
                margins: 1
            }

            visible: rootItem.isCurrentlyEditing

            model: rootItem.models ? rootItem.models : null

            onSelectedItemChanged: {

                if (cmbEditor.selectedItem)
                {
                    //console.log("QML: on Selected Item Changed " + cmbEditor.selectedItem);

                    // Emit the signal "Characteristic Value Updated"
                    rootItem.variableValueUpdated(cmbEditor.selectedItem);
                }
            }
        }
    }
}
