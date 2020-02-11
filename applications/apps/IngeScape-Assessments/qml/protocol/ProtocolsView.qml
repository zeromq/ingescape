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

    property ProtocolsController protocolsController: null;

    property ExperimentationM experimentation: protocolsController ? protocolsController.currentExperimentation : null;


    //--------------------------------
    //
    //
    // Signals
    //
    //
    //--------------------------------

    // Close Protocols view
    //signal closeProtocolsView();



    //--------------------------------
    //
    //
    // Slots
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
        color: IngeScapeAssessmentsTheme.darkerDarkBlueHeader

        radius: 5
    }

    Item {
        id: leftPart

        anchors {
            top: parent.top
            topMargin: 24
            left: parent.left
            bottom: parent.bottom
        }

        width: 340

        Text {
            id: titleProtocols

            anchors {
                verticalCenter: btnNewProtocol.verticalCenter
                left: parent.left
                leftMargin: 28
            }

            text: qsTr("PROTOCOLS")

            height: parent.height
            verticalAlignment: Text.AlignVCenter

            color: IngeScapeTheme.whiteColor
            font {
                family: IngeScapeTheme.labelFontFamily
                weight: Font.Black
                pixelSize: 20
            }
        }

        Button {
            id: btnNewProtocol

            anchors {
                top: parent.top
                right: parent.right
                rightMargin: 12
            }

            height: 40
            width: 134

            onClicked: {
                // Open the popup
                createProtocolPopup.open();
            }

            style: IngeScapeAssessmentsButtonStyle {
                text: qsTr("ADD NEW")
            }
        }

        Rectangle {
            id: topSeparator
            anchors {
                bottom: protocolsList.top
                left: parent.left
                right: parent.right
            }

            height: 1
            color: IngeScapeTheme.blackColor
        }

        ListView {
            id: protocolsList

            anchors {
                top: btnNewProtocol.bottom
                topMargin: 14
                left: parent.left
                right: parent.right
                bottom: parent.bottom
            }

            model: rootItem.experimentation ? rootItem.experimentation.allProtocols : null

            delegate: ProtocolInList {
                modelM: model.QtObject
                isSelected: rootItem.protocolsController && rootItem.protocolsController.selectedProtocol && (modelM === rootItem.protocolsController.selectedProtocol)

                //
                // Slots
                //
                onSelectProtocolAsked: {
                    if (rootItem.protocolsController)
                    {
                        // First, select the protocol
                        rootItem.protocolsController.selectedProtocol = model.QtObject;
                    }
                }

                onDeleteProtocolAsked: {
                    if (rootItem.protocolsController)
                    {
                        // Delete the protocol
                        rootItem.protocolsController.deleteProtocol(model.QtObject);
                    }
                }

                onDuplicateProtocolAsked: {
                    if (rootItem.protocolsController)
                    {
                        // Duplicate the protocol
                        rootItem.protocolsController.duplicateProtocol(model.QtObject);
                    }
                }
            }
        }
    }

    //
    // Protocol
    //
    Rectangle {
        id: rectProtocol

        anchors {
            top: parent.top
            bottom: parent.bottom
            left: leftPart.right
            right: parent.right
            rightMargin: 5
        }

        color: IngeScapeTheme.veryLightGreyColor

        Rectangle {
            anchors {
                top: parent.top
                bottom: parent.bottom
                horizontalCenter: parent.right
            }

            width: 10

            color: IngeScapeTheme.veryLightGreyColor
            radius: 5
        }


        Protocol {
            id: protocol

            anchors.fill: parent

            protocolsController: rootItem.protocolsController
        }


        //
        // Close button
        //
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
                if (rootItem.protocolsController) {
                    // UN-select the protocol
                    rootItem.protocolsController.selectedProtocol = null;
                }

                console.log("QML: close Protocols view");

                // Emit the signal "close protocols view"
                //rootItem.closeProtocolsView();

                close();
            }
        }
    }


    //
    // Create Protocol Popup
    //
    Popup.CreateProtocolPopup {
        id: createProtocolPopup

        layerObjectName: "overlay2Layer"

        protocolsController: rootItem.protocolsController
    }

}
