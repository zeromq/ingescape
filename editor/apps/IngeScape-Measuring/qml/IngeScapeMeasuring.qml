/*
 *	IngeScape Measuring
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
import QtQuick.Controls 2.2
import QtQuick.Controls.Styles 1.4

import I2Quick 1.0
import QtQuick.Window 2.3

import INGESCAPE 1.0

import "subject" as Subject
import "task" as Task

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



    //--------------------------------------------------------
    //
    //
    // Slots
    //
    //
    //--------------------------------------------------------

    Connections {
        target: IngeScapeMeasuringC.modelManager

        onCurrentExperimentationChanged: {

            if (IngeScapeMeasuringC.modelManager.currentExperimentation)
            {
                //console.log("QML: on Current Experimentation changed: " + IngeScapeMeasuringC.modelManager.currentExperimentation.name);

                stackview.push(componentExperimentationView);
            }
            /*else {
                console.log("QML: on Current Experimentation changed to NULL");
            }*/
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

        /*pushEnter: Transition {
            PropertyAnimation {
                property: "opacity"
                from: 0
                to:1
                duration: 200
            }
        }
        pushExit: Transition {
            PropertyAnimation {
                property: "opacity"
                from: 1
                to:0
                duration: 200
            }
        }
        popEnter: Transition {
            PropertyAnimation {
                property: "opacity"
                from: 0
                to:1
                duration: 200
            }
        }
        popExit: Transition {
            PropertyAnimation {
                property: "opacity"
                from: 1
                to:0
                duration: 200
            }
        }*/

        initialItem: componentExperimentationsListView
    }



    //
    // Experimentations List View
    //
    Component {
        id: componentExperimentationsListView

        ExperimentationsListView {
            id: experimentationsListView

            controller: IngeScapeMeasuringC.experimentationsListC
        }
    }


    //
    // Experimentations View
    //
    Component {
        id: componentExperimentationView

        ExperimentationView {
            id: experimentationView

            controller: IngeScapeMeasuringC.experimentationC
            modelManager: IngeScapeMeasuringC.modelManager

            //
            // Slots
            //

            onGoBackToHome: {
                //console.log("QML: on Go Back To Home");

                stackview.pop();

                // Reset the current experimentation
                if (IngeScapeMeasuringC.modelManager)
                {
                    IngeScapeMeasuringC.modelManager.currentExperimentation = null;
                    IngeScapeMeasuringC.modelManager.currentExperimentationsGroup = null;
                }
            }
        }
    }


    //
    // Subjects View
    //
    /*Subject.SubjectsView {
        id: subjectsView
    }*/


    //
    // Tasks View
    //
    /*Task.TasksView {
        id: tasksView
    }*/


    //
    // Network Configuration (Popup)
    //
    /*NetworkConfiguration {
        id: networkConfigurationPopup

        anchors.centerIn: parent
    }*/


}
