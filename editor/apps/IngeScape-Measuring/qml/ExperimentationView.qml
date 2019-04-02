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
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4

import I2Quick 1.0

import INGESCAPE 1.0

//import "theme" as Theme
import "popup" as Popup
import "subject" as Subject
import "task" as Task


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

    property IngeScapeModelManager modelManager: null;

    property ExperimentationM experimentation: modelManager ? modelManager.currentExperimentation : null;



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
        id: btnGoBack

        anchors {
            left: parent.left
            top: parent.top
        }

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
                weight : Font.Medium
                pixelSize : 18
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
                weight : Font.Medium
                pixelSize : 16
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
                weight : Font.Medium
                pixelSize : 16
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
            topMargin: 50
            bottom: parent.bottom
            left: parent.left
            right: parent.right
        }

        initialItem: componentMainView
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
                width: 250

                color: "#44AAAAAA"

                Column {

                    Button {
                        text: "Subjects"

                        onClicked: {
                            console.log("QML: Add the 'Subjects View' to the stack");

                            // Add the "Subjects View" to the stack
                            stackview.push(componentSubjectsView);
                        }
                    }

                    Button {
                        text: "Tasks"

                        onClicked: {
                            console.log("QML: Add the 'Tasks View' to the stack");

                            // Add the "Tasks View" to the stack
                            stackview.push(componentTasksView);
                        }
                    }

                    Button {
                        text: "Coding"

                        enabled: false
                    }

                    Button {
                        text: "Clean"

                        enabled: false
                    }

                    Button {
                        text: "Export"

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
                            weight : Font.Medium
                            pixelSize : 14
                        }
                    }

                    Button {
                        id: btnNewRecord

                        text: "New Record"

                        onClicked: {
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

                        delegate: componentRecord
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

            //controller: IngeScapeMeasuringC.experimentationC
            //modelManager: IngeScapeMeasuringC.modelManager


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

            //controller: IngeScapeMeasuringC.experimentationC
            //modelManager: IngeScapeMeasuringC.modelManager


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


    //
    // Component for "Record (Model)"
    //
    Component {
        id: componentRecord

        Rectangle {
            id: rootRecord

            property RecordM record: model.QtObject

            width: parent.width
            height: 30

            color: "#44222222"
            border {
                color: "black"
                width: 1
            }

            Row {
                spacing: 30

                anchors {
                    left: parent.left
                    verticalCenter: parent.verticalCenter
                }

                Text {
                    text: rootRecord.record ? rootRecord.record.name : ""

                    color: IngeScapeTheme.whiteColor
                    font {
                        family: IngeScapeTheme.textFontFamily
                        weight : Font.Medium
                        pixelSize : 12
                    }
                }

                Text {
                    /*text: rootRecord.record ? rootRecord.record.startDateTime.toLocaleString(Qt.locale(), "dd/MM/yyyy hh:mm:ss")
                                            : "../../.... ..:..:.."*/
                    text: "../../.... ..:..:.."

                    color: IngeScapeTheme.whiteColor
                    font {
                        family: IngeScapeTheme.textFontFamily
                        weight : Font.Medium
                        pixelSize : 12
                    }
                }

                Text {
                    /*text: rootRecord.record ? rootRecord.record.duration.toLocaleString(Qt.locale(), "hh:mm:ss.zzz")
                                            : "00:00:00.000"*/
                    text: "00:00:00.000"

                    color: IngeScapeTheme.whiteColor
                    font {
                        family: IngeScapeTheme.textFontFamily
                        weight : Font.Medium
                        pixelSize : 12
                    }
                }
            }

            /*MouseArea {
                id: mouseAreaRecord

                anchors.fill: parent

                hoverEnabled: true
            }*/

            Row {
                spacing: 20

                anchors {
                    right: parent.right
                }
                height: parent.height

                //visible: mouseAreaRecord.containsMouse

                Button {
                    id: btnOpen

                    text: "Open"

                    width: 100
                    height: parent.height

                    onClicked: {
                        if (rootRecord.record && rootItem.controller)
                        {
                            console.log("QML: Open " + rootRecord.record.name);

                            // Open the record
                            rootItem.controller.openRecord(rootRecord.record);
                        }
                    }
                }

                Button {
                    id: btnDelete

                    text: "Delete"

                    width: 100
                    height: parent.height

                    onClicked: {
                        if (rootRecord.record && rootItem.controller)
                        {
                            console.log("QML: Delete " + rootRecord.record.name);

                            // Delete the record
                            rootItem.controller.deleteRecord(rootRecord.record);
                        }
                    }
                }
            }
        }
    }

}
