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
import "../export" as Export


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

        color: IngeScapeTheme.veryLightGreyColor
    }

    Item {
        id: headerItem

        anchors {
            top: parent.top
            left: parent.left
            right: parent.right
        }

        height: 110

        Button {
            id: btnGoBackToHome

            anchors {
                left: parent.left
                leftMargin: 26
                top: parent.top
                topMargin: 20
            }

            height: 42
            width: 42

            text: "<--"

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
                topMargin: 30
                left: parent.left
                leftMargin: 92
            }

            spacing: 10

            Text {
                text: rootItem.experimentation ? rootItem.experimentation.name : ""

                verticalAlignment: Text.AlignVCenter
                horizontalAlignment: Text.AlignHCenter

                //FIXME Proper font
                color: IngeScapeTheme.blueButton
                font {
                    family: IngeScapeTheme.textFontFamily
                    weight: Font.Medium
                    pixelSize: 22
                    bold: true
                }
            }

            Row {
                spacing: 0

                Text {
                    id: expeGroupNameText
                    text: rootItem.experimentation ? rootItem.experimentation.groupName : ""

                    verticalAlignment: Text.AlignVCenter
                    horizontalAlignment: Text.AlignHCenter

                    //FIXME Proper font
                    color: IngeScapeTheme.blackColor
                    font {
                        family: IngeScapeTheme.textFontFamily
                        weight: Font.Medium
                        pixelSize: 20
                    }
                }

                Text {
                    height: expeGroupNameText.height

                    text: rootItem.experimentation ? rootItem.experimentation.creationDate.toLocaleString(Qt.locale(), " - dd/MM/yyyy - hh:mm:ss") : ""

                    verticalAlignment: Text.AlignVCenter
                    horizontalAlignment: Text.AlignHCenter

                    //FIXME Proper font
                    color: IngeScapeTheme.blackColor
                    font {
                        family: IngeScapeTheme.textFontFamily
                        weight: Font.Medium
                        pixelSize: 16
                        italic: true
                    }
                }
            }
        }
    }

    Rectangle {
        anchors {
            top: headerItem.bottom
            left: headerItem.left
            right: headerItem.right
        }

        height: 4

        gradient: Gradient {
            GradientStop { position: 0.0; color: IngeScapeTheme.blackColor; }
            GradientStop { position: 1.0; color: IngeScapeTheme.whiteColor; }
        }

    }

    //
    // Main view
    //
    StackView {
        id: stackview

        anchors {
            top: headerItem.bottom
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
                width: 120

                color: "transparent"
                border {
                    color: IngeScapeTheme.darkGreyColor
                    width: 1
                }

                Column {

                    anchors {
                        left: parent.left
                        leftMargin: 10
                        top: parent.top
                        topMargin: 10
                    }

                    width: 100

                    spacing: 10

                    Button {
                        text: "Subjects"

                        width: parent.width
                        height: 30

                        onClicked: {
                            console.log("QML: Add the 'Subjects View' to the stack");

                            // Add the "Subjects View" to the stack
                            stackview.push(componentSubjectsView);
                        }
                    }

                    Button {
                        text: "Tasks"

                        width: parent.width
                        height: 30

                        onClicked: {
                            console.log("QML: Add the 'Tasks View' to the stack");

                            // Add the "Tasks View" to the stack
                            stackview.push(componentTasksView);
                        }
                    }

                    Button {
                        text: "Coding"

                        width: parent.width
                        height: 30

                        enabled: false
                    }

                    Button {
                        text: "Clean"

                        width: parent.width
                        height: 30

                        enabled: false
                    }

                    Button {
                        text: "Export"

                        width: parent.width
                        height: 30

                        onClicked: {
                            console.log("QML: Add the 'Export View' to the stack");

                            // Add the "Export View" to the stack
                            stackview.push(componentExportView);
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

                color: "transparent"
                border {
                    color: IngeScapeTheme.darkGreyColor
                    width: 1
                }

                Row {
                    id: recordsHeader

                    anchors {
                        left: parent.left
                        leftMargin: 10
                        top: parent.top
                        topMargin: 10
                    }
                    height: 30

                    spacing: 20

                    Text {
                        id: titleRecords

                        text: qsTr("Records")

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
                        id: btnNewRecord

                        height: parent.height

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
                        leftMargin: 10
                        right: parent.right
                        rightMargin: 10
                    }

                    Repeater {
                        model: rootItem.experimentation ? rootItem.experimentation.allRecordSetups : null

                        delegate: RecordSetupInList {

                            modelM: model.QtObject

                            //
                            // Slots
                            //
                            onOpenRecordSetup: {
                                if (rootItem.controller && modelM) {
                                    //console.log("QML: openRecordSetup " + modelM.name);

                                    rootItem.controller.openRecordSetup(modelM);
                                }
                            }

                            onDeleteRecordSetup: {
                                if (rootItem.controller && modelM) {
                                    //console.log("QML: deleteRecordSetup " + modelM.name);

                                    rootItem.controller.deleteRecordSetup(modelM);
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


    //
    // Export View
    //
    Component {
        id: componentExportView

        Export.ExportView {
            id: exportView

            controller: IngeScapeAssessmentsC.exportC


            //
            // Slots
            //

            onCloseExportView: {
                console.log("QML: on Close Export view");

                // Remove the "Export View" from the stack
                stackview.pop();
            }
        }
    }

}
