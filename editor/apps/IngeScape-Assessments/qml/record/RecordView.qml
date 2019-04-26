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
//import "popup" as Popup


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

    property RecordController controller: null;

    //property AssessmentsModelManager modelManager: null;

    property ExperimentationRecordM record: controller ? controller.currentRecord : null;



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

        color: "#FF335555"
    }

    Row {
        id: breadcrumb

        anchors {
            left: parent.left
            top: parent.top
        }
        height: 30

        Button {
            id: btnGoBackToHome

            height: parent.height

            text: "HOME"

            onClicked: {
                console.log("QML: Go back to 'Home'");

                // Emit the signal the "Go Back To Home"
                rootItem.goBackToHome();
            }
        }

        Button {
            id: btnGoBackToExperimentation

            height: parent.height

            text: "EXPE"

            onClicked: {
                console.log("QML: Go back to 'Experimentation'");

                // Emit the signal the "Go Back To Experimentation"
                rootItem.goBackToExperimentation();
            }
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

            text: rootItem.record ? rootItem.record.name : ""

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

            text: rootItem.record && rootItem.record.subject ? rootItem.record.subject.name : ""

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

            text: rootItem.record && rootItem.record.task ? rootItem.record.task.name : ""

            color: IngeScapeTheme.whiteColor
            font {
                family: IngeScapeTheme.textFontFamily
                weight: Font.Medium
                pixelSize: 16
            }
        }
    }


    //
    // Actions panel
    //
    Rectangle {
        id: actionsPanel

        anchors {
            left: parent.left
            top: header.bottom
            topMargin: 20
            bottom: timeline.top
            bottomMargin: 5
        }
        width: parent.width / 4.0

        color: "transparent"
        border {
            color: "black"
            width: 2
        }

        Text {
            id: titleActions

            anchors {
                left: parent.left
                leftMargin: 5
                top: parent.top
                topMargin: 5
            }

            text: qsTr("Actions:")

            color: IngeScapeTheme.whiteColor
            font {
                family: IngeScapeTheme.textFontFamily
                weight: Font.Medium
                pixelSize: 14
            }
        }
    }


    //
    // Comments panel
    //
    Rectangle {
        id: commentsPanel

        anchors {
            left: actionsPanel.right
            top: header.bottom
            topMargin: 20
            bottom: timeline.top
            bottomMargin: 5
        }
        width: parent.width / 4.0

        color: "transparent"
        border {
            color: "black"
            width: 2
        }

        Text {
            id: titleComments

            anchors {
                left: parent.left
                leftMargin: 5
                top: parent.top
                topMargin: 5
            }

            text: qsTr("Comments:")

            color: IngeScapeTheme.whiteColor
            font {
                family: IngeScapeTheme.textFontFamily
                weight: Font.Medium
                pixelSize: 14
            }
        }

        TextArea {
            id: txtComments

            anchors {
                left: parent.left
                leftMargin: 5
                right: parent.right
                rightMargin: 5
                top: titleComments.bottom
                topMargin: 5
                bottom: parent.bottom
                bottomMargin: 5
            }

            text: "Lorem ipsum dolor sit amet, consectetur adipiscing elit. Nam lobortis augue pellentesque mattis tincidunt. Nunc efficitur faucibus nunc, nec facilisis augue semper vitae.
Proin consequat nulla at risus lacinia sollicitudin. Nunc efficitur commodo leo at fringilla. Quisque ullamcorper aliquet nulla, ut molestie ipsum placerat sed. Proin pretium sodales semper.
Ut vehicula nibh non metus lacinia dignissim. Suspendisse eu mi venenatis, porttitor tellus nec, ultrices erat. Mauris nibh metus, facilisis et diam quis, posuere faucibus lorem."
        }
    }


    //
    // Independent Variable panel
    //
    Rectangle {
        id: independentVariablePanel

        anchors {
            left: commentsPanel.right
            top: header.bottom
            topMargin: 20
            bottom: timeline.top
            bottomMargin: 5
        }
        width: parent.width / 4.0

        color: "transparent"
        border {
            color: "black"
            width: 2
        }

        Text {
            id: titleIndependentVariable

            anchors {
                left: parent.left
                leftMargin: 5
                top: parent.top
                topMargin: 5
            }
            height: 30

            text: qsTr("Independent Variable:")

            verticalAlignment: Text.AlignVCenter

            color: IngeScapeTheme.whiteColor
            font {
                family: IngeScapeTheme.textFontFamily
                weight: Font.Medium
                pixelSize: 14
            }
        }

        Button {
            id: btnEditIndependentVariable

            anchors {
                top: parent.top
                topMargin: 5
                right: parent.right
                rightMargin: 5
            }
            width: 50
            height: 30

            checkable: true

            checked: false

            text: checked ? "SAVE" : "EDIT"
        }

        Column {
            id: listIndependentVariable

            anchors {
                left: parent.left
                leftMargin: 5
                right: parent.right
                rightMargin: 5
                top: titleIndependentVariable.bottom
                topMargin: 5
                bottom: parent.bottom
                bottomMargin: 5
            }

            spacing: 5

            Repeater {

                model: (rootItem.record && rootItem.record.task) ? rootItem.record.task.independentVariables : null

                delegate: IndependentVariableValueEditor {

                    variable: model ? model.QtObject : null

                    variableValue: (rootItem.record && rootItem.record.mapIndependentVariableValues && model) ? rootItem.record.mapIndependentVariableValues[model.name] : ""

                    isCurrentlyEditing: btnEditIndependentVariable.checked


                    //
                    // Slots
                    //
                    onIndependentVariableValueUpdated: {
                        if (rootItem.record && rootItem.record.mapIndependentVariableValues && model)
                        {
                            //console.log("QML: on (IN-dependent) Variable Value Updated for " + model.name + ": " + value);

                            // Update the value (in C++)
                            rootItem.record.mapIndependentVariableValues[model.name] = value;
                        }
                    }
                }
            }
        }
    }


    //
    // Attachments panel
    //
    Rectangle {
        id: attachmentsPanel

        anchors {
            left: independentVariablePanel.right
            //right: parent.right
            top: header.bottom
            topMargin: 20
            bottom: timeline.top
            bottomMargin: 5
        }
        width: parent.width / 4.0

        color: "transparent"
        border {
            color: "black"
            width: 2
        }

        Text {
            id: titleAttachments

            anchors {
                left: parent.left
                leftMargin: 5
                top: parent.top
                topMargin: 5
            }

            text: qsTr("Attachments:")

            color: IngeScapeTheme.whiteColor
            font {
                family: IngeScapeTheme.textFontFamily
                weight: Font.Medium
                pixelSize: 14
            }
        }
    }


    //
    // Timeline
    //
    Rectangle {
        id: timeline

        anchors {
            left: parent.left
            right: parent.right
            bottom: parent.bottom
        }
        height: 250

        color: "white"

        Text {
            anchors.centerIn: parent

            text: qsTr("TIMELINE")

            //color: IngeScapeTheme.whiteColor
            font {
                family: IngeScapeTheme.textFontFamily
                weight: Font.Medium
                pixelSize: 14
            }
        }
    }
}
