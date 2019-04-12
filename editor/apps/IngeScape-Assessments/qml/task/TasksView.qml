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

    //anchors.fill: parent


    //--------------------------------------------------------
    //
    //
    // Properties
    //
    //
    //--------------------------------------------------------

    property TasksController controller: null;

    property ExperimentationM experimentation: controller ? controller.currentExperimentation : null;

    property TaskM selectedTak: null;


    //--------------------------------
    //
    //
    // Signals
    //
    //
    //--------------------------------

    // Close Tasks view
    signal closeTasksView();



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

        color: "#FF333366"
    }

    Button {
        id: btnClose

        anchors {
            right: parent.right
            top: parent.top
        }
        height: 30

        text: "X"

        onClicked: {
            //console.log("QML: close Tasks view");

            // Emit the signal "closeTasksView"
            rootItem.closeTasksView();
        }
    }

    Row {
        id: header

        anchors {
            left: parent.left
            leftMargin: 10
            top: parent.top
            topMargin: 10
        }

        spacing: 20

        Text {
            id: title

            text: "Tasks"

            color: IngeScapeTheme.whiteColor
            font {
                family: IngeScapeTheme.textFontFamily
                weight: Font.Medium
                pixelSize: 20
            }
        }

        Button {
            text: "New Task"

            height: 30

            onClicked: {
                console.log("QML: New Task");

                // Open the popup
                createTaskPopup.open();
            }
        }
    }

    ListView {

        id: listOfTasks

        anchors {
            top: header.bottom
            topMargin: 20
            bottom: parent.bottom
            bottomMargin: 5
            left: parent.left
            leftMargin: 5
        }
        width: 300

        model: rootItem.experimentation ? rootItem.experimentation.allTasks : null

        delegate: TaskInList {

            modelM: model.QtObject

            isSelected: ListView.isCurrentItem

            //
            // Slots
            //
            onSelectTask: {
                //console.log("QML: on Select Task " + model.name + " at " + index);

                // First, select the task
                rootItem.selectedTak = model.QtObject;

                // Then, set the index
                listOfTasks.currentIndex = index;
            }

            onDeleteTask: {
                if (rootItem.controller) {
                    console.log("QML: on Delete Task " + model.name);

                    // First, un-select the task
                    rootItem.selectedTak = null;

                    // Then, reset the index
                    listOfTasks.currentIndex = -1;

                    // Delete the task
                    rootItem.controller.deleteTask(model.QtObject);
                }
            }

            onDuplicateTask: {
                if (rootItem.controller) {
                    console.log("QML: on Duplicate Task " + model.name);
                }
            }
        }

        onCurrentIndexChanged: {
            // If the index is defined but if the selected task is null, we have to select the corresponding task
            if ((currentIndex > -1) && (rootItem.selectedTak === null)
                    && rootItem.experimentation && (currentIndex < rootItem.experimentation.allTasks.count))
            {
                //console.log("QML: Must select task at " + currentIndex);
                rootItem.selectedTak = rootItem.experimentation.allTasks.get(currentIndex);
            }
        }

    }


    //
    // Task
    //
    Task {
        id: task

        anchors {
            top: header.bottom
            topMargin: 20
            bottom: parent.bottom
            bottomMargin: 5
            left: listOfTasks.right
            leftMargin: 0
            right: parent.right
            rightMargin: 5
        }

        modelM: rootItem.selectedTak
    }


    //
    // Create Task@ Popup
    //
    Popup.CreateTaskPopup {
        id: createTaskPopup

        //anchors.centerIn: parent

        controller: rootItem.controller
    }

}
