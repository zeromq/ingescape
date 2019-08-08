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
import QtQuick.Window 2.3

import INGESCAPE 1.0

import "experimentation" as Experimentation
import "task_instance" as TaskInstance


Item {
    id: rootItem

    anchors.fill: parent


    //--------------------------------------------------------
    //
    //
    // Properties
    //
    //
    //--------------------------------------------------------



    //--------------------------------------------------------
    //
    //
    // Functions
    //
    //
    //--------------------------------------------------------

    /**
     * Reset the current experimentation
     */
    function resetCurrentExperimentation()
    {
        if (IngeScapeAssessmentsC.experimentationC)
        {
            IngeScapeAssessmentsC.experimentationC.currentExperimentation = null;
        }
    }

    /**
     * Reset the current task instance
     */
    function resetCurrentTaskInstance()
    {
        if (IngeScapeAssessmentsC.experimentationC && IngeScapeAssessmentsC.experimentationC.taskInstanceC)
        {
            IngeScapeAssessmentsC.experimentationC.taskInstanceC.currentTaskInstance = null;
        }
    }


    //--------------------------------------------------------
    //
    //
    // Slots
    //
    //
    //--------------------------------------------------------

    Connections {
        target: IngeScapeAssessmentsC.experimentationC

        onCurrentExperimentationChanged: {
            if (IngeScapeAssessmentsC.experimentationC.currentExperimentation)
            {
                //console.log("QML: on Current Experimentation changed: " + IngeScapeAssessmentsC.modelManager.currentExperimentation.name);

                // Add the "Experimentation View" to the stack
                stackview.push(componentExperimentationView);
            }
            else {
                console.log("QML: on Current Experimentation changed to NULL");

                // Remove the "Experimentation View" from the stack
                stackview.pop();
            }
        }
    }

    Connections {
        target: IngeScapeAssessmentsC.experimentationC.taskInstanceC

        onCurrentTaskInstanceChanged: {

            if (IngeScapeAssessmentsC.experimentationC.taskInstanceC.currentTaskInstance)
            {
                console.log("QML: on Current Task Instance changed: " + IngeScapeAssessmentsC.experimentationC.taskInstanceC.currentTaskInstance.name);

                // Add the "Task Instance View" to the stack
                stackview.push(componentTaskInstanceView);
            }
            else {
                console.log("QML: on Current Task Instance changed to NULL");

                // Remove the "Task Instance iew" from the stack
                stackview.pop();
            }
        }
    }


    //--------------------------------------------------------
    //
    //
    // Content
    //
    //
    //--------------------------------------------------------

    StackView {
        id: stackview

        anchors.fill: parent

        initialItem: componentExperimentationsListView
    }



    //
    // Experimentations List View
    //
    Component {
        id: componentExperimentationsListView

        ExperimentationsListView {
            id: experimentationsListView

            controller: IngeScapeAssessmentsC.experimentationsListC
        }
    }


    //
    // Experimentation View
    //
    Component {
        id: componentExperimentationView

        Experimentation.ExperimentationView {
            //id: experimentationView

            controller: IngeScapeAssessmentsC.experimentationC

            //
            // Slots
            //

            onGoBackToHome: {
                console.log("QML: on Go Back to 'Home' (from 'Experimentation' view)");

                // Reset the current experimentation
                rootItem.resetCurrentExperimentation();
            }
        }
    }


    //
    // Task Instance View
    //
    Component {
        id: componentTaskInstanceView

        TaskInstance.TaskInstanceView {
            taskInstanceController: IngeScapeAssessmentsC.experimentationC.taskInstanceC

            //
            // Slots
            //

            onGoBackToHome: {
                console.log("QML: on Go Back to 'Home' (from 'TaskInstance' view)");

                // Reset the current record
                rootItem.resetCurrentTaskInstance();

                // Reset the current experimentation
                rootItem.resetCurrentExperimentation();
            }

            onGoBackToExperimentation: {
                console.log("QML: on Go Back to 'Experimentation' (from 'TaskInstance' view)");

                // Reset the current task instance
                rootItem.resetCurrentTaskInstance();
            }
        }
    }
}
