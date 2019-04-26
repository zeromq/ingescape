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
import QtQml 2.12

import I2Quick 1.0

import INGESCAPE 1.0

//import "theme" as Theme
import "../popup" as Popup


Item {
    id: rootItem

    //anchors.fill: parent


    //--------------------------------------------------------
    //
    //
    // Properties
    //
    //
    //--------------------------------------------------------

    property TasksController controller: null;

    property TaskM modelM: controller ? controller.selectedTask : null;

    property int indexPreviousSelectedDependentVariable: -1;
    property int indexDependentVariableCurrentlyEditing: -1;

    visible: rootItem.modelM ? true : false


    //--------------------------------
    //
    //
    // Signals
    //
    //
    //--------------------------------

    //
    //signal todo();



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

        color: "lightsteelblue"
        border {
            color: "black"
            width: 1
        }
    }


    /*Text {
        id: txtName

        anchors {
            top: parent.top
            topMargin: 10
            horizontalCenter: parent.horizontalCenter
        }

        text: rootItem.modelM ? rootItem.modelM.name : "..."

        color: IngeScapeTheme.whiteColor
        font {
            family: IngeScapeTheme.textFontFamily
            weight: Font.Medium
            pixelSize: 18
        }
    }*/


    //
    // Independent Variables Panel
    //
    Rectangle {
        id: panelIndepVar

        anchors {
            left: parent.left
            right: parent.right
            top: parent.top
        }
        height: parent.height / 2

        color: "transparent"
        border {
            width: 1
            color: "white"
        }

        Row {
            id: headerIndepVar

            anchors {
                left: parent.left
                leftMargin: 10
                top: parent.top
                topMargin: 10
            }

            spacing: 20

            Text {
                text: "Independent Variables"

                color: IngeScapeTheme.whiteColor
                font {
                    family: IngeScapeTheme.textFontFamily
                    weight: Font.Medium
                    pixelSize: 18
                }
            }

            Button {
                text: "New Independent Variable"

                height: 30

                onClicked: {
                    console.log("QML: New Independent Variable");

                    // Open the popup
                    createIndependentVariablePopup.open();
                }
            }
        }


        TableView {
            id: tableIndepVar

            anchors {
                left: parent.left
                leftMargin: 10
                right: parent.right
                rightMargin: 10
                top: headerIndepVar.bottom
                topMargin: 10
                bottom: parent.bottom
                bottomMargin: 10
            }

            rowDelegate: Rectangle {
                width: childrenRect.width
                height: styleData.selected ? 30 : 20

                color: styleData.selected ? "lightblue"
                                          : (styleData.alternate ? "lightgray" : "white")
            }

            model: rootItem.modelM ? rootItem.modelM.independentVariables : null

            TableViewColumn {
                role: "name"
                title: qsTr("Nom")
            }

            TableViewColumn {
                role: "description"
                title: qsTr("Description")
            }

            TableViewColumn {
                role: "valueType"
                title: qsTr("Type")

                delegate: Text {
                    text: IndependentVariableValueTypes.enumToString(styleData.value) + ((styleData.value === IndependentVariableValueTypes.INDEPENDENT_VARIABLE_ENUM) ? " {" + tableIndepVar.model.get(styleData.row).enumValues + "}"
                                                                                                                                                                       : "" )

                    verticalAlignment: Text.AlignVCenter

                    color: styleData.selected ? IngeScapeTheme.whiteColor : IngeScapeTheme.blackColor
                    font {
                        family: IngeScapeTheme.textFontFamily
                        //weight: Font.Medium
                        pixelSize: 12
                    }
                }
            }

            /*TableViewColumn {
                role: "enumValues"
                title: qsTr("")
            }*/
        }
    }


    //
    // Dependent Variables Panel
    //
    Rectangle {
        id: panelDepVar

        anchors {
            left: parent.left
            right: parent.right
            bottom: parent.bottom
        }
        height: parent.height / 2

        color: "transparent"
        border {
            width: 1
            color: "white"
        }

        Row {
            id: headerDepVar

            anchors {
                left: parent.left
                leftMargin: 10
                top: parent.top
                topMargin: 10
            }

            spacing: 20

            Text {
                text: "Dependent Variables"

                color: IngeScapeTheme.whiteColor
                font {
                    family: IngeScapeTheme.textFontFamily
                    weight: Font.Medium
                    pixelSize: 18
                }
            }

            Button {
                text: "New Dependent Variable"

                height: 30

                onClicked: {
                    console.log("QML: New Dependent Variable");

                    if (controller) {
                        controller.createNewDependentVariable();
                    }
                }
            }
        }

        TableView {
            id: tableDepVariables

            anchors {
                left: parent.left
                leftMargin: 10
                right: parent.right
                rightMargin: 10
                top: headerDepVar.bottom
                topMargin: 10
                bottom: parent.bottom
                bottomMargin: 10
            }

            rowDelegate: Rectangle {
                width: childrenRect.width
                height: styleData.selected ? 30 : 20

                color: styleData.selected ? "lightblue"
                                          : (styleData.alternate ? "lightgray" : "white")
            }

            model: rootItem.modelM ? rootItem.modelM.dependentVariables : null

            TableViewColumn {
                id: columnName

                role: "name"
                title: qsTr("Nom")

                width: tableDepVariables.width / 4.0

                delegate: VariableTextEditor {

                    variable: model ? model.QtObject : null

                    variableValue: styleData.value

                    //isSelected: styleData.selected

                    isCurrentlyEditing: (rootItem.indexDependentVariableCurrentlyEditing === styleData.row)


                    //
                    // Slots
                    //

                    /*onEditVariable: {
                        // Edit the dependent variable at the row index
                        rootItem.editDependentVariableAtRowIndex(styleData.row);
                    }

                    onStopEditionOfVariable: {
                        // Stop the current edition of the dependent variable
                        rootItem.stopCurrentEditionOfDependentVariable();
                    }*/

                    onVariableValueUpdated: {
                        if (model)
                        {
                            //console.log("QML: on (Dependent) Variable Value Updated for 'Name' " + value);

                            // Update the name
                            model.name = value;
                        }
                    }
                }
            }

            TableViewColumn {
                id: columnDescription

                role: "description"
                title: qsTr("Description")

                width: tableDepVariables.width / 4.0

                delegate: VariableTextEditor {

                    variable: model ? model.QtObject : null

                    variableValue: styleData.value

                    //isSelected: styleData.selected

                    isCurrentlyEditing: (rootItem.indexDependentVariableCurrentlyEditing === styleData.row)


                    //
                    // Slots
                    //
                    onVariableValueUpdated: {
                        if (model)
                        {
                            //console.log("QML: on (Dependent) Variable Value Updated for 'Description' " + value);

                            // Update the description
                            model.description = value;
                        }
                    }
                }
            }

            TableViewColumn {
                id: columnAgentName

                role: "agentName"
                title: qsTr("Agent")

                width: tableDepVariables.width / 4.0

                delegate: VariableComboBoxEditor {

                    variable: model ? model.QtObject : null

                    variableValue: styleData.value

                    //isSelected: styleData.selected

                    isCurrentlyEditing: (rootItem.indexDependentVariableCurrentlyEditing === styleData.row)

                    models: rootItem.modelM ? rootItem.modelM.agentNamesList : []


                    //
                    // Slots
                    //
                    onVariableValueUpdated: {
                        if (model)
                        {
                            console.log("QML: on (Dependent) Variable Value Updated for 'Agent Name' " + value);

                            // Update the agent name
                            model.agentName = value;
                        }
                    }
                }
            }

            TableViewColumn {
                id: columnOutputName

                role: "outputName"
                title: qsTr("Sortie")

                width: tableDepVariables.width / 4.0

                delegate: VariableComboBoxEditor {

                    variable: model ? model.QtObject : null

                    variableValue: styleData.value

                    //isSelected: styleData.selected

                    isCurrentlyEditing: (rootItem.indexDependentVariableCurrentlyEditing === styleData.row)

                    models: model ? model.outputNamesList : []


                    //
                    // Slots
                    //
                    onVariableValueUpdated: {
                        if (model)
                        {
                            console.log("QML: on (Dependent) Variable Value Updated for 'Output Name' " + value);

                            // Update the output name
                            model.outputName = value;
                        }
                    }
                }
            }

            onDoubleClicked: {
                //console.log("onDoubleClicked " + row);

                // Edit the dependent variable at the row index
                editDependentVariableAtRowIndex(row);
            }

            // NOT Called
            //onSelectionChanged: {
            //}
            // --> Workaround
            Connections {
                target: tableDepVariables.selection

                onSelectionChanged: {
                    //console.log("onSelectionChanged:");

                    if ((rootItem.indexPreviousSelectedDependentVariable === rootItem.indexDependentVariableCurrentlyEditing)
                            && (rootItem.indexDependentVariableCurrentlyEditing > -1))
                    {
                        // Stop the current edition of the dependent variable
                        stopCurrentEditionOfDependentVariable();
                    }

                    // Reset
                    rootItem.indexPreviousSelectedDependentVariable = -1;

                    tableDepVariables.selection.forEach(function(rowIndex) {
                        //console.log("row " + rowIndex + " is selected");

                        // Update
                        rootItem.indexPreviousSelectedDependentVariable = rowIndex;
                    })
                }
            }
        }
    }


    //
    // Create Characteristic Popup
    //
    Popup.CreateIndependentVariablePopup {
        id: createIndependentVariablePopup

        //anchors.centerIn: parent

        controller: rootItem.controller
    }

}
