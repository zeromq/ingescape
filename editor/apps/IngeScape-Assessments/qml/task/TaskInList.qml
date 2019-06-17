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
//import "../popup" as Popup


Item {
    id: rootItem

    //anchors.fill: parent

    width: parent.width
    height: 60


    //--------------------------------------------------------
    //
    //
    // Properties
    //
    //
    //--------------------------------------------------------

    property TaskM modelM: null;

    property bool isSelected: false;



    //--------------------------------
    //
    //
    // Signals
    //
    //
    //--------------------------------

    // Signal emitted when the user clicks on our task
    signal selectTask();

    // Signal emitted when the user clicks on the "duplicate" button
    signal duplicateTask();

    // Signal emitted when the user clicks on the "delete" button
    signal deleteTask();



    //--------------------------------
    //
    //
    // Functions
    //
    //
    //--------------------------------




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

        color: rootItem.isSelected ? "lightsteelblue" : "transparent"

        Rectangle {
            id: leftBorder

            anchors {
                left: parent.left
                top: parent.top
                bottom: parent.bottom
            }
            width: 1

            color: IngeScapeTheme.darkGreyColor
        }
        Rectangle {
            id: topBorder

            anchors {
                left: parent.left
                right: parent.right
                top: parent.top
            }
            height: 1

            color: IngeScapeTheme.darkGreyColor
        }
        Rectangle {
            id: bottomBorder

            anchors {
                left: parent.left
                right: parent.right
                bottom: parent.bottom
            }
            height: 1

            color: IngeScapeTheme.darkGreyColor
        }

        Text {
            id: txtTaskName

            anchors {
                left: parent.left
                leftMargin: 5
                top: parent.top
                topMargin: 5
            }

            text: rootItem.modelM ? rootItem.modelM.name : ""

            color: IngeScapeTheme.whiteColor
            font {
                family: IngeScapeTheme.textFontFamily
                weight: Font.Medium
                pixelSize: 16
            }
        }

        Text {
            id: txtPlatformFile

            anchors {
                left: parent.left
                leftMargin: 5
                bottom: parent.bottom
                bottomMargin: 5
            }

            text: rootItem.modelM ? "Platform: " + rootItem.modelM.platformFileName
                                  : ""

            color: IngeScapeTheme.whiteColor
            font {
                family: IngeScapeTheme.textFontFamily
                //weight: Font.Medium
                pixelSize: 14
            }
        }

        MouseArea {

            anchors.fill: parent

            onClicked: {
                // Emit the signal "Select Task"
                rootItem.selectTask();
            }
        }


        Row {
            id: rowOptions

            spacing: 0

            anchors {
                right: parent.right
                top: parent.top
            }
            height: 30

            visible: rootItem.isSelected

            Button {
                id: btnDuplicate

                text: "COPY"

                //width: 80
                height: parent.height

                onClicked: {
                    // Emit the signal "Duplicate Task"
                    rootItem.duplicateTask();
                }
            }

            Button {
                id: btnDelete

                text: "DEL"

                //width: 50
                height: parent.height

                onClicked: {
                    // Emit the signal "Delete Task"
                    rootItem.deleteTask();
                }
            }
        }
    }

}
