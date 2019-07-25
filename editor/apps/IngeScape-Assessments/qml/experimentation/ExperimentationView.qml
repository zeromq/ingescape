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

    property bool isEditingName: false

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

        MouseArea {
            id: expeNameMouseArea
            anchors {
                fill: expeNameEditBackground
            }

            hoverEnabled: true
        }

        Rectangle {
            id: expeNameEditBackground
            anchors {
                left: expeName.left
                leftMargin: -10
                verticalCenter: expeName.verticalCenter
            }

            radius: 5

            width: expeName.width + 76
            height: 40

            color: IngeScapeTheme.middleLightGreyColor

            visible: expeNameMouseArea.containsMouse || editButton.hovered || rootItem.isEditingName

            Button {
                id: editButton

                anchors {
                    right: parent.right
                    rightMargin: 5
                    verticalCenter: parent.verticalCenter
                }

                width: 42
                height: 30

                //FIXME Edition picto
                style: IngeScapeAssessmentsButtonStyle {
                    text: "E"
                }

                onClicked: {
                    rootItem.isEditingName = !rootItem.isEditingName
                    if (rootItem.isEditingName)
                    {
                        // Entering edition mode
                        expeNameEditionTextField.text = expeName.text;
                    }
                    else
                    {
                        // Exiting edition mode
                        if (rootItem.experimentation)
                        {
                            expeName.text = expeNameEditionTextField.text
                            rootItem.experimentation.name = expeName.text
                        }

                    }
                }
            }
        }

        Text {
            id: expeName
            anchors {
                top: parent.top
                topMargin: 30
                left: parent.left
                leftMargin: 92
            }

            text: rootItem.experimentation ? rootItem.experimentation.name : ""

            verticalAlignment: Text.AlignVCenter
            horizontalAlignment: Text.AlignHCenter

            visible: !rootItem.isEditingName

            //FIXME Proper font
            color: IngeScapeAssessmentsTheme.blueButton
            font {
                family: IngeScapeTheme.textFontFamily
                weight: Font.Medium
                pixelSize: 22
                bold: true
            }
        }

        //FIXME Style !!
        TextField {
            id: expeNameEditionTextField
            anchors.fill: expeName

            visible: rootItem.isEditingName
            enabled: visible
        }

        Row {
            anchors {
                top: expeName.bottom
                topMargin: 15
                left: expeName.left
            }

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

    Rectangle {
        id: headerBottomShadow

        anchors {
            top: headerItem.bottom
            left: headerItem.left
            right: headerItem.right
        }

        height: 6

        gradient: Gradient {
            GradientStop { position: 0.0; color: IngeScapeTheme.darkGreyColor; }
            GradientStop { position: 1.0; color: IngeScapeTheme.veryLightGreyColor; }
        }

    }


    Component {
        id: componentMainView

        Item {
            id: mainView

            //FIXME Minimum size not handled
            property real taskColumnWidth: 280
            property real subjectColumnWidth: 216
            property real startDateColumnWidth: 138
            property real startTimeColumnWidth: 138
            property real durationColumnWidth: 126
            property real buttonColumnWidth: 158
            property real recordNameColumnWidth: recordsPanel.width
                                                 - taskColumnWidth
                                                 - subjectColumnWidth
                                                 - startDateColumnWidth
                                                 - startTimeColumnWidth
                                                 - durationColumnWidth
                                                 - buttonColumnWidth

            //
            // Configuration Panel
            //
            Item {
                id: configurationPanel

                anchors {
                    top: parent.top
                    topMargin: 24
                    bottom: parent.bottom
                    left: parent.left
                    leftMargin: 26
                }

                width: 182

                Column {

                    anchors {
                        left: parent.left
                        right: parent.right
                        top: parent.top
                    }

                    spacing: 18

                    Button {
                        width: parent.width
                        height: 62

                        style: IngeScapeAssessmentsButtonStyle {
                            text: "SUBJECTS"
                        }

                        onClicked: {
                            console.log("QML: Add the 'Subjects View' to the stack");

                            // Add the "Subjects View" to the stack
                            stackview.push(componentSubjectsView);
                        }
                    }

                    Button {
                        width: parent.width
                        height: 62

                        style: IngeScapeAssessmentsButtonStyle {
                            text: "TASKS"
                        }

                        onClicked: {
                            console.log("QML: Add the 'Tasks View' to the stack");

                            // Add the "Tasks View" to the stack
                            stackview.push(componentTasksView);
                        }
                    }

                    Button {
                        width: parent.width
                        height: 62

                        enabled: false

                        style: IngeScapeAssessmentsButtonStyle {
                            text: "CODING"
                        }
                    }

                    Button {
                        width: parent.width
                        height: 62

                        enabled: false

                        style: IngeScapeAssessmentsButtonStyle {
                            text: "CLEANING"
                        }
                    }

                    Button {
                        width: parent.width
                        height: 62

                        style: IngeScapeAssessmentsButtonStyle {
                            text: "EXPORT"
                        }

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
            Item {
                id: recordsPanel

                anchors {
                    top: parent.top
                    topMargin: 34
                    bottom: parent.bottom
                    bottomMargin: 49
                    left: configurationPanel.right
                    leftMargin: 30
                    right: parent.right
                    rightMargin: 28
                }

                Text {
                    id: titleRecords

                    anchors {
                        verticalCenter: btnNewRecord.verticalCenter
                        left: parent.left
                    }

                    text: qsTr("RECORDS")

                    height: parent.height
                    verticalAlignment: Text.AlignVCenter

                    color: IngeScapeTheme.blackColor
                    font {
                        family: IngeScapeTheme.textFontFamily
                        weight: Font.Medium
                        pixelSize: 20
                    }
                }

                Button {
                    id: btnNewRecord

                    anchors {
                        top: parent.top
                        right: parent.right
                    }

                    height: 39
                    width: 182

                    onClicked: {
                        // Open the popup
                        createRecordPopup.open();
                    }

                    //FIXME correct font
                    style: IngeScapeAssessmentsButtonStyle {
                        text: qsTr("NEW RECORD")
                    }
                }

                Rectangle {

                    anchors {
                        top: btnNewRecord.bottom
                        topMargin: 12
                        bottom: parent.bottom
                        left: parent.left
                        right: parent.right
                    }

                    color: IngeScapeTheme.whiteColor

                    Column {
                        id: recordsColumn
                        anchors.fill: parent
                        spacing: 0

                        IngeScapeAssessmentsListHeader {
                            anchors {
                                left: parent.left
                                right: parent.right
                            }

                            Row {
                                anchors {
                                    top: parent.top
                                    left: parent.left
                                    leftMargin: 15
                                    right: parent.right
                                    bottom: parent.bottom
                                }

                                Text {
                                    anchors {
                                        verticalCenter: parent.verticalCenter
                                    }

                                    width: mainView.recordNameColumnWidth

                                    text: qsTr("Record's name")
                                    color: IngeScapeTheme.whiteColor
                                    font {
                                        family: IngeScapeTheme.labelFontFamily
                                        pixelSize: 16
                                        weight: Font.Black
                                    }
                                }

                                Text {
                                    anchors {
                                        verticalCenter: parent.verticalCenter
                                    }

                                    width: mainView.taskColumnWidth

                                    text: qsTr("Task's name")
                                    color: IngeScapeTheme.whiteColor
                                    font {
                                        family: IngeScapeTheme.labelFontFamily
                                        pixelSize: 16
                                        weight: Font.Black
                                    }
                                }

                                Text {
                                    anchors {
                                        verticalCenter: parent.verticalCenter
                                    }

                                    width: mainView.subjectColumnWidth

                                    text: qsTr("Subject's ID")
                                    color: IngeScapeTheme.whiteColor
                                    font {
                                        family: IngeScapeTheme.labelFontFamily
                                        pixelSize: 16
                                        weight: Font.Black
                                    }
                                }

                                Text {
                                    anchors {
                                        verticalCenter: parent.verticalCenter
                                    }

                                    width: mainView.startDateColumnWidth

                                    text: qsTr("Start date")
                                    color: IngeScapeTheme.whiteColor
                                    font {
                                        family: IngeScapeTheme.labelFontFamily
                                        pixelSize: 16
                                        weight: Font.Black
                                    }
                                }

                                Text {
                                    anchors {
                                        verticalCenter: parent.verticalCenter
                                    }

                                    width: mainView.startTimeColumnWidth

                                    text: qsTr("Start time")
                                    color: IngeScapeTheme.whiteColor
                                    font {
                                        family: IngeScapeTheme.labelFontFamily
                                        pixelSize: 16
                                        weight: Font.Black
                                    }
                                }

                                Text {
                                    anchors {
                                        verticalCenter: parent.verticalCenter
                                    }

                                    width: mainView.durationColumnWidth

                                    text: qsTr("Duration")
                                    color: IngeScapeTheme.whiteColor
                                    font {
                                        family: IngeScapeTheme.labelFontFamily
                                        pixelSize: 16
                                        weight: Font.Black
                                    }
                                }
                            }
                        }

                        Repeater {
                            model: rootItem.experimentation ? rootItem.experimentation.allRecordSetups : null

                            delegate: RecordSetupInList {
                                taskColumnWidth: mainView.taskColumnWidth
                                subjectColumnWidth: mainView.subjectColumnWidth
                                startDateColumnWidth: mainView.startDateColumnWidth
                                startTimeColumnWidth: mainView.startTimeColumnWidth
                                durationColumnWidth: mainView.durationColumnWidth
                                buttonColumnWidth: mainView.buttonColumnWidth

                                anchors {
                                    left: parent.left
                                    right: parent.right
                                }

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

                    Rectangle {
                        id: bottomShadow
                        anchors {
                            left: parent.left
                            right: parent.right
                            bottom: parent.bottom
                        }
                        height: 4
                        gradient: Gradient {
                            GradientStop { position: 0.0; color: IngeScapeTheme.whiteColor; }
                            GradientStop { position: 1.0; color: IngeScapeTheme.darkGreyColor; }
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
