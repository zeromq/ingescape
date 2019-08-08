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


        Rectangle {
            id: headerBackground

            anchors.fill: parent

            color: IngeScapeTheme.whiteColor
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

            height: 50
            width: 50

            style: IngeScapeAssessmentsSvgButtonStyle {
                releasedID: "home-button"
            }

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

            width: Math.max(expeName.width + 76, 450)
            height: 40

            color: IngeScapeTheme.veryLightGreyColor

            opacity: (expeNameMouseArea.containsMouse || editButton.containsMouse || rootItem.isEditingName) ? 1 : 0
            enabled: opacity > 0

            Button {
                id: editButton

                anchors {
                    right: parent.right
                    rightMargin: 5
                    verticalCenter: parent.verticalCenter
                }

                property bool containsMouse: __behavior.containsMouse

                width: 42
                height: 30

                style: IngeScapeAssessmentsSvgButtonStyle {
                    releasedID: "edit"
                    disabledID: releasedID
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
            horizontalAlignment: Text.AlignLeft

            visible: !rootItem.isEditingName

            color: IngeScapeAssessmentsTheme.blueButton
            font {
                family: IngeScapeTheme.labelFontFamily
                weight: Font.Black
                pixelSize: 24
            }
        }

        TextField {
            id: expeNameEditionTextField
            anchors.fill: expeName

            visible: rootItem.isEditingName
            enabled: visible

            style: I2TextFieldStyle {
                backgroundColor: IngeScapeTheme.whiteColor
                borderColor: IngeScapeTheme.lightGreyColor
                borderErrorColor: IngeScapeTheme.redColor
                radiusTextBox: 5
                borderWidth: 0
                borderWidthActive: 1
                textIdleColor: IngeScapeAssessmentsTheme.regularDarkBlueHeader
                textDisabledColor: IngeScapeTheme.veryLightGreyColor

                padding.left: 10
                padding.right: 5

                font {
                    pixelSize: 20
                    family: IngeScapeTheme.textFontFamily
                }
            }
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

                color: IngeScapeAssessmentsTheme.regularDarkBlueHeader
                font {
                    family: IngeScapeTheme.textFontFamily
                    pixelSize: 20
                    bold: true
                }
            }

            Text {
                height: expeGroupNameText.height

                text: rootItem.experimentation ? rootItem.experimentation.creationDate.toLocaleString(Qt.locale(), " - dd/MM/yyyy - hh:mm:ss") : ""

                verticalAlignment: Text.AlignVCenter
                horizontalAlignment: Text.AlignHCenter

                color: IngeScapeAssessmentsTheme.regularDarkBlueHeader
                font {
                    family: IngeScapeTheme.textFontFamily
                    pixelSize: 18
                    bold: true
                    italic: true
                }
            }
        }
    }

    //
    // Main view
    //
    Item {
        id: mainView

        anchors {
            top: headerItem.bottom
            bottom: parent.bottom
            left: parent.left
            right: parent.right
        }

        //FIXME Minimum size not handled
        property real taskColumnWidth: 280
        property real subjectColumnWidth: 216
        property real startDateColumnWidth: 138
        property real startTimeColumnWidth: 138
        property real durationColumnWidth: 126
        property real buttonColumnWidth: 158
        property real taskInstanceNameColumnWidth: taskInstancesPanel.width
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

                    style: IngeScapeAssessmentsSvgAndTextButtonStyle {
                        text: "SUBJECTS"

                        releasedID: "subjects"
                        pressedID: releasedID
                        rolloverID: releasedID
                        disabledID: releasedID
                    }

                    onClicked: {
                        // Add the "Subjects View" to the stack
                        subjectsViewPopup.open()
                    }
                }

                Button {
                    width: parent.width
                    height: 62

                    style: IngeScapeAssessmentsSvgAndTextButtonStyle {
                        text: "TASKS"

                        releasedID: "tasks"
                        pressedID: releasedID
                        rolloverID: releasedID
                        disabledID: releasedID
                    }

                    onClicked: {
                        // Add the "Tasks View" to the stack
                        tasksViewPopup.open();
                    }
                }

                Button {
                    width: parent.width
                    height: 62

                    enabled: false

                    style: IngeScapeAssessmentsSvgAndTextButtonStyle {
                        text: "CODING"

                        releasedID: "coding"
                        pressedID: releasedID
                        rolloverID: releasedID
                        disabledID: releasedID
                    }
                }

                Button {
                    width: parent.width
                    height: 62

                    enabled: false

                    style: IngeScapeAssessmentsSvgAndTextButtonStyle {
                        text: "CLEANING"

                        releasedID: "cleaning"
                        pressedID: releasedID
                        rolloverID: releasedID
                        disabledID: releasedID
                    }
                }

                Button {
                    width: parent.width
                    height: 62

                    style: IngeScapeAssessmentsSvgAndTextButtonStyle {
                        text: "EXPORT"

                        releasedID: "export"
                        pressedID: releasedID
                        rolloverID: releasedID
                        disabledID: releasedID
                    }

                    onClicked: {
                        console.log("QML: Add the 'Export View' to the stack");

                        // Add the "Export View" to the stack
                        //                            stackview.push(componentExportView);
                    }
                }
            }
        }


        //
        // Task Instances Panel
        //
        Item {
            id: taskInstancesPanel

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
                id: titleTaskInstances

                anchors {
                    verticalCenter: btnNewRecord.verticalCenter
                    left: parent.left
                }

                text: qsTr("TASK INSTANCES")

                height: parent.height
                verticalAlignment: Text.AlignVCenter

                color: IngeScapeAssessmentsTheme.regularDarkBlueHeader
                font {
                    family: IngeScapeTheme.labelFontFamily
                    weight: Font.Black
                    pixelSize: 24
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
                    createTaskInstancePopup.open();
                }

                style: IngeScapeAssessmentsButtonStyle {
                    text: qsTr("NEW TASK INSTANCE")
                }
            }

            IngeScapeAssessmentsListHeader {
                id: listHeader
                anchors {
                    top: btnNewRecord.bottom
                    topMargin: 12
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

                        width: mainView.taskInstanceNameColumnWidth

                        text: qsTr("Task instance's name")
                        color: IngeScapeTheme.whiteColor
                        font {
                            family: IngeScapeTheme.labelFontFamily
                            pixelSize: 18
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
                            pixelSize: 18
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
                            pixelSize: 18
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
                            pixelSize: 18
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
                            pixelSize: 18
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
                            pixelSize: 18
                            weight: Font.Black
                        }
                    }
                }
            }

            Rectangle {
                anchors {
                    top: listHeader.bottom
                    bottom: parent.bottom
                    left: parent.left
                    right: parent.right
                }

                color: IngeScapeTheme.whiteColor

                ScrollView {
                    id: taskInstanceScrollView
                    anchors {
                        fill: parent
                        rightMargin: -taskInstanceScrollView.scrollBarSize -taskInstanceScrollView.verticalScrollbarMargin
                    }

                    property int scrollBarSize: 11
                    property int verticalScrollbarMargin: 3

                    style: IngeScapeAssessmentsScrollViewStyle {
                        scrollBarSize: taskInstanceScrollView.scrollBarSize
                        verticalScrollbarMargin: taskInstanceScrollView.verticalScrollbarMargin
                    }

                    // Prevent drag overshoot on Windows
                    flickableItem.boundsBehavior: Flickable.OvershootBounds

                    Column {
                        id: taskInstanceColumn
                        width: taskInstanceScrollView.width - (taskInstanceScrollView.scrollBarSize + taskInstanceScrollView.verticalScrollbarMargin)
                        height: childrenRect.height
                        spacing: 0

                        Repeater {
                            model: rootItem.experimentation ? rootItem.experimentation.allTaskInstances : null

                            delegate: TaskInstanceInList {
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
                                onOpenTaskInstance: {
                                    if (rootItem.controller && modelM) {
                                        rootItem.controller.openTaskInstance(modelM);
                                    }
                                }

                                onDeleteTaskInstance: {
                                    deleteTaskInstancePopup.taskInstance = modelM
                                    deleteTaskInstancePopup.open()
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
        Popup.CreateTaskInstancePopup {
            id: createTaskInstancePopup

            experimentationController: rootItem.controller
            experimentation: rootItem.experimentation
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


    Subject.SubjectsView {
        id: subjectsViewPopup

        anchors.centerIn: parent

        width: parent.width - 78
        height: parent.height - 78

        subjectController: IngeScapeAssessmentsC.subjectsC

        //
        // Slots
        //
        onCloseSubjectsView: {
            console.log("QML: on Close Subjects view");

            // Remove the "Subjects View" from the stack
            close()
        }
    }


    //
    // Tasks View (popup)
    //
    Task.TasksView {
        id: tasksViewPopup

        anchors.centerIn: parent

        width: parent.width - 78
        height: parent.height - 78

        taskController: IngeScapeAssessmentsC.tasksC

        //
        // Slots
        //

        onCloseTasksView: {
            console.log("QML: on Close Tasks view");

            // Remove the "Tasks View" from the stack
            close()
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
//                stackview.pop();
            }
        }
    }


    Popup.DeleteConfirmationPopup {
        id: deleteTaskInstancePopup

        property var taskInstance: null

        showPopupTitle: false
        anchors.centerIn: parent

        text: qsTr("Are you sure you want to delete the task instance %1 ?").arg(taskInstance ? taskInstance.name : "")

        height: 160
        width: 470

        onValidated: {
            if (rootItem.controller && taskInstance) {
                rootItem.controller.deleteTaskInstance(taskInstance);
            }
            close()
        }

        onCanceled: {
            close()
        }

    }

}
