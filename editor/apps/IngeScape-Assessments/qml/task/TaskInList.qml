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

import "../popup" as Popup


Item {
    id: rootItem

    width: parent.width
    height: 97


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

        width: parent.width
        height: 96

        color: rootItem.isSelected ? IngeScapeTheme.veryLightGreyColor : IngeScapeAssessmentsTheme.darkerDarkBlueHeader

        Rectangle {
            id: bottomBorder

            anchors {
                left: parent.left
                right: parent.right
                bottom: parent.bottom
            }
            height: 1

            color: IngeScapeTheme.blackColor
        }

        Column {
            anchors {
                verticalCenter: parent.verticalCenter
                left: parent.left
                leftMargin: 28
            }

            spacing: 6

            Text {
                text: rootItem.modelM ? rootItem.modelM.name : ""

                color: rootItem.isSelected ? IngeScapeAssessmentsTheme.darkerDarkBlueHeader : IngeScapeTheme.whiteColor

                font {
                    family: IngeScapeTheme.textFontFamily
                    weight: Font.Medium
                    pixelSize: 20
                }
            }

            Text {
                text: rootItem.modelM ? "Platform: " + rootItem.modelM.platformFileName : ""

                color: IngeScapeTheme.lightGreyColor

                font {
                    family: IngeScapeTheme.textFontFamily
                    pixelSize: 18
                }
            }
        }

        MouseArea {
            id: mouseArea
            anchors.fill: parent
            hoverEnabled: true

            onClicked: {
                // Emit the signal "Select Task"
                rootItem.selectTask();
            }
        }


        Column {
            id: buttonRow

            anchors {
                right: parent.right
                rightMargin: 12
                top: parent.top
                topMargin: 14
            }

            spacing: 8

            opacity: rootItem.isSelected || mouseArea.containsMouse || btnDuplicate.containsMouse || btnDelete.containsMouse ? 1 : 0
            enabled: opacity > 0

            Button {
                id: btnDuplicate

                property bool containsMouse: __behavior.containsMouse

                width: 41
                height: 30

                style: IngeScapeAssessmentsSvgButtonStyle {
                    releasedID: "delete"
                    disabledID: releasedID
                }

                onClicked: {
                    deleteTaskPopup.open()
                }
            }

            Button {
                id: btnDelete

                property bool containsMouse: __behavior.containsMouse

                width: 41
                height: 30

                style: IngeScapeAssessmentsSvgButtonStyle {
                    releasedID: "duplicate"
                    disabledID: releasedID
                }

                onClicked: {
                    // Emit the signal "Duplicate Task"
                    rootItem.duplicateTask();
                }
            }
        }
    }

    Popup.DeleteConfirmationPopup {
        id: deleteTaskPopup

        layerObjectName: "overlay2Layer"

        showPopupTitle: false
        anchors.centerIn: parent

        text: qsTr("Are you sure you want to delete the task %1 ?").arg(rootItem.modelM ? rootItem.modelM.name : "")

        height: 160
        width: 470

        onValidated: {
            // Emit the signal "Delete Task"
            rootItem.deleteTask();
            deleteTaskPopup.close()
        }

        onCanceled: {
            deleteTaskPopup.close()
        }
    }

}
