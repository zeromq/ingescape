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
import "record" as Record


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
        target: IngeScapeAssessmentsC.modelManager
        //target: IngeScapeAssessmentsC.experimentationC

        onCurrentExperimentationChanged: {

            if (IngeScapeAssessmentsC.modelManager.currentExperimentation)
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
        target: IngeScapeAssessmentsC.experimentationC.recordC

        onCurrentRecordChanged: {

            if (IngeScapeAssessmentsC.experimentationC.recordC.currentRecord)
            {
                console.log("QML: on Current Record changed: " + IngeScapeAssessmentsC.experimentationC.recordC.currentRecord.name);

                // Add the "Record View" to the stack
                stackview.push(componentRecordView);
            }
            else {
                console.log("QML: on Current Record changed to NULL");

                // Remove the "Record View" from the stack
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
            modelManager: IngeScapeAssessmentsC.modelManager


            //
            // Slots
            //

            onGoBackToHome: {
                console.log("QML: on Go Back to 'Home'");

                // Reset the current experimentation
                if (IngeScapeAssessmentsC.modelManager)
                {
                    IngeScapeAssessmentsC.modelManager.currentExperimentation = null;
                    IngeScapeAssessmentsC.modelManager.currentExperimentationsGroup = null;
                }
            }
        }
    }


    //
    // Record View
    //
    Component {
        id: componentRecordView

        Record.RecordView {
            //id: recordView

            controller: IngeScapeAssessmentsC.experimentationC.recordC
            //modelManager: IngeScapeAssessmentsC.modelManager


            //
            // Slots
            //

            onGoBackToExperimentation: {
                console.log("QML: on Go Back to 'Experimentation'");

                // Reset the current record
                if (IngeScapeAssessmentsC.experimentationC && IngeScapeAssessmentsC.experimentationC.recordC)
                {
                    IngeScapeAssessmentsC.experimentationC.recordC.currentRecord = null;
                }
            }
        }
    }



    //
    // Network Configuration (Popup)
    //
    /*NetworkConfiguration {
        id: networkConfigurationPopup

        anchors.centerIn: parent
    }*/


}
