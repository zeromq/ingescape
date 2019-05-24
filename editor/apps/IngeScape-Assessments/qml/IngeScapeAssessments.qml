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

    /**
     * Reset the current experimentation
     */
    function resetCurrentExperimentation()
    {
        if (IngeScapeAssessmentsC.modelManager)
        {
            IngeScapeAssessmentsC.modelManager.currentExperimentation = null;
            IngeScapeAssessmentsC.modelManager.currentExperimentationsGroup = null;
        }
    }

    /**
     * Reset the current record setup
     */
    function resetCurrentRecordSetup()
    {
        if (IngeScapeAssessmentsC.experimentationC && IngeScapeAssessmentsC.experimentationC.recordC)
        {
            IngeScapeAssessmentsC.experimentationC.recordC.currentRecordSetup = null;
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

        onCurrentRecordSetupChanged: {

            if (IngeScapeAssessmentsC.experimentationC.recordC.currentRecordSetup)
            {
                console.log("QML: on Current Record Setup changed: " + IngeScapeAssessmentsC.experimentationC.recordC.currentRecordSetup.name);

                // Add the "Record View" to the stack
                stackview.push(componentRecordView);
            }
            else {
                console.log("QML: on Current Record Setup changed to NULL");

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
                console.log("QML: on Go Back to 'Home' (from 'Experimentation' view)");

                // Reset the current experimentation
                rootItem.resetCurrentExperimentation();
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

            onGoBackToHome: {
                console.log("QML: on Go Back to 'Home' (from 'Record' view)");

                // Reset the current record
                rootItem.resetCurrentRecord();

                // Reset the current experimentation
                rootItem.resetCurrentExperimentation();
            }

            onGoBackToExperimentation: {
                console.log("QML: on Go Back to 'Experimentation' (from 'Record' view)");

                // Reset the current record setup
                rootItem.resetCurrentRecordSetup();
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
