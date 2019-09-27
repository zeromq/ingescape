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
 *      Mathieu Soum <soum@ingenuity.io>
 *
 */

import QtQuick 2.9
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4

import I2Quick 1.0

import INGESCAPE 1.0

//import "theme" as Theme
import "../popup" as Popup


I2PopupBase {
    id: rootItem

    //--------------------------------------------------------
    //
    //
    // Properties
    //
    //
    //--------------------------------------------------------

    property SubjectsController subjectController: null;

    property ExperimentationM experimentation: subjectController ? subjectController.currentExperimentation : null;

    // Width of the columns in the subject list view
    property real characteristicValueColumnWidth: 228

    // Flag indicating if a subject, amongst all subjects, is being edited
    property bool subjectEditionInProgress: false

    // Current subject delegate being edited
    property var currentlyEditedSubjectDelegate: null


    //--------------------------------
    //
    //
    // Signals
    //
    //
    //--------------------------------

    // Close Tasks view
    //signal closeSubjectsView();


    //--------------------------------
    //
    //
    // Slots
    //
    //
    //--------------------------------

    onClosed: {
        // Cancel any edition in progress when closing the subject popup
        if (rootItem.currentlyEditedSubjectDelegate) {
            rootItem.currentlyEditedSubjectDelegate.cancelEdition()
        }
    }

    onSubjectEditionInProgressChanged: {
        if (!subjectEditionInProgress) {
            // Clear current subject being edited when edition ended
            currentlyEditedSubjectDelegate = null
        }
    }


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

    Button {
        id: btnClose

        anchors {
            top: parent.top
            topMargin: 21
            right: parent.right
            rightMargin: 21
        }

        height: 18
        width: 18

        style: IngeScapeAssessmentsSvgButtonStyle {
            releasedID: "close"
        }

        onClicked: {
            console.log("QML: close Subjects view");

            // Emit the signal "closeSubjectsView"
            //rootItem.closeSubjectsView();

            close();
        }
    }

    // Characteristics
    Item {
        id: characteristicsItem
        anchors {
            top: parent.top
            topMargin: 24
            left: parent.left
            leftMargin: 26
            bottom: parent.bottom
            bottomMargin: 24
        }

        width: 360

        Text {
            id: titleCharacteristics

            anchors {
                verticalCenter: btnNewCharacteristic.verticalCenter
                left: parent.left
            }

            text: qsTr("CHARACTERISTICS")

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
            id: btnNewCharacteristic

            anchors {
                top: parent.top
                right: parent.right
            }

            height: 40
            width: 134

            onClicked: {
                // Open the popup
                createCharacteristicPopup.open();
            }

            style: IngeScapeAssessmentsButtonStyle {
                text: qsTr("ADD NEW")
            }
        }

        Rectangle {
            id: characteristicsList
            anchors {
                top: btnNewCharacteristic.bottom
                topMargin: 14
                left: parent.left
                right: parent.right
                bottom: parent.bottom
            }

            color: IngeScapeTheme.whiteColor

            ScrollView {
                id: characteristicsScrillView
                anchors {
                    fill: parent
                    rightMargin: -17
                }

                style: IngeScapeAssessmentsScrollViewStyle {}

                // Prevent drag overshoot on Windows
                flickableItem.boundsBehavior: Flickable.OvershootBounds

                Column {
                    id: characteristicsColumn
                    width: characteristicsScrillView.width - 17
                    height: childrenRect.height
                    spacing: 0

                    Repeater {
                        model: rootItem.experimentation ? rootItem.experimentation.allCharacteristics : null

                        delegate: Characteristic {

                            anchors {
                                left: parent.left
                                right: parent.right
                            }

                            modelM: model.QtObject

                            //
                            // Slots
                            //
                            onDeleteCharacteristic: {
                                if (rootItem.subjectController) {
                                    rootItem.subjectController.deleteCharacteristic(model.QtObject);
                                }
                            }
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

    // Subjects
    Item {
        id: subjectsItem
        anchors {
            top: parent.top
            topMargin: 80
            left: characteristicsItem.right
            leftMargin: 26
            right: parent.right
            rightMargin: 26
            bottom: parent.bottom
            bottomMargin: 24
        }

        Text {
            id: titleSubjects

            anchors {
                verticalCenter: btnDownloadSubjects.verticalCenter
                left: parent.left
            }

            text: qsTr("SUBJECTS")

            height: parent.height
            verticalAlignment: Text.AlignVCenter

            color: IngeScapeAssessmentsTheme.regularDarkBlueHeader
            font {
                family: IngeScapeTheme.labelFontFamily
                weight: Font.Black
                pixelSize: 22
            }
        }

        Button {
            id: btnDownloadSubjects

            anchors {
                top: parent.top
                right: btnNewSubject.left
                rightMargin: 12
            }

            height: 40
            width: 182

            onClicked: {
                console.log("Not implemented yet")
            }

            style: IngeScapeAssessmentsSvgAndTextButtonStyle {
                text: qsTr("DOWNLOAD LIST")

                releasedID: "download-list"
                pressedID: releasedID
                rolloverID: releasedID
                disabledID: releasedID
            }
        }

        Button {
            id: btnNewSubject

            anchors {
                top: parent.top
                right: parent.right
            }

            height: 40
            width: 182

            onClicked: {
                // Open the popup
                if (rootItem.subjectController)
                {
                    rootItem.subjectController.createNewSubject()
                }
            }

            style: IngeScapeAssessmentsButtonStyle {
                text: qsTr("NEW SUBJECT")
            }
        }

        IngeScapeAssessmentsListHeader {
            id: subjectListHeader
            anchors {
                top: btnDownloadSubjects.bottom
                topMargin: 12
                left: parent.left
                right: parent.right
                rightMargin: 144
            }

            Flickable {
                id: headerFlickable

                anchors {
                    top: parent.top
                    left: parent.left
                    leftMargin: 15
                    right: parent.right
                    bottom: parent.bottom
                }

                clip: true

                Row {
                    height: parent.height
                    width: childrenRect.height

                    Repeater {
                        model: rootItem.experimentation ? rootItem.experimentation.allCharacteristics : null

                        Text {
                            anchors {
                                verticalCenter: parent.verticalCenter
                            }

                            width: rootItem.characteristicValueColumnWidth

                            elide: Text.ElideRight
                            text: model ? model.name : ""
                            color: IngeScapeTheme.whiteColor
                            font {
                                family: IngeScapeTheme.labelFontFamily
                                pixelSize: 16
                                weight: Font.Black
                            }
                        }
                    }
                }
            }
        }

        Binding {
            target: headerFlickable
            property: "contentX"
            value: subjectsScrollView.flickableItem.contentX
        }

        Binding {
            target: buttonsFlickable
            property: "contentY"
            value: subjectsScrollView.flickableItem.contentY
        }

        Rectangle {
            id: scrollViewBackground
            anchors {
                top: subjectListHeader.bottom
                left: subjectListHeader.left
                right: subjectListHeader.right
                bottom: parent.bottom
            }

            color: IngeScapeTheme.whiteColor

            ScrollView {
                id: subjectsScrollView
                anchors {
                    fill: parent
                    bottomMargin: __horizontalScrollBar.visible ? -scrollBarSize - horizontalScrollbarMargin  : 0
                    rightMargin: __verticalScrollBar.visible ? -scrollBarSize - verticalScrollbarMargin : 0
                }

                property int scrollBarSize: 8
                property int verticalScrollbarMargin: 3
                property int horizontalScrollbarMargin: 3

                style: IngeScapeAssessmentsScrollViewStyle {
                    scrollBarSize: subjectsScrollView.scrollBarSize
                    verticalScrollbarMargin: subjectsScrollView.verticalScrollbarMargin
                    horizontalScrollbarMargin: subjectsScrollView.horizontalScrollbarMargin
                }

                // Prevent drag overshoot on Windows
                flickableItem.boundsBehavior: Flickable.OvershootBounds

                Column {
                    id: subjectsColumn

                    width: childrenRect.width
                    height: childrenRect.height
                    spacing: 0

                    // Items within subjectsRepeater have this column as parent
                }
            }
        }

        Flickable {
            id: buttonsFlickable

            anchors {
                top: scrollViewBackground.top
                left: scrollViewBackground.right
                right: parent.right
                bottom: scrollViewBackground.bottom
            }

            clip: true
            interactive: false

            Column {
                id: buttonsFlickableColumn
                height: parent.height
                width: parent.width

                // Items within subjectsRepeater have this column as parent
            }
        }

        Rectangle {
            id: subjectsBottomShadow
            anchors {
                left: scrollViewBackground.left
                right: scrollViewBackground.right
                bottom: parent.bottom
            }
            height: 4
            gradient: Gradient {
                GradientStop { position: 0.0; color: IngeScapeTheme.whiteColor; }
                GradientStop { position: 1.0; color: IngeScapeTheme.darkGreyColor; }
            }
        }
    }

    Repeater {
        id: subjectsRepeater
        model: rootItem.experimentation ? rootItem.experimentation.allSubjects : null

        delegate: Subject {
            id: subjectDelegate

            height: 40
            width: Math.max(characteristicValueColumnWidth * rootItem.experimentation.allCharacteristics.count, subjectsScrollView.width - (subjectsScrollView.__verticalScrollBar.visible ? subjectsScrollView.scrollBarSize + subjectsScrollView.verticalScrollbarMargin : 0))

            experimentation: rootItem.experimentation
            subject: model ? model.QtObject : null

            characteristicValueColumnWidth: rootItem.characteristicValueColumnWidth
            subjectEditionInProgress: rootItem.subjectEditionInProgress

            // Changing characteristics values item's parent
            characteristicsRowParent: subjectsColumn
            // Changing button item's parent
            buttonsRowParent: buttonsFlickableColumn

            Binding {
                target: rootItem
                property: "subjectEditionInProgress"
                value: subjectDelegate.isCurrentlyEditing
            }

            Binding {
                target: rootItem
                property: "currentlyEditedSubjectDelegate"
                value: subjectDelegate
                when: subjectDelegate.isCurrentlyEditing
            }

            onDeleteSubject: {
                if (rootItem.subjectController && subjectDelegate.subject)
                {
                    subjectDelegate.isCurrentlyEditing = false
                    rootItem.subjectController.deleteSubject(subjectDelegate.subject);
                }

            }
        }
    }

    //
    // Create Characteristic Popup
    //
    Popup.CreateCharacteristicPopup {
        id: createCharacteristicPopup

        layerObjectName: "overlay2Layer"

        subjectController: rootItem.subjectController
    }
}
