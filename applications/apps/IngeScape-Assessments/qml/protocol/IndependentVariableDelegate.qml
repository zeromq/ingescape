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
 *      Mathieu Soum <soum@ingenuity.io>
 *
 */

import QtQuick 2.9
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4

import I2Quick 1.0

import INGESCAPE 1.0

Rectangle {
    id: rootItem

    //--------------------------------------------------------
    //
    //
    // Properties
    //
    //
    //--------------------------------------------------------

    // Protocols controller
    property ProtocolsController protocolsController: null;

    // Model of protocol that contains our model of independent variable
//    property ProtocolM protocol: null

    // Current model of independent variable
    property IndependentVariableM independentVarModel: null

    // Flag indicating if the mouse is hovering the item
    property bool isMouseHovering: itemMouseArea.containsMouse || editIndepVarButton.containsMouse || deleteIndepVarButton.containsMouse

    // Flag indicating if the current independent variable is being edited
    //property bool isCurrentlyEditing: false

    // Width of the columns (bound by the parent)
    property var columnWidths: [ 0, 0, 0 ]

    // Flag indicating if an independent variable, among all independent variables, is being edited
    // Bound by the parent
    property bool indepVarEditionInProgress: false

    color: rootItem.isMouseHovering ? IngeScapeTheme.veryLightGreyColor
                                    : IngeScapeTheme.whiteColor


    //--------------------------------
    //
    //
    // Signals
    //
    //
    //--------------------------------

    signal editAsked();


    //--------------------------------------------------------
    //
    //
    // Content
    //
    //
    //--------------------------------------------------------

    MouseArea {
        id: itemMouseArea
        anchors.fill: parent
        hoverEnabled: true

        onDoubleClicked: {
            if (editIndepVarButton.enabled)
            {
                // Emit the signal
                rootItem.editAsked();
            }
        }
    }

    Row {
        anchors.fill: parent
        spacing: 0

        Item {
            id: nameColumn

            width: rootItem.columnWidths[0]
            height: parent.height

            /*Rectangle {
                width: parent.width
                height: parent.height
                color: "transparent"
                border {
                    color: "red"
                    width: 1
                }
            }*/

            Text {
                anchors{
                    fill: parent
                    leftMargin: 15
                    rightMargin: 5
                }

                text: rootItem.independentVarModel ? rootItem.independentVarModel.name : ""

                verticalAlignment: Text.AlignVCenter
                //visible: !rootItem.isCurrentlyEditing

                elide: Text.ElideRight
                color: IngeScapeAssessmentsTheme.regularDarkBlueHeader
                font {
                    family: IngeScapeTheme.textFontFamily
                    weight: Font.Medium
                    pixelSize: 16
                }
            }

            /*TextField {
                anchors {
                    fill: parent
                    margins: 5
                }

                text: rootItem.protocol && rootItem.protocol.temporaryIndependentVariable
                      ? rootItem.protocol.temporaryIndependentVariable.name
                      : ""
                visible: rootItem.isCurrentlyEditing

                style: I2TextFieldStyle {
                    backgroundColor: IngeScapeTheme.whiteColor
                    borderColor: IngeScapeTheme.lightGreyColor
                    borderErrorColor: IngeScapeTheme.redColor
                    radiusTextBox: 5
                    borderWidth: 0
                    borderWidthActive: 1
                    textIdleColor: IngeScapeAssessmentsTheme.regularDarkBlueHeader
                    textDisabledColor: IngeScapeTheme.veryLightGreyColor

                    padding.left: 10
                    padding.right: 5

                    font {
                        family: IngeScapeTheme.textFontFamily
                        weight: Font.Medium
                        pixelSize: 16
                    }
                }

                onTextChanged: {
                    if (rootItem.protocol && rootItem.protocol.temporaryIndependentVariable)
                    {
                        rootItem.protocol.temporaryIndependentVariable.name = text
                    }
                }
            }*/
        }


        Item {
            id: descriptionColumn

            width: rootItem.columnWidths[1]
            height: parent.height

            /*Rectangle {
                width: parent.width
                height: parent.height
                color: "transparent"
                border {
                    color: "red"
                    width: 1
                }
            }*/

            Text {
                anchors{
                    fill: parent
                    leftMargin: 15
                    rightMargin: 5
                }

                text: rootItem.independentVarModel ? rootItem.independentVarModel.description : ""

                verticalAlignment: Text.AlignVCenter
                //visible: !rootItem.isCurrentlyEditing

                elide: Text.ElideRight
                color: IngeScapeAssessmentsTheme.regularDarkBlueHeader
                font {
                    family: IngeScapeTheme.textFontFamily
                    pixelSize: 16
                }
            }

            /*TextField {
                anchors {
                    fill: parent
                    margins: 5
                }

                text: rootItem.protocol && rootItem.protocol.temporaryIndependentVariable
                      ? rootItem.protocol.temporaryIndependentVariable.description
                      : ""
                visible: rootItem.isCurrentlyEditing

                style: I2TextFieldStyle {
                    backgroundColor: IngeScapeTheme.whiteColor
                    borderColor: IngeScapeTheme.lightGreyColor
                    borderErrorColor: IngeScapeTheme.redColor
                    radiusTextBox: 5
                    borderWidth: 0
                    borderWidthActive: 1
                    textIdleColor: IngeScapeAssessmentsTheme.regularDarkBlueHeader
                    textDisabledColor: IngeScapeTheme.veryLightGreyColor

                    padding.left: 10
                    padding.right: 5

                    font {
                        family: IngeScapeTheme.textFontFamily
                        weight: Font.Medium
                        pixelSize: 16
                    }
                }

                onTextChanged: {
                    if (rootItem.protocol && rootItem.protocol.temporaryIndependentVariable)
                    {
                        rootItem.protocol.temporaryIndependentVariable.description = text
                    }
                }
            }*/
        }


        Item {
            id: typeColumn

            width: rootItem.columnWidths[2]
            height: parent.height

            /*Rectangle {
                width: parent.width
                height: parent.height
                color: "transparent"
                border {
                    color: "red"
                    width: 1
                }
            }*/

            Text {
                anchors{
                    fill: parent
                    leftMargin: 15
                    rightMargin: 5
                }

                text: rootItem.independentVarModel ? IndependentVariableValueTypes.enumToString(rootItem.independentVarModel.valueType)
                                                   : ""

                verticalAlignment: Text.AlignVCenter
                //visible: !rootItem.isCurrentlyEditing

                elide: Text.ElideRight
                color: IngeScapeAssessmentsTheme.regularDarkBlueHeader
                font {
                    family: IngeScapeTheme.textFontFamily
                    pixelSize: 16
                }
            }

            // Combo to select the type of the value comparison
            /*I2ComboboxItemModel {
                id: comboBoxValueTypes

                anchors {
                    fill: parent
                    margins: 5
                }

                model: rootItem.protocolsController ? rootItem.protocolsController.allIndependentVariableValueTypes : null

                visible: rootItem.isCurrentlyEditing

                style: IngeScapeAssessmentsComboboxStyle {
                    frameVisible: false
                    listBackgroundColorIdle: IngeScapeTheme.veryLightGreyColor
                }
                scrollViewStyle: IngeScapeAssessmentsScrollViewStyle {
                    scrollBarSize: 4
                    verticalScrollbarMargin: 2
                }

                function modelToString(entry)
                {
                    return entry.name;
                }

                Binding {
                    target: comboBoxValueTypes
                    property: "selectedIndex"
                    value: (rootItem.protocolsController && rootItem.protocol && rootItem.protocol.temporaryIndependentVariable) ? rootItem.protocolsController.allIndependentVariableValueTypes.indexOfEnumValue(rootItem.protocol.temporaryIndependentVariable.valueType)
                                                                                                                            : -1
                }

                onSelectedItemChanged: {
                    if ((comboBoxValueTypes.selectedIndex >= 0) && rootItem.protocol && rootItem.protocol.temporaryIndependentVariable)
                    {
                        rootItem.protocol.temporaryIndependentVariable.valueType = comboBoxValueTypes.selectedItem.value;
                    }
                }
            }*/
        }

    }


    //
    // Buttons Apply / Cancel
    //
    /*Row {
        spacing: 10

        anchors {
            right: parent.right
            rightMargin: 0
            verticalCenter: parent.verticalCenter
        }

        Button {
            id: aplpyEditionButton
            height: 30
            width: 86

            opacity: rootItem.isCurrentlyEditing ? 1 : 0
            enabled: opacity > 0

            style: IngeScapeAssessmentsButtonStyle {
                text: "APPLY"
            }

            onClicked: {
                if (rootItem.protocol && rootItem.independentVarModel)
                {
                    rootItem.protocol.applyTemporaryIndependentVariable(rootItem.independentVarModel);
                }

                rootItem.isCurrentlyEditing = false
            }
        }

        Button {
            id: cancelEditionButton
            anchors {
                verticalCenter: parent.verticalCenter
            }

            height: 18
            width: 18

            opacity: rootItem.isCurrentlyEditing ? 1 : 0
            enabled: opacity > 0

            style: IngeScapeAssessmentsSvgButtonStyle {
                releasedID: "cancel-edition"
                disabledID: releasedID
            }

            onClicked: {
                rootItem.isCurrentlyEditing = false
            }
        }
    }*/


    //
    // Buttons Edit / Delete
    //
    Row {
        spacing: 10

        anchors {
            right: parent.right
            rightMargin: 5
            verticalCenter: parent.verticalCenter
        }

        Button {
            id: editIndepVarButton
            height: 30
            width: 40

            property bool containsMouse: __behavior.containsMouse

            opacity: (rootItem.isMouseHovering && !rootItem.indepVarEditionInProgress) ? 1 : 0
            enabled: opacity > 0

            style: IngeScapeAssessmentsSvgButtonStyle {
                releasedID: "edit"
                disabledID: releasedID
            }

            onClicked: {
                // Emit the signal
                rootItem.editAsked();
            }
        }

        Button {
            id: deleteIndepVarButton

            property bool containsMouse: __behavior.containsMouse

            height: 30
            width: 40

            opacity: (rootItem.isMouseHovering && !rootItem.indepVarEditionInProgress) ? 1 : 0
            enabled: opacity > 0

            style: IngeScapeAssessmentsSvgButtonStyle {
                releasedID: "delete-blue"
                disabledID: releasedID
            }

            onClicked: {
                if (rootItem.protocolsController && rootItem.independentVarModel)
                {
                    rootItem.protocolsController.deleteIndependentVariable(rootItem.independentVarModel)
                }
            }
        }
    }


    Rectangle {
        id: bottomSeparator

        anchors {
            left: parent.left
            right: parent.right
            bottom: parent.bottom
        }
        height: 2
        //color: rootItem.isCurrentlyEditing ? IngeScapeTheme.lightGreyColor : IngeScapeTheme.veryLightGreyColor
        color: IngeScapeTheme.veryLightGreyColor
    }
}
