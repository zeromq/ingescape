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

    property int indexPreviousSelectedDependentVariable: -1;
    property int indexDependentVariableCurrentlyEditing: -1;

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

    /**
     * Edit a dependent variable at a row index
     */
    function editDependentVariableAtRowIndex(rowIndex) {

        console.log("QML: Edit the dependent variable at the row index " + rowIndex);

        // Set the index
        rootItem.indexDependentVariableCurrentlyEditing = rowIndex;
    }


    /**
     * Stop the current edition of a dependent variable
     */
    function stopCurrentEditionOfDependentVariable() {
        if (rootItem.indexDependentVariableCurrentlyEditing > -1)
        {
            console.log("QML: Stop the current edition of the dependent variable at the row index " + rootItem.indexDependentVariableCurrentlyEditing);

            // Reset the index
            rootItem.indexDependentVariableCurrentlyEditing = -1;
        }
    }


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
            leftMargin: 26
        }

        text: rootItem.modelM ? "TASK : " + rootItem.modelM.name.toUpperCase() : "..."
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
            rightMargin: 27
        }

        // 34 => margin between popup.top and taskName.top
        // 18 => margin between taskName.bottom and indeVar.top
        // 24 => margin between depVar.bottom and popup.bottom
        // 26 => 52 / 2 ; 52 is margin between indeVar.bottom and depVar.top
        height: (parent.height - 34 - taskName.height - 18 - 24 - 26) / 2

        Text {
            text: "INDEPENDENT VARIABLES"
            anchors {
                left: parent.left
                verticalCenter: newIndeVarButton.verticalCenter
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
            id: newIndeVarButton
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
                console.log("QML: New Independent Variable");

                // Open the popup
                createIndependentVariablePopup.open();
            }
        }

        IngeScapeAssessmentsListHeader {
            id: indeVarListHeader
            anchors {
                top: newIndeVarButton.bottom
                topMargin: 12
                left: parent.left
                right: parent.right
            }

            property var headerColumnWidths: [ 229, 336, width - 229 - 336]

            Row {
                anchors {
                    top: parent.top
                    left: parent.left
                    leftMargin: 15
                    right: parent.right
                    bottom: parent.bottom
                }

                Repeater {
                    model: [ "Name", "Description", "Type" ]

                    Text {
                        anchors {
                            verticalCenter: parent.verticalCenter
                        }

                        width: indeVarListHeader.headerColumnWidths[index]

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
            anchors {
                top: indeVarListHeader.bottom
                left: parent.left
                right: parent.right
                bottom: parent.bottom
            }

            color: IngeScapeTheme.whiteColor

            ListView {
                id: indeVarColumn
                anchors.fill: parent

                model: rootItem.modelM ? rootItem.modelM.independentVariables : null

                delegate: IndependentVariableDelegate {
                    id: indeVarDelegate

                    taskController: rootItem.taskController

                    height: 40
                    width: indeVarColumn.width
                    indeVarModel: model ? model.QtObject : null

                    columnWidths: indeVarListHeader.headerColumnWidths
                }
            }
        }

        Rectangle {
            id: indeVarBottomShadow
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
        // 18 => margin between taskName.bottom and indeVar.top
        // 24 => margin between depVar.bottom and popup.bottom
        // 26 => 52 / 2 ; 52 is margin between indeVar.bottom and depVar.top
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
                console.log("QML: New Dependent Variable");

                if (taskController) {
                    taskController.createNewDependentVariable();
                }
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
                229, // Name
                336, // Description
                231, // Agent
                227  // Output
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
                        "Output"
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
            anchors {
                top: depVarListHeader.bottom
                left: parent.left
                right: parent.right
                bottom: parent.bottom
            }

            color: IngeScapeTheme.whiteColor

            property bool depVarEditionInProgress: false

            ListView {
                id: depVarColumn
                anchors.fill: parent

                model: rootItem.modelM ? rootItem.modelM.dependentVariables : null

                delegate: DependentVariableDelegate {
                    id: depVarDelegate

                    height: 40
                    width: depVarColumn.width

                    columnWidths: depVarListHeader.headerColumnWidths

                    taskModel: rootItem.modelM
                    dependentVariableModel: model ? model.QtObject : null

                    depVarEditionInProgress: depVarTable.depVarEditionInProgress

                    Binding {
                        target: depVarTable
                        property: "depVarEditionInProgress"
                        value: depVarDelegate.isCurrentlyEditing
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
    // Create Characteristic Popup
    //
    Popup.CreateIndependentVariablePopup {
        id: createIndependentVariablePopup

        layerObjectName: "overlay2Layer"

        taskController: rootItem.taskController
    }

}
