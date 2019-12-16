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
import "../popup" as Popup


Item {
    id: rootItem

    //--------------------------------------------------------
    //
    //
    // Properties
    //
    //
    //--------------------------------------------------------

    property TasksController taskController: null;

    property TaskM modelM: taskController ? taskController.selectedTask : null;

    property IndependentVariableM independentVariableCurrentlyEdited : null;

    visible: rootItem.modelM


    //--------------------------------
    //
    //
    // Signals
    //
    //
    //--------------------------------



    //--------------------------------
    //
    //
    // Functions
    //
    //
    //--------------------------------


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
        color: IngeScapeTheme.veryLightGreyColor
    }

    Text {
        id: taskName

        anchors {
            top: parent.top
            topMargin: 34
            left: parent.left
            leftMargin: 20
        }

        text: rootItem.modelM ? rootItem.modelM.name.toUpperCase() : "..."
        verticalAlignment: Text.AlignVCenter

        color: IngeScapeAssessmentsTheme.regularDarkBlueHeader
        font {
            family: IngeScapeTheme.labelFontFamily
            weight: Font.Black
            pixelSize: 20
        }
    }

    //
    // Independent Variables Panel
    //
    Item {
        id: panelIndependentVariable

        anchors {
            top: taskName.bottom
            topMargin: 18
            left: taskName.left
            right: parent.right
            rightMargin: 20
        }

        // 34 => margin between popup.top and taskName.top
        // 18 => margin between taskName.bottom and indepVar.top
        // 24 => margin between depVar.bottom and popup.bottom
        // 26 => 52 / 2 ; 52 is margin between indepVar.bottom and depVar.top
        height: (parent.height - 34 - taskName.height - 18 - 24 - 26) / 2

        Text {
            text: "INDEPENDENT VARIABLES"
            anchors {
                left: parent.left
                verticalCenter: newIndepVarButton.verticalCenter
            }

            verticalAlignment: Text.AlignVCenter

            color: IngeScapeAssessmentsTheme.regularDarkBlueHeader
            font {
                family: IngeScapeTheme.labelFontFamily
                weight: Font.Black
                pixelSize: 18
            }
        }

        Button {
            id: newIndepVarButton
            anchors {
                top: parent.top
                right: parent.right
            }

            height: 40
            width: 263

            style: IngeScapeAssessmentsButtonStyle {
                text: "NEW INDEPENDENT VARIABLE"
            }

            onClicked: {
                //console.log("QML: New Independent Variable");

                rootItem.independentVariableCurrentlyEdited = null;

                // Reset temporary independent variable to allow creation
                rootItem.taskController.initTemporaryIndependentVariable(null);

                // Open the popup
                independentVariablePopup.open();
            }
        }

        IngeScapeAssessmentsListHeader {
            id: indepVarListHeader

            anchors {
                top: newIndepVarButton.bottom
                topMargin: 12
                left: parent.left
                right: parent.right
            }

            property var headerColumnWidths: [
                220,    // Name
                width - 220 - 220 - 95,  // Description
                220,    // Value type
                95      // Buttons
            ]

            Row {
                anchors {
                    top: parent.top
                    left: parent.left
                    leftMargin: 15
                    right: parent.right
                    bottom: parent.bottom
                }

                Repeater {
                    model: [
                        "Name",
                        "Description",
                        "Type",
                        ""          // Buttons
                    ]

                    Text {
                        anchors {
                            verticalCenter: parent.verticalCenter
                        }

                        width: indepVarListHeader.headerColumnWidths[index]

                        text: modelData
                        color: IngeScapeTheme.whiteColor
                        font {
                            family: IngeScapeTheme.labelFontFamily
                            pixelSize: 16
                            weight: Font.Black
                        }
                    }
                }
            }
        }

        Rectangle {
            id: indepVarTable

            property bool indepVarEditionInProgress: false

            anchors {
                top: indepVarListHeader.bottom
                left: parent.left
                right: parent.right
                bottom: parent.bottom
            }

            color: IngeScapeTheme.whiteColor
            clip: true

            ListView {
                id: indepVarListView
                anchors.fill: parent

                model: rootItem.modelM ? rootItem.modelM.independentVariables : null

                delegate: IndependentVariableDelegate {
                    id: indepVarDelegate

                    taskController: rootItem.taskController
                    protocol: rootItem.modelM
                    independentVarModel: model ? model.QtObject : null
                    indepVarEditionInProgress: indepVarTable.indepVarEditionInProgress

                    height: 40
                    width: indepVarListView.width

                    columnWidths: indepVarListHeader.headerColumnWidths

                    /*Binding {
                        target: indepVarTable
                        property: "indepVarEditionInProgress"
                        value: indepVarDelegate.isCurrentlyEditing
                    }*/

                    onEditAsked: {
                        if (indepVarDelegate.independentVarModel /*&& rootItem.modelM*/)
                        {
                            //console.log("onEditAsked " + indepVarDelegate.independentVarModel.name);

                            // Configure the popup with the temporary Independent Variable instead of the current variable
                            // Because the "var enumTexts: []" is automatically binded to the I2_QML_PROPERTY(QStringList, enumValues)
                            // with the line: enumTexts = independentVariableToEdit.enumValues;
                            rootItem.independentVariableCurrentlyEdited = indepVarDelegate.independentVarModel;
                            rootItem.taskController.initTemporaryIndependentVariable(indepVarDelegate.independentVarModel);

                            // Open the popup
                            independentVariablePopup.open();
                        }
                    }
                }
            }
        }

        Rectangle {
            id: indepVarBottomShadow

            anchors {
                left: parent.left
                right: parent.right
                bottom: parent.bottom
            }
            height: 4
            gradient: Gradient {
                GradientStop { position: 0.0; color: IngeScapeTheme.whiteColor; }
                GradientStop { position: 1.0; color: IngeScapeTheme.darkGreyColor; }
            }
        }
    }


    //
    // Dependent Variables Panel
    //
    Item {
        id: panelDependentVariable

        anchors {
            left: taskName.left
            right: panelIndependentVariable.right
            bottom: parent.bottom
            bottomMargin: 24
        }

        // 34 => margin between popup.top and taskName.top
        // 18 => margin between taskName.bottom and indepVar.top
        // 24 => margin between depVar.bottom and popup.bottom
        // 26 => 52 / 2 ; 52 is margin between indepVar.bottom and depVar.top
        height: (parent.height - 34 - taskName.height - 18 - 24 - 26) / 2

        Text {
            text: "DEPENDENT VARIABLES"
            anchors {
                left: parent.left
                verticalCenter: newDepVarButton.verticalCenter
            }

            verticalAlignment: Text.AlignVCenter

            color: IngeScapeAssessmentsTheme.regularDarkBlueHeader
            font {
                family: IngeScapeTheme.labelFontFamily
                weight: Font.Black
                pixelSize: 18
            }
        }

        Button {
            id: newDepVarButton
            anchors {
                top: parent.top
                right: parent.right
            }

            height: 40
            width: 263

            style: IngeScapeAssessmentsButtonStyle {
                text: "NEW DEPENDENT VARIABLE"
            }

            onClicked: {
                //console.log("QML: New Dependent Variable");

                // Open the popup
                dependentVariablePopup.open();
            }
        }

        IngeScapeAssessmentsListHeader {
            id: depVarListHeader

            anchors {
                top: newDepVarButton.bottom
                topMargin: 12
                left: parent.left
                right: parent.right
            }

            property var headerColumnWidths: [
                220,    // Name
                width - 220 - 220 - 220 - 95, // Description
                220,    // Agent
                220,    // Output
                95      // Buttons
            ]

            Row {
                anchors {
                    top: parent.top
                    left: parent.left
                    leftMargin: 15
                    right: parent.right
                    bottom: parent.bottom
                }

                Repeater {
                    model: [
                        "Name",
                        "Description",
                        "Agent",
                        "Output",
                        ""          // Buttons
                    ]

                    Text {
                        anchors {
                            verticalCenter: parent.verticalCenter
                        }

                        width: depVarListHeader.headerColumnWidths[index]

                        text: modelData
                        color: IngeScapeTheme.whiteColor
                        font {
                            family: IngeScapeTheme.labelFontFamily
                            weight: Font.Black
                            pixelSize: 16
                        }
                    }
                }
            }
        }

        Rectangle {
            id: depVarTable

            property bool depVarEditionInProgress: false

            anchors {
                top: depVarListHeader.bottom
                left: parent.left
                right: parent.right
                bottom: parent.bottom
            }

            color: IngeScapeTheme.whiteColor
            clip: true

            ListView {
                id: depVarListView

                anchors.fill: parent

                model: rootItem.modelM ? rootItem.modelM.dependentVariables : null

                delegate: DependentVariableDelegate {
                    id: depVarDelegate

                    protocol: rootItem.modelM
                    dependentVariableModel: model ? model.QtObject : null
                    depVarEditionInProgress: depVarTable.depVarEditionInProgress

                    height: 40
                    width: depVarListView.width

                    columnWidths: depVarListHeader.headerColumnWidths

                    Binding {
                        target: depVarTable
                        property: "depVarEditionInProgress"
                        value: depVarDelegate.isCurrentlyEditing
                    }

                    onEditAsked: {
                        if (depVarDelegate.dependentVariableModel)
                        {
                            //console.log("onEditAsked " + depVarDelegate.dependentVariableModel.name);

                            // Open the popup
                            dependentVariablePopup.dependentVariableCurrentlyEdited = depVarDelegate.dependentVariableModel;
                            dependentVariablePopup.open();
                        }
                    }
                }
            }
        }

        Rectangle {
            id: depVarBottomShadow
            anchors {
                left: parent.left
                right: parent.right
                bottom: parent.bottom
            }
            height: 4
            gradient: Gradient {
                GradientStop { position: 0.0; color: IngeScapeTheme.whiteColor; }
                GradientStop { position: 1.0; color: IngeScapeTheme.darkGreyColor; }
            }
        }
    }


    //
    // Create Independent Variable Popup
    //
    Popup.CreateIndependentVariablePopup {
        id: independentVariablePopup

        layerObjectName: "overlay2Layer"

        taskController: rootItem.taskController

        independentVariableToEdit: rootItem.taskController.temporaryIndependentVariable

        onIndependentVariableIsEdited : {
            if ((rootItem.taskController) && (rootItem.taskController.temporaryIndependentVariable)) {
                if (rootItem.independentVariableCurrentlyEdited) {
                    // Edit an existing independent variable
                    rootItem.taskController.saveModificationsOfIndependentVariableFromTemporary(rootItem.independentVariableCurrentlyEdited);
                }
                else {
                    // Create an existing independent variable
                    rootItem.taskController.createNewIndependentVariableFromTemporary();
                }
            }
        }
    }


    //
    // Create Dependent Variable Popup
    //
    Popup.CreateDependentVariablePopup {
        id: dependentVariablePopup

        layerObjectName: "overlay2Layer"

        taskController: rootItem.taskController
        protocolM: rootItem.modelM
    }
}
