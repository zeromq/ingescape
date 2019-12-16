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

    // Model of protocol that contains our model of dependent variable
    property TaskM protocol: null

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
                }

                text: rootItem.protocol && rootItem.protocol.temporaryDependentVariable
                      ? rootItem.protocol.temporaryDependentVariable.name
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
                    if (rootItem.protocol && rootItem.protocol.temporaryDependentVariable)
                    {
                        rootItem.protocol.temporaryDependentVariable.name = text
                    }
                }
            }
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
                }

                text: rootItem.protocol && rootItem.protocol.temporaryDependentVariable
                      ? rootItem.protocol.temporaryDependentVariable.description
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
                    if (rootItem.protocol && rootItem.protocol.temporaryDependentVariable)
                    {
                        rootItem.protocol.temporaryDependentVariable.description = text
                    }
                }
            }
        }


        Item {
            id: agentColumn

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

                model: rootItem.protocol ? rootItem.protocol.hashFromAgentNameToSimplifiedAgent.keys : []

                anchors {
                    fill: parent
                    margins: 5
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
                    value: rootItem.protocol && rootItem.protocol.temporaryDependentVariable ? rootItem.protocol.temporaryDependentVariable.agentName : ""
                }

                onSelectedItemChanged: {
                    if (agentComboboxEditor.selectedItem && rootItem.protocol && rootItem.protocol.temporaryDependentVariable)
                    {
                        rootItem.protocol.temporaryDependentVariable.agentName = agentComboboxEditor.selectedItem
                    }
                }

                onVisibleChanged: {
                    if (visible && rootItem.protocol && rootItem.protocol.temporaryDependentVariable)
                    {
                        var index = agentComboboxEditor.model.indexOf(rootItem.protocol.temporaryDependentVariable.agentName);
                        if (index > -1) {
                            agentComboboxEditor.selectedIndex = index;
                        }
                    }
                }
            }
        }


        Item {
            id: optputNameColumn

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

                model: rootItem.protocol && rootItem.protocol.temporaryDependentVariable
                       && (rootItem.protocol.temporaryDependentVariable.agentName.length > 0)
                       && rootItem.protocol.hashFromAgentNameToSimplifiedAgent.containsKey(rootItem.protocol.temporaryDependentVariable.agentName)
                       ? rootItem.protocol.hashFromAgentNameToSimplifiedAgent.value(rootItem.protocol.temporaryDependentVariable.agentName).outputNamesList
                       : []

                anchors {
                    fill: parent
                    margins: 5
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
                    value: rootItem.protocol && rootItem.protocol.temporaryDependentVariable ? rootItem.protocol.temporaryDependentVariable.outputName : ""
                }

                onSelectedItemChanged: {
                    if (outputComboboxEditor.selectedItem && rootItem.protocol && rootItem.protocol.temporaryDependentVariable)
                    {
                        rootItem.protocol.temporaryDependentVariable.outputName = outputComboboxEditor.selectedItem
                    }
                }

                onVisibleChanged: {
                    if (visible && rootItem.protocol && rootItem.protocol.temporaryDependentVariable)
                    {
                        var index = outputComboboxEditor.model.indexOf(rootItem.protocol.temporaryDependentVariable.outputName);
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
                if (rootItem.protocol && rootItem.dependentVariableModel)
                {
                    rootItem.protocol.applyTemporaryDependentVariable(rootItem.dependentVariableModel);
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
        spacing: 10

        anchors {
            right: parent.right
            rightMargin: 5
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
                if (rootItem.protocol && rootItem.dependentVariableModel)
                {
                    rootItem.protocol.initTemporaryDependentVariable(rootItem.dependentVariableModel)

                    // Emit the signal
                    rootItem.editAsked();
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

                if (rootItem.protocol && rootItem.dependentVariableModel)
                {
                    rootItem.protocol.deleteDependentVariable(rootItem.dependentVariableModel)
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

