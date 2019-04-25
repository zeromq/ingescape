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
                weight : Font.Medium
                pixelSize : 16
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
                weight : Font.Medium
                pixelSize : 16
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
                weight : Font.Medium
                pixelSize : 16
            }
        }
    }

}
