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


Item {
    id: rootItem

    //--------------------------------------------------------
    //
    //
    // Properties
    //
    //
    //--------------------------------------------------------

    property TaskInstanceController taskInstanceController: null;

    property ExperimentationController experimentationController: null;

    property TaskInstanceM taskInstance: taskInstanceController ? taskInstanceController.currentTaskInstance : null;

    property bool isEditingName: false

    // 319 = actions panel width
    // 16 = left margin of other panels
    // 8 = the (right) margin between the last panel and the window
    //property real panelWidth: (rootItem.width - 319 - 8) / 3 - 16
    //property var panelWidths: [ 319, panelWidth, panelWidth, panelWidth]

    // 319 = actions panel width
    // 8 = the (right) margin between the last panel and the window
    // 16 = left margin of other panels
    property real restWidth_For3Panels: rootItem.width - 319 - 8 - (3*16)
    property var panelWidths: [ 319, 2*restWidth_For3Panels/7, 3*restWidth_For3Panels/7, 2*restWidth_For3Panels/7]



    //--------------------------------
    //
    //
    // Signals
    //
    //
    //--------------------------------

    // Go back to "Home"
    signal goBackToHome();

    // Go back to "Experimentation"
    signal goBackToExperimentation();



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

    Rectangle {
        id: actionsRightShadow

        x: leftPanel.width - (width / 2) + height / 2
        y: leftPanel.y + (leftPanel.height / 2) - height / 2

        height: 8
        width: leftPanel.height
        rotation: -90

        gradient: Gradient {
            GradientStop { position: 0.0; color: IngeScapeTheme.lightGreyColor; }
            GradientStop { position: 1.0; color: "transparent" }
        }
    }

    //
    // Scenario TimeLine
    //
    ScenarioTimeLine {
        id: timeline

        anchors {
            left: leftPanel.right
            right: parent.right
            bottom: parent.bottom
        }
        height: 0

        scenarioController: rootItem.taskInstanceController ? rootItem.taskInstanceController.scenarioC : null;
        timeLineController: rootItem.taskInstanceController ? rootItem.taskInstanceController.timeLineC : null;
        licensesController: IngeScapeAssessmentsC.licensesC
        mainController: IngeScapeAssessmentsC

        Rectangle {
            id: timeLineLeftShadow

            anchors {
                horizontalCenter: parent.left
            }

            y: (timeline.height / 2) - (height / 2)

            height: 16
            width: parent.height
            rotation: -90

            gradient: Gradient {
                GradientStop { position: 0.0; color: "#000000"; }
                GradientStop { position: 1.0; color: "transparent" }
            }
        }
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

        height: 108

        Button {
            id: btnGoBackToHome

            anchors {
                left: parent.left
                leftMargin: 21
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

        Button {
            id: btnGoBackToExpe

            anchors {
                left: btnGoBackToHome.right
                leftMargin: 15
                top: parent.top
                topMargin: 20
            }

            height: 50
            width: 50

            style: IngeScapeAssessmentsSvgButtonStyle {
                releasedID: "back-to-records-list"
                disabledID: releasedID
            }

            onClicked: {
                console.log("QML: Go back to 'Expe'");

                // Emit the signal the "Go Back To Expe"
                rootItem.goBackToExperimentation();
            }
        }

        MouseArea {
            id: taskInstanceMouseArea
            anchors {
                fill: taskInstanceNameEditBackground
            }

            hoverEnabled: true
        }

        Rectangle {
            id: taskInstanceNameEditBackground
            anchors {
                left: taskInstanceName.left
                leftMargin: -10
                verticalCenter: taskInstanceName.verticalCenter
            }

            radius: 5

            // Background rectangle won't be smaller than this
            property real minWidth: 450

            // Max available width based on the size of the parent. Won't be bigger than this
            property real maxAvailableWidth: (parent.width
                                              - 92 // taskInstanceName's leftMargin
                                              + 10 // this left margin from taskInstanceName
                                              - 22 // Right margin left to avoid reaching the edge of the window
                                              - 10  // Margins around the edit button (5 on both sides)
                                              )

            // Desired width to follow the size of expeName
            property real desiredWidth: taskInstanceName.width
                                        + 10                // leftMargin
                                        + 10                // rightMargin
                                        + editButton.width  // button size
                                        + 14                // margin between edit button and text

            width: Math.min(maxAvailableWidth, Math.max(desiredWidth, minWidth))
            height: 40

            color: IngeScapeTheme.veryLightGreyColor

            opacity: (taskInstanceMouseArea.containsMouse || editButton.containsMouse || rootItem.isEditingName) ? 1 : 0
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
                        taskInstanceNameEditionTextField.text = taskInstanceName.text;
                    }
                    else
                    {
                        // Exiting edition mode
                        if (rootItem.taskInstance)
                        {
                            taskInstanceName.text = taskInstanceNameEditionTextField.text
                            rootItem.taskInstance.name = taskInstanceName.text
                        }
                    }
                }
            }
        }

        Text {
            id: taskInstanceName
            anchors {
                top: parent.top
                topMargin: 30
                left: parent.left
                leftMargin: 153
            }

            property real maxAvailableWidth: taskInstanceNameEditBackground.maxAvailableWidth
                                             - 10                // background's left margin
                                             - 5                 // edit button's right margin
                                             - editButton.width  // edit button's width
                                             - 14                // margin between edit button and text

            width: Math.min(implicitWidth, maxAvailableWidth)

            text: rootItem.taskInstance ? rootItem.taskInstance.name : ""
            elide: Text.ElideRight

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
            id: taskInstanceNameEditionTextField
            anchors.fill: taskInstanceName

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

        Text {
            id: expeNameText

            anchors {
                top: taskInstanceName.bottom
                topMargin: 15
                left: taskInstanceName.left
            }

            width: taskInstanceName.maxAvailableWidth

            text: rootItem.experimentationController && rootItem.experimentationController.currentExperimentation ? rootItem.experimentationController.currentExperimentation.name : ""
            elide: Text.ElideRight

            verticalAlignment: Text.AlignVCenter

            color: IngeScapeAssessmentsTheme.regularDarkBlueHeader
            font {
                family: IngeScapeTheme.textFontFamily
                pixelSize: 20
                bold: true
            }
        }
    }

    //
    // Agents and actions pannel
    //
    Rectangle {
        id: leftPanel

        anchors {
            left: parent.left
            top: headerItem.bottom
            bottom: parent.bottom
        }
        width: panelWidths[0]

        color: IngeScapeTheme.veryDarkGreyColor

        I2CustomRectangle {
            anchors {
                fill: parent
                topMargin: 9
            }
            color: IngeScapeTheme.blackColor

            fuzzyRadius: 8
            topRightRadius : 5

            borderWidth: 1
            borderColor: IngeScapeTheme.veryDarkGreyColor

            // tabs of left panel
            I2TabView {
                id : leftPanelTabs

                anchors.fill: parent

                style: I2TabViewStyle {
                    frameOverlap: 1

                    tab: I2CustomRectangle {
                        color: styleData.selected ? IngeScapeTheme.veryDarkGreyColor : "transparent"

                        implicitWidth: leftPanelTabs.width / 2
                        implicitHeight: 26
                        topRightRadius : 5

                        visible: true

                        Text {
                            id: text

                            anchors.fill: parent
                            horizontalAlignment: Text.AlignHCenter
                            verticalAlignment: Text.AlignVCenter;

                            text: styleData.title

                            color: styleData.selected ? IngeScapeTheme.whiteColor : IngeScapeTheme.lightGreyColor
                            wrapMode: Text.Wrap;

                            font {
                                family: IngeScapeTheme.labelFontFamily;
                                weight: Font.ExtraBold;
                                pixelSize:18;
                                capitalization: Font.AllUppercase;
                            }
                        }
                    }

                    frame: Rectangle {
                        color: IngeScapeTheme.veryDarkGreyColor
                    }
                }

                Tab {
                    id: tabAgents
                    title: qsTr("AGENTS")
                    active: true

                    Rectangle {
                        color: IngeScapeTheme.veryDarkGreyColor

                    }
                }

                Tab {
                    id: tabActions
                    anchors.fill: parent
                    title: qsTr("ACTIONS")
                    active : true

                    ActionsList {
                        id: agentsList
                        taskInstanceController: rootItem.taskInstanceController
                    }
                }
            }
        }
    }


    //
    // Comments panel
    //
    Item {
        id: commentsPanel

        anchors {
            left: leftPanel.right
            leftMargin: 16
            top: headerItem.bottom
            topMargin: 16
            bottom: timeline.top
            bottomMargin: 56
        }
        width: panelWidths[1]

        IngeScapeAssessmentsListHeader {
            id: commentsHeader
            anchors {
                top: parent.top
                left: parent.left
                right: parent.right
            }

            Text {
                anchors {
                    left: parent.left
                    leftMargin: 25
                    top: parent.top
                    bottom: parent.bottom
                }

                text: qsTr("COMMENTS")
                verticalAlignment: Text.AlignVCenter

                color: IngeScapeTheme.whiteColor
                font {
                    family: IngeScapeTheme.labelFontFamily
                    weight: Font.Black
                    pixelSize: 20
                }
            }
        }

        Rectangle {
            anchors {
                left: parent.left
                right: parent.right
                top: commentsHeader.bottom
                bottom: parent.bottom
            }

            color: IngeScapeTheme.whiteColor

            TextAreaWithTimer {
                id: txtComments

                anchors {
                    fill: parent
                    margins: 18
                }

                wrapMode: Text.WordWrap

                text: rootItem.taskInstance ? rootItem.taskInstance.comments : ""

                style: IngeScapeAssessmentsTextAreaStyle {}

                onInputChanged: {
                    if (rootItem.taskInstance) {
                        rootItem.taskInstance.comments = text
                    }
                }
            }
        }

        // Bottom shadow
        Rectangle {
            anchors {
                top: parent.bottom
                left: parent.left
                right: parent.right
            }
            height: 4
            gradient: Gradient {
                GradientStop { position: 0.0; color: IngeScapeTheme.whiteColor; }
                GradientStop { position: 1.0; color: IngeScapeTheme.darkGreyColor; }
            }
        }
    }

    //
    // Independent Variable panel
    //
    Item {
        id: indepVarPanel

        anchors {
            left: commentsPanel.right
            leftMargin: 16
            top: headerItem.bottom
            topMargin: 16
            bottom: timeline.top
            bottomMargin: 56
        }
        width: panelWidths[2]

        IngeScapeAssessmentsListHeader {
            id: indepVarHeader

            anchors {
                top: parent.top
                left: parent.left
                right: parent.right
            }

            Text {
                anchors {
                    left: parent.left
                    leftMargin: 25
                    top: parent.top
                    bottom: parent.bottom
                }

                text: qsTr("INDEPENDENT VARIABLES")
                verticalAlignment: Text.AlignVCenter

                color: IngeScapeTheme.whiteColor
                font {
                    family: IngeScapeTheme.labelFontFamily
                    weight: Font.Black
                    pixelSize: 20
                }
            }
        }

        Rectangle {
            anchors {
                left: parent.left
                right: parent.right
                top: indepVarHeader.bottom
                bottom: parent.bottom
            }

            color: IngeScapeTheme.whiteColor


            Column {
                id: listIndependentVariable

                anchors {
                    fill: parent
                    leftMargin: 16
                    topMargin: 18
                    rightMargin: 16
                    bottomMargin: 5
                }

                spacing: 15

                Repeater {

                    model: (rootItem.taskInstance && rootItem.taskInstance.task) ? rootItem.taskInstance.task.independentVariables : null

                    delegate: IndependentVariableValueEditor {

                        anchors {
                            left: parent.left
                            right: parent.right
                        }

                        variable: model ? model.QtObject : null

                        variableValue: (rootItem.taskInstance && rootItem.taskInstance.mapIndependentVariableValues && model) ? rootItem.taskInstance.mapIndependentVariableValues[model.name] : ""

                        //
                        // Slots
                        //
                        onIndependentVariableValueUpdated: {
                            if (rootItem.taskInstance && rootItem.taskInstance.mapIndependentVariableValues && model)
                            {
                                //console.log("QML: on (IN-dependent) Variable Value Updated for " + model.name + ": " + value);

                                // Update the value (in C++)
                                rootItem.taskInstance.mapIndependentVariableValues[model.name] = value;
                            }
                        }
                    }
                }
            }
        }

        // Bottom shadow
        Rectangle {
            anchors {
                top: parent.bottom
                left: parent.left
                right: parent.right
            }
            height: 4
            gradient: Gradient {
                GradientStop { position: 0.0; color: IngeScapeTheme.whiteColor; }
                GradientStop { position: 1.0; color: IngeScapeTheme.darkGreyColor; }
            }
        }
    }


    //
    // Attachments panel
    //
    Item {
        id: attachmentsPanel

        anchors {
            left: indepVarPanel.right
            leftMargin: 16
            top: headerItem.bottom
            topMargin: 16
            bottom: timeline.top
            bottomMargin: 56
        }
        width: panelWidths[3]

        IngeScapeAssessmentsListHeader {
            id: attachementsHeader

            anchors {
                top: parent.top
                left: parent.left
                right: parent.right
            }

            Text {
                anchors {
                    left: parent.left
                    leftMargin: 25
                    top: parent.top
                    bottom: parent.bottom
                }

                text: qsTr("ATTACHEMENTS")
                verticalAlignment: Text.AlignVCenter

                color: IngeScapeTheme.whiteColor
                font {
                    family: IngeScapeTheme.labelFontFamily
                    weight: Font.Black
                    pixelSize: 20
                }
            }
        }

        Rectangle {
            id: dropZone

            anchors {
                left: parent.left
                right: parent.right
                top: attachementsHeader.bottom
                bottom: parent.bottom
                //bottomMargin: 10
            }

            color: IngeScapeTheme.whiteColor

            // Fake model with URLs of dropped files to see results in view
            property var listModel: ListModel {}

            ScrollView {
                id: attachementScrollView
                anchors {
                    fill: parent
                    rightMargin: -attachementScrollView.scrollBarSize -attachementScrollView.verticalScrollbarMargin
                }

                property int scrollBarSize: 11
                property int verticalScrollbarMargin: 3

                style: IngeScapeAssessmentsScrollViewStyle {
                    scrollBarSize: attachementScrollView.scrollBarSize
                    verticalScrollbarMargin: attachementScrollView.verticalScrollbarMargin
                }

                // Prevent drag overshoot on Windows
                flickableItem.boundsBehavior: Flickable.OvershootBounds

                contentItem: Item {
                    id: attachementScrollViewItem

                    width: attachementScrollView.width - (attachementScrollView.scrollBarSize + attachementScrollView.verticalScrollbarMargin)
                    height: childrenRect.height

                    ListView {
                        id: attechementsListView
                        anchors {
                            top: parent.top
                            left: parent.left
                            right: parent.right
                        }

                        height: childrenRect.height

                        // Fake model
                        model: dropZone.listModel

                        delegate: AttachementDelegate {
                            id: attachementDelegate

                            anchors {
                                left: parent.left
                                right: parent.right
                            }

                            attachementName: model ? model.text : ""
                        }
                    }

                    Rectangle {
                        id: dropAreaIndicator

                        property real minimumHeight: 56

                        height: Math.max(minimumHeight, attachementScrollView.height - attechementsListView.height - 8 - 8)

                        anchors {
                            top: attechementsListView.bottom
                            topMargin: 8
                            left: parent.left
                            leftMargin: 10
                            right: parent.right
                            rightMargin: 10
                        }

                        radius: 5
                        visible: dropArea.dragHovering

                        color: IngeScapeTheme.veryLightGreyColor

                        border {
                            color: IngeScapeTheme.lightGreyColor
                            width: 1
                        }

                        I2SvgItem {
                            anchors {
                                centerIn: parent
                            }

                            svgFileCache: IngeScapeAssessmentsTheme.svgFileIngeScapeAssessments
                            svgElementId: "drag-and-drop-area"
                        }
                    }
                }
            }

            DropArea {
                id: dropArea

                anchors.fill: parent

                property bool dragHovering: false

                onEntered: {
                    console.log("Entered")
                    dragHovering = true
                }

                onExited: {
                    console.log("Exited")
                    dragHovering = false
                }

                onDropped: {
                    console.log("Dropped")
                    dragHovering = false
                    if (drop.hasUrls && rootItem.taskInstanceController)
                    {
                        rootItem.taskInstanceController.addNewAttachements(drop.urls)

                        // Populate fake model to see results in view
                        for (var index in drop.urls)
                        {
                            dropZone.listModel.append({"text": URLUtils.fileNameFromFileUrl(drop.urls[index])})
                        }
                    }
                }
            }
        }

        // Bottom shadow
        Rectangle {
            anchors {
                top: parent.bottom
                left: parent.left
                right: parent.right
            }
            height: 4
            gradient: Gradient {
                GradientStop { position: 0.0; color: IngeScapeTheme.whiteColor; }
                GradientStop { position: 1.0; color: IngeScapeTheme.darkGreyColor; }
            }
        }
    }

    Rectangle {
        id: headerBottomShadow

        anchors {
            top: headerItem.bottom
            left: leftPanel.right
            right: headerItem.right
        }

        height: 8

        gradient: Gradient {
            GradientStop { position: 0.0; color: IngeScapeTheme.darkGreyColor; }
            GradientStop { position: 1.0; color: "transparent" }
        }

    }
}
