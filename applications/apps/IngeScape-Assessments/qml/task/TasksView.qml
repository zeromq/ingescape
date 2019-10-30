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

    property TasksController taskController: null;

    property ExperimentationM experimentation: taskController ? taskController.currentExperimentation : null;


    //--------------------------------
    //
    //
    // Signals
    //
    //
    //--------------------------------

    // Close Tasks view
    //signal closeTasksView();



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

        radius: 5
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
            id: titleProtocols

            anchors {
                verticalCenter: btnNewTask.verticalCenter
                left: parent.left
                leftMargin: 28
            }

            text: qsTr("PROTOCOLS")

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
                createProtocolPopup.open();
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
                isSelected: rootItem.taskController && rootItem.taskController.selectedTask && (modelM === rootItem.taskController.selectedTask)

                //
                // Slots
                //
                onSelectTask: {
                    if (rootItem.taskController)
                    {
                        // First, select the task
                        rootItem.taskController.selectedTask = model.QtObject;
                    }
                }

                onDeleteTask: {
                    if (rootItem.taskController)
                    {
                        // Delete the task
                        rootItem.taskController.deleteTask(model.QtObject);
                    }
                }

                onDuplicateTask: {
                    if (rootItem.taskController)
                    {
                        // Duplicate the task
                        rootItem.taskController.duplicateTask(model.QtObject);
                    }
                }
            }
        }
    }

    //
    // Task
    //
    Rectangle {
        id: taskItem

        anchors {
            top: parent.top
            bottom: parent.bottom
            left: leftPart.right
            right: parent.right
            rightMargin: 5
        }

        color: IngeScapeTheme.veryLightGreyColor

        Rectangle {
            anchors {
                top: parent.top
                bottom: parent.bottom
                horizontalCenter: parent.right
            }

            width: 10

            color: IngeScapeTheme.veryLightGreyColor
            radius: 5
        }


        //
        // Task
        //
        Task {
            id: task

            anchors.fill: parent

            taskController: rootItem.taskController
        }


        //
        // Close button
        //
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
                if (taskController) {
                    taskController.selectedTask = null;
                }

                console.log("QML: close Tasks view");

                // Emit the signal "closeTasksView"
                //rootItem.closeTasksView();

                close();
            }
        }
    }


    //
    // Create Task Popup
    //
    Popup.CreateTaskPopup {
        id: createProtocolPopup

        layerObjectName: "overlay2Layer"

        taskController: rootItem.taskController
    }

}
