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


    /*SwipeView {
        id: swipeView

        anchors.fill: parent

        currentIndex: 0


        //
        // Experimentations List View
        //
        ExperimentationsListView {
            id: experimentationsListView

            controller: IngeScapeMeasuringC.experimentationsListC
        }


        //
        // Experimentations View
        //
        ExperimentationView {
            id: experimentationView
        }


        //
        // Subjects View
        //
        Subject.SubjectsView {
            id: subjectsView
        }


        //
        // Tasks View
        //
        Task.TasksView {
            id: tasksView
        }

    }*/


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
        }
    }



    Row {
        anchors {
            top: parent.top
            left: parent.left
        }
        Button {
            text: "PREVIOUS"

            onClicked: {
                console.log("PREVIOUS");

                //swipeView.currentIndex--;

                stackview.pop();
            }
        }

        Button {
            text: "NEXT"

            onClicked: {
                console.log("NEXT");

                //swipeView.currentIndex++;

                stackview.push(componentExperimentationView);
            }
        }
    }


    //
    // Left panel
    //
    /*Rectangle {
        id: leftPanel

        width: IngeScapeTheme.leftPanelWidth

        anchors {
            left: parent.left
            top: parent.top
            bottom: parent.bottom
        }

        color: IngeScapeTheme.blackColor

        I2CustomRectangle {
            anchors {
                fill: parent
                topMargin: 9
            }
            color: IngeScapeTheme.leftPanelBackgroundColor

            fuzzyRadius: 8
            topRightRadius : 5

            borderWidth: 1
            borderColor: IngeScapeTheme.selectedTabsBackgroundColor

        }
    }*/



    //
    // Network Configuration (Popup)
    //
    /*NetworkConfiguration {
        id: networkConfigurationPopup

        anchors.centerIn: parent
    }*/


}
