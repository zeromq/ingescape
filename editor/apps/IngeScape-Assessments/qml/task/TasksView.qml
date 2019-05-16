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


    //--------------------------------
    //
    //
    // Signals
    //
    //
    //--------------------------------

    // Close Tasks view
    signal closeTasksView();



    //--------------------------------
    //
    //
    // Slots
    //
    //
    //--------------------------------

    /*Connections {
        target: rootItem.controller

        onSelectedTaskChanged: {
            if (rootItem.controller.selectedTask)
            {
                console.log("QML: on Selected Task changed to " + rootItem.controller.selectedTask.name);
            }
            else {
                console.log("QML: on Selected Task changed to NULL");
            }
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

        anchors.fill: parent

        color: IngeScapeTheme.veryDarkGreyColor
        border {
            color: IngeScapeTheme.darkGreyColor
            width: 1
        }
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

            if (controller) {
                controller.selectedTask = null;
            }

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
        height: 30

        spacing: 20

        Text {
            id: title

            text: "Tasks"

            height: parent.height
            verticalAlignment: Text.AlignVCenter

            color: IngeScapeTheme.whiteColor
            font {
                family: IngeScapeTheme.textFontFamily
                weight: Font.Medium
                pixelSize: 20
            }
        }

        Button {
            text: "New Task"

            height: parent.height

            onClicked: {
                //console.log("QML: New Task");

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

        // In front of the Task (main view)
        z: 1

        model: rootItem.experimentation ? rootItem.experimentation.allTasks : null

        delegate: TaskInList {

            modelM: model.QtObject

            //isSelected: ListView.isCurrentItem
            isSelected: rootItem.controller && rootItem.controller.selectedTask && (modelM === rootItem.controller.selectedTask)


            //
            // Slots
            //
            onSelectTask: {
                if (rootItem.controller)
                {
                    //console.log("QML: on Select Task " + model.name + " at " + index);

                    // First, select the task
                    rootItem.controller.selectedTask = model.QtObject;
                }
            }

            onDeleteTask: {
                if (rootItem.controller)
                {
                    //console.log("QML: on Delete Task " + model.name);

                    // First, un-select the task
                    rootItem.controller.selectedTask = null;

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
            leftMargin: -1
            right: parent.right
            rightMargin: 5
        }
        // Behind the list of tasks
        z: 0

        controller: rootItem.controller
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