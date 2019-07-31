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
        height: 18
        width: 18

        style: IngeScapeAssessmentsSvgButtonStyle {
            releasedID: "close"
        }

        onClicked: {
            if (controller) {
                controller.selectedTask = null;
            }

            // Emit the signal "closeTasksView"
            rootItem.closeTasksView();
        }
    }

    Item {
        id: leftPart

        anchors {
            top: parent.top
            topMargin: 24
            left: parent.left
            bottom: parent.bottom
        }

        width: 340

        Text {
            id: titleCharacteristics

            anchors {
                verticalCenter: btnNewTask.verticalCenter
                left: parent.left
                leftMargin: 28
            }

            text: qsTr("TASKS")

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
            id: btnNewTask

            anchors {
                top: parent.top
                right: parent.right
            }

            height: 40
            width: 134

            onClicked: {
                // Open the popup
                createTaskPopup.open();
            }

            style: IngeScapeAssessmentsButtonStyle {
                text: qsTr("ADD NEW")
            }
        }

        Rectangle {
            id: topSeparator
            anchors {
                bottom: taskList.top
                left: parent.left
                right: parent.right
            }

            height: 1
            color: IngeScapeTheme.blackColor
        }

        ListView {
            id: taskList

            anchors {
                top: btnNewTask.bottom
                topMargin: 14
                left: parent.left
                right: parent.right
                bottom: parent.bottom
            }

            model: rootItem.experimentation ? rootItem.experimentation.allTasks : null

            delegate: TaskInList {

                modelM: model.QtObject

                isSelected: rootItem.controller && rootItem.controller.selectedTask && (modelM === rootItem.controller.selectedTask)


                //
                // Slots
                //
                onSelectTask: {
                    if (rootItem.controller)
                    {
                        // First, select the task
                        rootItem.controller.selectedTask = model.QtObject;
                    }
                }

                onDeleteTask: {
                    if (rootItem.controller)
                    {
                        // Delete the task
                        rootItem.controller.deleteTask(model.QtObject);
                    }
                }

                onDuplicateTask: {
                    if (rootItem.controller)
                    {
                        // Duplicate the task
                        rootItem.controller.duplicateTask(model.QtObject);
                    }
                }
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
                    //rootItem.controller.selectedTask = null;

                    // Delete the task
                    rootItem.controller.deleteTask(model.QtObject);
                }
            }

            onDuplicateTask: {
                if (rootItem.controller)
                {
                    //console.log("QML: on Duplicate Task " + model.name);

                    // Duplicate the task
                    rootItem.controller.duplicateTask(model.QtObject);
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
            top: parent.top
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
