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

// Needed to access to ToolTip (https://doc.qt.io/qt-5.11/qml-qtquick-controls2-tooltip.html)
import QtQuick.Controls 2.0 as Controls2

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

    property ProtocolM modelM: null;

    property bool isSelected: false;



    //--------------------------------
    //
    //
    // Signals
    //
    //
    //--------------------------------

    // Signal emitted when the user clicks on our protocol
    signal selectProtocolAsked();

    // Signal emitted when the user clicks on the "duplicate" button
    signal duplicateProtocolAsked();

    // Signal emitted when the user clicks on the "delete" button
    signal deleteProtocolAsked();



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
                leftMargin: 25
                right: parent.right
                rightMargin: 15
            }

            spacing: 10

            Text {
                anchors {
                    left: parent.left
                    right: parent.right
                }

                text: rootItem.modelM ? rootItem.modelM.name : ""
                elide: Text.ElideRight

                color: rootItem.isSelected ? IngeScapeAssessmentsTheme.darkerDarkBlueHeader : IngeScapeTheme.whiteColor

                font {
                    family: IngeScapeTheme.textFontFamily
                    weight: Font.Medium
                    pixelSize: 20
                }
            }

            Text {
                anchors {
                    left: parent.left
                    right: parent.right
                }

                text: rootItem.modelM ? rootItem.modelM.platformFileName : ""
                elide: Text.ElideRight

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
                // Emit the signal "Select Protocol"
                rootItem.selectProtocolAsked();
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
                id: btnDelete

                property bool containsMouse: __behavior.containsMouse

                width: 41
                height: 30

                style: IngeScapeAssessmentsSvgButtonStyle {
                    releasedID: "delete"
                    disabledID: releasedID
                }

                onClicked: {
                    deleteProtocolPopup.open()
                }

                Controls2.ToolTip {
                    visible: btnDelete.hovered
                    text: qsTr("delete")
                    delay: 800
                }
            }

            Button {
                id: btnDuplicate

                property bool containsMouse: __behavior.containsMouse

                width: 41
                height: 30

                style: IngeScapeAssessmentsSvgButtonStyle {
                    releasedID: "duplicate"
                    disabledID: releasedID
                }

                onClicked: {
                    // Emit the signal "Duplicate Task asked"
                    rootItem.duplicateProtocolAsked();
                }

                Controls2.ToolTip {
                    visible: btnDuplicate.hovered
                    text: qsTr("duplicate")
                    delay: 800
                }
            }
        }
    }

    Popup.DeleteConfirmationPopup {
        id: deleteProtocolPopup

        layerObjectName: "overlay2Layer"

        showPopupTitle: false
        anchors.centerIn: parent

        text: qsTr("Are you sure you want to delete the task %1 ?").arg(rootItem.modelM ? rootItem.modelM.name : "")

        height: 160
        width: 470

        onValidated: {
            // Emit the signal "Delete Protocol asked"
            rootItem.deleteProtocolAsked();

            deleteProtocolPopup.close()
        }

        onCanceled: {
            deleteProtocolPopup.close()
        }
    }

}
