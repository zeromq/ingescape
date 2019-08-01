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


I2PopupBase {
    id: rootItem


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
        color: IngeScapeAssessmentsTheme.darkerDarkBlueHeader
    }

    Button {
        id: btnClose

        anchors {
            top: parent.top
            topMargin: 21
            right: parent.right
            rightMargin: 21
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
                family: IngeScapeTheme.labelFontFamily
                weight: Font.Black
                pixelSize: 20
            }
        }

        Button {
            id: btnNewTask

            anchors {
                top: parent.top
                right: parent.right
                rightMargin: 12
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

    //
    // Task
    //
    Task {
        id: task

        anchors {
            top: parent.top
            bottom: parent.bottom
            left: leftPart.right
            right: parent.right
        }

        taskController: rootItem.controller
    }


    //
    // Create Task Popup
    //
    Popup.CreateTaskPopup {
        id: createTaskPopup

        //anchors.centerIn: parent

        controller: rootItem.controller
    }

}
