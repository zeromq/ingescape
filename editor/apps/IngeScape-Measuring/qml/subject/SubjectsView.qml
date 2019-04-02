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

    // Go back to "Parent" view
    signal goBackToParentView();



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
        id: btnGoBack

        anchors {
            left: parent.left
            top: parent.top
        }

        text: "BACK"

        onClicked: {
            console.log("QML: Go back to 'Parent' view");

            // Emit the signal "goBackToParentView"
            rootItem.goBackToParentView();
        }
    }

    Text {
        id: title

        anchors {
            top: parent.top
            topMargin: 10
            horizontalCenter: parent.horizontalCenter
        }

        text: "Subjects"

        color: IngeScapeTheme.whiteColor
        font {
            family: IngeScapeTheme.textFontFamily
            weight : Font.Medium
            pixelSize : 20
        }
    }


    //
    // Characteristics Panel
    //
    Rectangle {
        id: characteristicsPanel

        anchors {
            left: parent.left
            top: title.bottom
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
            top: title.bottom
            topMargin: 30
            bottom: parent.bottom
        }

        color: "#44AAAAAA"

        Row {
            id: subjectsHeader

            anchors {
                left: parent.left
                top: parent.top
            }

            Text {
                text: "Subjects"

                color: IngeScapeTheme.whiteColor
                font {
                    family: IngeScapeTheme.textFontFamily
                    weight : Font.Medium
                    pixelSize : 16
                }
            }

            Button {
                text: "New Subject"

                onClicked: {
                    console.log("New Subject");
                }
            }
        }
    }

}
