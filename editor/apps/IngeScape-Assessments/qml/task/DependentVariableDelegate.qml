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

    // Task model the current dependent variable is in
    property var taskModel: null

    // Current model of dependent variable
    property var dependentVariableModel: null

    // Flag indicating if the mouse is hovering the item
    property bool isMouseHovering: itemMouseArea.containsMouse || editDepVarButton.containsMouse || deleteDepVarButton.containsMouse

    // Flag indicating if the current dependent variable is being edited
    property bool isCurrentlyEditing: false

    // List with all the column widths
    property var columnWidths: []

    // Flag indicating if a dependent variable, among all dependent variables, is being edited
    // Bound by the parent
    property bool depVarEditionInProgress: false

    color: rootItem.isCurrentlyEditing ? IngeScapeTheme.lightGreyColor
                                       : (rootItem.isMouseHovering ? IngeScapeTheme.veryLightGreyColor
                                                                   : IngeScapeTheme.whiteColor)


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
    }

    Row {
        anchors.fill: parent

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
                    rightMargin: 15
                }

                text: rootItem.dependentVariableModel ? rootItem.dependentVariableModel.name : ""

                verticalAlignment: Text.AlignVCenter
                visible: !rootItem.isCurrentlyEditing

                elide: Text.ElideRight
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

                text: rootItem.taskModel && rootItem.taskModel.temporaryDependentVariable
                      ? rootItem.taskModel.temporaryDependentVariable.name
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
                    if (rootItem.taskModel && rootItem.taskModel.temporaryDependentVariable)
                    {
                        rootItem.taskModel.temporaryDependentVariable.name = text
                    }
                }
            }
        }


        Item {
            id: descriptionColumn

            anchors {
                top: parent.top
                bottom: parent.bottom
            }

            width: rootItem.columnWidths[1]

            Text {
                anchors{
                    fill: parent
                    leftMargin: 15
                    rightMargin: 15
                }

                text: rootItem.dependentVariableModel ? rootItem.dependentVariableModel.description : ""

                verticalAlignment: Text.AlignVCenter
                visible: !rootItem.isCurrentlyEditing

                elide: Text.ElideRight
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

                text: rootItem.taskModel && rootItem.taskModel.temporaryDependentVariable
                      ? rootItem.taskModel.temporaryDependentVariable.description
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
                    if (rootItem.taskModel && rootItem.taskModel.temporaryDependentVariable)
                    {
                        rootItem.taskModel.temporaryDependentVariable.description = text
                    }
                }
            }
        }


        Item {
            id: agentColumn

            anchors {
                top: parent.top
                bottom: parent.bottom
            }

            width: rootItem.columnWidths[2]

            Text {
                anchors{
                    fill: parent
                    leftMargin: 15
                    rightMargin: 15
                }

                text: rootItem.dependentVariableModel ? rootItem.dependentVariableModel.agentName : ""

                verticalAlignment: Text.AlignVCenter
                visible: !rootItem.isCurrentlyEditing

                elide: Text.ElideRight
                color: IngeScapeAssessmentsTheme.regularDarkBlueHeader
                font {
                    family: IngeScapeTheme.textFontFamily
                    pixelSize: 16
                }
            }

            I2ComboboxStringList {
                id: agentComboboxEditor

                model: rootItem.taskModel ? rootItem.taskModel.hashFromAgentNameToSimplifiedAgent.keys : []

                anchors {
                    fill: parent
                    margins: 5
                    rightMargin: 33
                }
                visible: rootItem.isCurrentlyEditing

                style: IngeScapeAssessmentsComboboxStyle {
                    frameVisible: false
                    listBackgroundColorIdle: IngeScapeTheme.veryLightGreyColor
                }
                scrollViewStyle: IngeScapeAssessmentsScrollViewStyle {
                    scrollBarSize: 4
                    verticalScrollbarMargin: 2
                }

                Binding {
                    target: agentComboboxEditor
                    property: "selectedItem"
                    value: rootItem.taskModel && rootItem.taskModel.temporaryDependentVariable ? rootItem.taskModel.temporaryDependentVariable.agentName : ""
                }

                onSelectedItemChanged: {
                    if (agentComboboxEditor.selectedItem && rootItem.taskModel && rootItem.taskModel.temporaryDependentVariable)
                    {
                        rootItem.taskModel.temporaryDependentVariable.agentName = agentComboboxEditor.selectedItem
                    }
                }

                onVisibleChanged: {
                    if (visible && rootItem.taskModel && rootItem.taskModel.temporaryDependentVariable)
                    {
                        var index = agentComboboxEditor.model.indexOf(rootItem.taskModel.temporaryDependentVariable.agentName);
                        if (index > -1) {
                            agentComboboxEditor.selectedIndex = index;
                        }
                    }
                }
            }
        }


        Item {
            id: optputNameColumn

            anchors {
                top: parent.top
                bottom: parent.bottom
            }

            width: rootItem.columnWidths[2]

            Text {
                anchors{
                    fill: parent
                    leftMargin: 15
                    rightMargin: 15
                }

                text: rootItem.dependentVariableModel ? rootItem.dependentVariableModel.outputName : ""

                verticalAlignment: Text.AlignVCenter
                visible: !rootItem.isCurrentlyEditing

                elide: Text.ElideRight
                color: IngeScapeAssessmentsTheme.regularDarkBlueHeader
                font {
                    family: IngeScapeTheme.textFontFamily
                    pixelSize: 16
                }
            }

            I2ComboboxStringList {
                id: outputComboboxEditor

                model: rootItem.taskModel && rootItem.taskModel.temporaryDependentVariable
                       && (rootItem.taskModel.temporaryDependentVariable.agentName.length > 0)
                       && rootItem.taskModel.hashFromAgentNameToSimplifiedAgent.containsKey(rootItem.taskModel.temporaryDependentVariable.agentName)
                       ? rootItem.taskModel.hashFromAgentNameToSimplifiedAgent.value(rootItem.taskModel.temporaryDependentVariable.agentName).outputNamesList
                       : []

                anchors {
                    fill: parent
                    margins: 5
                    rightMargin: 33
                }
                visible: rootItem.isCurrentlyEditing

                style: IngeScapeAssessmentsComboboxStyle {
                    frameVisible: false
                    listBackgroundColorIdle: IngeScapeTheme.veryLightGreyColor
                }
                scrollViewStyle: IngeScapeAssessmentsScrollViewStyle {
                    scrollBarSize: 4
                    verticalScrollbarMargin: 2
                }

                Binding {
                    target: outputComboboxEditor
                    property: "selectedItem"
                    value: rootItem.taskModel && rootItem.taskModel.temporaryDependentVariable ? rootItem.taskModel.temporaryDependentVariable.outputName : ""
                }

                onSelectedItemChanged: {
                    if (outputComboboxEditor.selectedItem && rootItem.taskModel && rootItem.taskModel.temporaryDependentVariable)
                    {
                        rootItem.taskModel.temporaryDependentVariable.outputName = outputComboboxEditor.selectedItem
                    }
                }

                onVisibleChanged: {
                    if (visible && rootItem.taskModel && rootItem.taskModel.temporaryDependentVariable)
                    {
                        var index = outputComboboxEditor.model.indexOf(rootItem.taskModel.temporaryDependentVariable.outputName);
                        if (index > -1) {
                            outputComboboxEditor.selectedIndex = index;
                        }
                    }
                }
            }
        }
    }


    //
    // Buttons Apply / Cancel
    //
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
                if (rootItem.taskModel && rootItem.dependentVariableModel)
                {
                    rootItem.taskModel.applyTemporaryDependentVariable(rootItem.dependentVariableModel);
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
    }


    //
    // Buttons Edit / Delete
    //
    Row {
        spacing: 12

        anchors {
            right: parent.right
            rightMargin: 15
            verticalCenter: parent.verticalCenter
        }

        Button {
            id: editDepVarButton
            height: 30
            width: 40

            property bool containsMouse: __behavior.containsMouse

            opacity: (rootItem.isMouseHovering && !rootItem.depVarEditionInProgress) ? 1 : 0
            enabled: opacity > 0

            style: IngeScapeAssessmentsSvgButtonStyle {
                releasedID: "edit"
                disabledID: releasedID
            }

            onClicked: {
                if (rootItem.taskModel && rootItem.dependentVariableModel)
                {
                    rootItem.taskModel.initTemporaryDependentVariable(rootItem.dependentVariableModel)
                }

                rootItem.isCurrentlyEditing = true
            }
        }

        Button {
            id: deleteDepVarButton
            height: 30
            width: 40

            property bool containsMouse: __behavior.containsMouse

            opacity: (rootItem.isMouseHovering && !rootItem.depVarEditionInProgress) ? 1 : 0
            enabled: opacity > 0

            style: IngeScapeAssessmentsSvgButtonStyle {
                releasedID: "delete-blue"
                disabledID: releasedID
            }

            onClicked: {
                /*if (rootItem.taskController && rootItem.dependentVariableModel)
                {
                    rootItem.taskController.deleteDependentVariable(rootItem.dependentVariableModel)
                }*/

                if (rootItem.taskModel && rootItem.dependentVariableModel)
                {
                    rootItem.taskModel.deleteDependentVariable(rootItem.dependentVariableModel)
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
        color: rootItem.isCurrentlyEditing ? IngeScapeTheme.lightGreyColor : IngeScapeTheme.veryLightGreyColor
    }
}

