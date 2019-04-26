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
import "../subject" as Subject
import "../task" as Task


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

    property ExperimentationController controller: null;

    property AssessmentsModelManager modelManager: null;

    property ExperimentationM experimentation: controller ? controller.currentExperimentation : null;



    //--------------------------------
    //
    //
    // Signals
    //
    //
    //--------------------------------

    // Go back to "Home"
    signal goBackToHome();



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

        color: "#FF336633"
    }

    Button {
        id: btnGoBackToHome

        anchors {
            left: parent.left
            top: parent.top
        }
        height: 30

        text: "HOME"

        onClicked: {
            console.log("QML: Go back to 'Home'");

            // Emit the signal the "Go Back To Home"
            rootItem.goBackToHome();
        }
    }

    Column {
        id: header

        anchors {
            top: parent.top
            topMargin: 10
            horizontalCenter: parent.horizontalCenter
        }
        spacing: 10

        Text {
            //id: title

            anchors {
                horizontalCenter: parent.horizontalCenter
            }

            text: rootItem.experimentation ? rootItem.experimentation.name : ""

            color: IngeScapeTheme.whiteColor
            font {
                family: IngeScapeTheme.textFontFamily
                weight: Font.Medium
                pixelSize: 18
            }
        }

        Text {
            //id: title

            anchors {
                horizontalCenter: parent.horizontalCenter
            }

            text: (rootItem.modelManager && rootItem.modelManager.currentExperimentationsGroup) ? rootItem.modelManager.currentExperimentationsGroup.name : ""

            color: IngeScapeTheme.whiteColor
            font {
                family: IngeScapeTheme.textFontFamily
                weight: Font.Medium
                pixelSize: 16
            }
        }

        Text {
            //id: title

            anchors {
                horizontalCenter: parent.horizontalCenter
            }

            text: rootItem.experimentation ? rootItem.experimentation.creationDate.toLocaleString(Qt.locale(), "dd/MM/yyyy hh:mm:ss") : ""

            color: IngeScapeTheme.whiteColor
            font {
                family: IngeScapeTheme.textFontFamily
                weight: Font.Medium
                pixelSize: 16
            }
        }
    }


    //
    // Main view
    //
    StackView {
        id: stackview

        anchors {
            top: header.bottom
            topMargin: 20
            bottom: parent.bottom
            left: parent.left
            right: parent.right
        }

        initialItem: componentMainView

        delegate: StackViewDelegate {

            pushTransition: StackViewTransition {
                PropertyAnimation {
                    target: enterItem
                    property: "y"
                    from: target.height
                    to: 0
                    duration: 250
                }
                /*PropertyAnimation {
                    target: exitItem
                    property: "y"
                    from: 0
                    to: 0
                    duration: 250
                }*/
            }

            popTransition: StackViewTransition {
                /*PropertyAnimation {
                    target: enterItem
                    property: "y"
                    from: 0
                    to: 0
                    duration: 250
                }*/
                PropertyAnimation {
                    target: exitItem
                    property: "y"
                    from: 0
                    to: target.height
                    duration: 250
                }
            }
        }
    }


    Component {
        id: componentMainView

        Item {
            id: mainView

            //
            // Configuration Panel
            //
            Rectangle {
                id: configurationPanel

                anchors {
                    top: parent.top
                    bottom: parent.bottom
                    left: parent.left
                }
                width: 150

                color: "#44AAAAAA"

                Column {

                    spacing: 10

                    Button {
                        text: "Subjects"

                        width: 100
                        height: 30

                        onClicked: {
                            console.log("QML: Add the 'Subjects View' to the stack");

                            // Add the "Subjects View" to the stack
                            stackview.push(componentSubjectsView);
                        }
                    }

                    Button {
                        text: "Tasks"

                        width: 100
                        height: 30

                        onClicked: {
                            console.log("QML: Add the 'Tasks View' to the stack");

                            // Add the "Tasks View" to the stack
                            stackview.push(componentTasksView);
                        }
                    }

                    Button {
                        text: "Coding"

                        width: 100
                        height: 30

                        enabled: false
                    }

                    Button {
                        text: "Clean"

                        width: 100
                        height: 30

                        enabled: false
                    }

                    Button {
                        text: "Export"

                        width: 100
                        height: 30

                        onClicked: {
                            console.log("QML: Export...");
                        }
                    }
                }
            }


            //
            // Records Panel
            //
            Rectangle {
                id: recordsPanel

                anchors {
                    top: parent.top
                    bottom: parent.bottom
                    left: configurationPanel.right
                    right: parent.right
                }
                color: "#44222222"

                Row {
                    id: recordsHeader

                    spacing: 20

                    Text {
                        id: titleRecords

                        text: qsTr("Records")

                        color: IngeScapeTheme.whiteColor
                        font {
                            family: IngeScapeTheme.textFontFamily
                            weight: Font.Medium
                            pixelSize: 18
                        }
                    }

                    Button {
                        id: btnNewRecord

                        height: 30

                        text: "New Record"

                        onClicked: {
                            // Open the popup
                            createRecordPopup.open();
                        }
                    }
                }


                Column {

                    anchors {
                        top: recordsHeader.bottom
                        topMargin: 20
                        left: parent.left
                        right: parent.right
                    }

                    Repeater {
                        model: rootItem.experimentation ? rootItem.experimentation.allRecords : null

                        delegate: RecordInList {

                            modelM: model.QtObject

                            //
                            // Slots
                            //
                            onOpenRecord: {
                                if (rootItem.controller && modelM) {
                                    //console.log("QML: onOpenRecord " + modelM.name);

                                    rootItem.controller.openRecord(modelM);
                                }
                            }

                            onDeleteRecord: {
                                if (rootItem.controller && modelM) {
                                    //console.log("QML: onDeleteRecord " + modelM.name);

                                    rootItem.controller.deleteRecord(modelM);
                                }
                            }
                        }
                    }
                }
            }


            //
            // Create Experimentation Popup
            //
            Popup.CreateRecordPopup {
                id: createRecordPopup

                //anchors.centerIn: parent

                controller: rootItem.controller
                experimentation: rootItem.experimentation
            }

        }
    }


    //
    // Subjects View
    //
    Component {
        id: componentSubjectsView

        Subject.SubjectsView {
            id: subjectsView

            controller: IngeScapeAssessmentsC.subjectsC


            //
            // Slots
            //

            onCloseSubjectsView: {
                console.log("QML: on Close Subjects view");

                // Remove the "Subjects View" from the stack
                stackview.pop();
            }
        }
    }


    //
    // Tasks View
    //
    Component {
        id: componentTasksView

        Task.TasksView {
            id: tasksView

            controller: IngeScapeAssessmentsC.tasksC


            //
            // Slots
            //

            onCloseTasksView: {
                console.log("QML: on Close Tasks view");

                // Remove the "Tasks View" from the stack
                stackview.pop();
            }
        }
    }

}
