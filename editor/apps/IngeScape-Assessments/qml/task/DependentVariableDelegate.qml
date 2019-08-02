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

    // List with all the column widths
    property var columnWidths: []

    // Current dependent variable model
    property var dependentVariableModel: null

    // Flag indicating if the mouse is hovering the item
    property bool isMouseHovering: itemMouseArea.containsMouse || editDepVarButton.containsMouse || deleteDepVarButton.containsMouse

    // Flag indicating if the current dependent variable is being edited
    property bool isCurrentlyEditing: false

    // Flag indicating if a dependent variable, amongst all dependent variables, is being edited
    // Bound by the parent
    property bool depVarEditionInProgress: false

    color: rootItem.isCurrentlyEditing ? IngeScapeTheme.lightGreyColor
                                       : (rootItem.isMouseHovering ? IngeScapeTheme.veryLightGreyColor
                                                                   : IngeScapeTheme.whiteColor)


    //
    // Signals
    //

    signal deleteDepVariable();


    //
    // Content
    //

    MouseArea {
        id: itemMouseArea
        anchors.fill: parent
        hoverEnabled: true
    }

    Row {
        anchors {
            fill: parent
        }

        Item {
            id: nameColumn

            anchors {
                top: parent.top
                bottom: parent.bottom
            }

            width: rootItem.columnWidths[0]

            Text {
                anchors{
                    fill: parent
                    leftMargin: 15
                }

                text: rootItem.dependentVariableModel ? rootItem.dependentVariableModel.name : ""

                verticalAlignment: Text.AlignVCenter
                visible: !rootItem.isCurrentlyEditing

                color: IngeScapeAssessmentsTheme.regularDarkBlueHeader
                font {
                    family: IngeScapeTheme.textFontFamily
                    weight: Font.Medium
                    pixelSize: 16
                }
            }

            TextField {
                anchors {
                    fill: parent
                    margins: 5
                    rightMargin: 33
                }

                text: rootItem.dependentVariableModel ? rootItem.dependentVariableModel.name : ""
                visible: rootItem.isCurrentlyEditing

                style: I2TextFieldStyle {
                    backgroundColor: IngeScapeTheme.whiteColor
                    borderColor: IngeScapeTheme.lightGreyColor
                    borderErrorColor: IngeScapeTheme.redColor
                    radiusTextBox: 5
                    borderWidth: 0
                    borderWidthActive: 1
                    textIdleColor: IngeScapeTheme.regularDarkBlueHeader
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
                    if (rootItem.dependentVariableModel)
                    {
                        console.log("TODO Handle temporary")
//                        rootItem.dependentVariableModel.name = text
                    }
                }
            }
        }


        Item {
            id: descriptionColumn

            anchors {
                left: nameColumn.right
                top: parent.top
                bottom: parent.bottom
            }

            width: rootItem.columnWidths[1]

            Text {
                anchors{
                    fill: parent
                    leftMargin: 15
                }

                text: rootItem.dependentVariableModel ? rootItem.dependentVariableModel.description : ""

                verticalAlignment: Text.AlignVCenter
                visible: !rootItem.isCurrentlyEditing

                color: IngeScapeAssessmentsTheme.regularDarkBlueHeader
                font {
                    family: IngeScapeTheme.textFontFamily
                    pixelSize: 16
                }
            }

            TextField {
                anchors {
                    fill: parent
                    margins: 5
                    rightMargin: 33
                }

                text: rootItem.dependentVariableModel ? rootItem.dependentVariableModel.description : ""
                visible: rootItem.isCurrentlyEditing

                style: I2TextFieldStyle {
                    backgroundColor: IngeScapeTheme.whiteColor
                    borderColor: IngeScapeTheme.lightGreyColor
                    borderErrorColor: IngeScapeTheme.redColor
                    radiusTextBox: 5
                    borderWidth: 0
                    borderWidthActive: 1
                    textIdleColor: IngeScapeTheme.regularDarkBlueHeader
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
                    if (rootItem.dependentVariableModel)
                    {
                        console.log("TODO Handle temporary")
//                        rootItem.dependentVariableModel.description = text
                    }
                }
            }
        }


        Item {
            id: agentColumn

            anchors {
                left: descriptionColumn.right
                top: parent.top
                bottom: parent.bottom
            }

            width: rootItem.columnWidths[2]

            Text {
                anchors{
                    fill: parent
                    leftMargin: 15
                }

                text: rootItem.dependentVariableModel ? rootItem.dependentVariableModel.agentName : ""

                verticalAlignment: Text.AlignVCenter
                visible: !rootItem.isCurrentlyEditing

                color: IngeScapeAssessmentsTheme.regularDarkBlueHeader
                font {
                    family: IngeScapeTheme.textFontFamily
                    pixelSize: 16
                }
            }

            I2ComboboxStringList {
                id: agentComboboxEditor

                model: null //TODO

                anchors {
                    fill: parent
                    margins: 5
                    rightMargin: 33
                }
                visible: rootItem.isCurrentlyEditing

                style: I2ComboboxStyle {
                    borderColorIdle: IngeScapeTheme.veryLightGreyColor
                    currentTextColorIdle: IngeScapeTheme.regularDarkBlueHeader
                    font {
                        family: IngeScapeTheme.textFontFamily
                        weight: Font.Medium
                        pixelSize: 16
                    }
                }

                Binding {
                    target: agentComboboxEditor
                    property: "selectedItem"
                    value: rootItem.dependentVariableModel.agentName
                }

                onSelectedItemChanged: {
                    console.log("TODO Handle temporary")
                }

                onVisibleChanged: {
                    console.log("TODO Handle temporary")
                }
            }
        }


        Item {
            id: optputNameColumn

            anchors {
                left: agentColumn.right
                top: parent.top
                bottom: parent.bottom
            }

            width: rootItem.columnWidths[2]

            Text {
                anchors{
                    fill: parent
                    leftMargin: 15
                }

                text: rootItem.dependentVariableModel ? rootItem.dependentVariableModel.outputName : ""

                verticalAlignment: Text.AlignVCenter
                visible: !rootItem.isCurrentlyEditing

                color: IngeScapeAssessmentsTheme.regularDarkBlueHeader
                font {
                    family: IngeScapeTheme.textFontFamily
                    pixelSize: 16
                }
            }

            I2ComboboxStringList {
                id: outputComboboxEditor

                anchors.fill: parent

                anchors {
                    fill: parent
                    margins: 5
                    rightMargin: 33
                }
                visible: rootItem.isCurrentlyEditing

                style: I2ComboboxStyle {
                    borderColorIdle: IngeScapeTheme.veryLightGreyColor
                    currentTextColorIdle: IngeScapeTheme.regularDarkBlueHeader
                    font {
                        family: IngeScapeTheme.textFontFamily
                        weight: Font.Medium
                        pixelSize: 16
                    }
                }

                Binding {
                    target: outputComboboxEditor
                    property: "selectedItem"
                    value: rootItem.dependentVariableModel.outputName
                }

                onSelectedItemChanged: {
                    console.log("TODO Handle temporary")
                }

                onVisibleChanged: {
                    console.log("TODO Handle temporary")
                }
            }
        }
    }

    Row {
        spacing: 12

        anchors {
            right: parent.right
            rightMargin: 18
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
                console.log("TODO Handle temporary")

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
    }

    Row {
        spacing: 12

        anchors {
            right: parent.right
            rightMargin: 18
            verticalCenter: parent.verticalCenter
        }

        Button {
            id: deleteDepVarButton
            height: 30
            width: 40

            property bool containsMouse: __behavior.containsMouse

            opacity: rootItem.isMouseHovering && !rootItem.depVarEditionInProgress ? 1 : 0
            enabled: opacity > 0

            style: IngeScapeAssessmentsSvgButtonStyle {
                releasedID: "delete-blue"
                disabledID: releasedID
            }

            onClicked: {
                console.log("TODO Handle temporary")
            }
        }

        Button {
            id: editDepVarButton
            height: 30
            width: 40

            property bool containsMouse: __behavior.containsMouse

            opacity: rootItem.isMouseHovering && !rootItem.depVarEditionInProgress ? 1 : 0
            enabled: opacity > 0

            style: IngeScapeAssessmentsSvgButtonStyle {
                releasedID: "edit"
                disabledID: releasedID
            }

            onClicked: {
                console.log("TODO Handle temporary")

                rootItem.isCurrentlyEditing = true
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
        color: rootItem.isCurrentlyEditing ? IngeScapeTheme.lightGreyColor : IngeScapeTheme.veryLightGreyColor
    }
}

