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
import "../popup" as Popup


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

    property SubjectsController controller: null;

    property IngeScapeModelManager modelManager: null;

    property ExperimentationM experimentation: modelManager ? modelManager.currentExperimentation : null;



    //--------------------------------
    //
    //
    // Signals
    //
    //
    //--------------------------------

    // Close Subjects view
    signal closeSubjectsView();



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

        color: "#FF333366"
    }

    Button {
        id: btnClose

        anchors {
            right: parent.right
            top: parent.top
        }

        text: "X"

        onClicked: {
            console.log("QML: close Subjects view");

            // Emit the signal "closeSubjectsView"
            rootItem.closeSubjectsView();
        }
    }

    Row {
        id: header

        anchors {
            top: parent.top
            topMargin: 10
            horizontalCenter: parent.horizontalCenter
        }

        Text {
            id: title

            text: "Subjects"

            color: IngeScapeTheme.whiteColor
            font {
                family: IngeScapeTheme.textFontFamily
                weight : Font.Medium
                pixelSize : 20
            }
        }

        Button {
            text: "New Subject"

            onClicked: {
                console.log("New Subject");
            }
        }
    }


    //
    // Characteristics Panel
    //
    Rectangle {
        id: characteristicsPanel

        anchors {
            left: parent.left
            top: header.bottom
            topMargin: 30
            bottom: parent.bottom
        }
        width: 350

        color: "#44222222"

        Row {
            id: characteristicsHeader

            anchors {
                left: parent.left
                top: parent.top
            }

            Text {
                text: "Characteristics"

                color: IngeScapeTheme.whiteColor
                font {
                    family: IngeScapeTheme.textFontFamily
                    weight : Font.Medium
                    pixelSize : 16
                }
            }

            Button {
                text: "New Characteristic"

                onClicked: {
                    console.log("New Characteristic");
                }
            }
        }

        Column {
            anchors {
                left: parent.left
                top: characteristicsHeader.bottom
                topMargin: 20
            }

            Repeater {
                model: rootItem.experimentation ? rootItem.experimentation.allCharacteristics : null

                delegate: Text {
                    text: model.name
                }

            }
        }
    }


    //
    // Subjects Panel
    //
    Rectangle {
        id: subjectsPanel

        anchors {
            left: characteristicsPanel.right
            top: header.bottom
            topMargin: 30
            bottom: parent.bottom
        }

        color: "#44AAAAAA"

    }

}
