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

import QtQuick 2.8
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4

import I2Quick 1.0

import INGESCAPE 1.0

//import "theme" as Theme

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

    //
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

        color: "#AAEEAA"
    }

    Button {
        id: btnHome

        anchors {
            left: parent.left
            top: parent.top
        }

        text: "HOME"

        onClicked: {
            console.log("QML: Go back to home");

            rootItem.goBackToHome();
        }
    }

    Column {
        id: headers

        anchors {
            top: parent.top
            horizontalCenter: parent.horizontalCenter
        }
        spacing: 10

        Text {
            //id: title

            text: rootItem.experimentation ? rootItem.experimentation.name : ""

            anchors {
                horizontalCenter: parent.horizontalCenter
            }
        }

        Text {
            //id: title

            text: (rootItem.modelManager && rootItem.modelManager.currentExperimentationsGroup) ? rootItem.modelManager.currentExperimentationsGroup.name : ""

            anchors {
                horizontalCenter: parent.horizontalCenter
            }
        }

        Text {
            //id: title

            text: rootItem.experimentation ? rootItem.experimentation.creationDate.toLocaleString(Qt.locale(), "dd/MM/yyyy hh:mm:ss") : ""

            anchors {
                horizontalCenter: parent.horizontalCenter
            }
        }
    }


    Column {
        id: configurationPanel

        anchors {
            left: parent.left
            top: headers.bottom
            topMargin: 50
        }

        Button {
            text: "Subjects"

            onClicked: {
                console.log("QML: Open Subjects");
            }
        }

        Button {
            text: "Tasks"

            onClicked: {
                console.log("QML: Open Tasks");
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
