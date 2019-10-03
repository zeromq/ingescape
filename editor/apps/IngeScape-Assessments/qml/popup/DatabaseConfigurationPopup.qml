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


AssessmentsPopupBase {
    id: rootPopup

    height: 330
    width: 626

    anchors.centerIn: parent

    title: "DATABASE CONFIGURATION"


    //--------------------------------------------------------
    //
    //
    // Properties
    //
    //
    //--------------------------------------------------------

    property AssessmentsModelManager modelManager: null;


    //--------------------------------
    //
    //
    // Slots
    //
    //
    //--------------------------------

    onOpened: {
        if (modelManager)
        {
            txtDatabaseAddress.text = modelManager.databaseAddress;
        }
    }


    //--------------------------------
    //
    //
    // Functions
    //
    //
    //--------------------------------


    //--------------------------------
    //
    //
    // Content
    //
    //
    //--------------------------------


    Item {
        id: nameRow

        anchors {
            top: parent.top
            topMargin: 30
            left: parent.left
            leftMargin: 28
            right: parent.right
            rightMargin: 28
        }

        //height: 30

        Text {
            id: lblDatabaseAddress

            anchors {
                left: parent.left
                top: parent.top
                //verticalCenter: parent.verticalCenter
            }

            height: 30

            text: qsTr("Database IP address (or 'localhost'):")

            verticalAlignment: Text.AlignVCenter

            color: IngeScapeAssessmentsTheme.regularDarkBlueHeader
            font {
                family: IngeScapeTheme.textFontFamily
                weight: Font.Medium
                pixelSize: 16
            }
        }

        TextField {
            id: txtDatabaseAddress

            anchors {
                left: parent.left
                right: parent.right
                top: lblDatabaseAddress.bottom
                topMargin: 5
                //verticalCenter: parent.verticalCenter
            }

            height: 30

            text: ""

            style: I2TextFieldStyle {
                backgroundColor: IngeScapeTheme.veryLightGreyColor
                borderColor: IngeScapeAssessmentsTheme.blueButton
                borderErrorColor: IngeScapeTheme.redColor
                radiusTextBox: 5
                borderWidth: 0;
                borderWidthActive: 2
                textIdleColor: IngeScapeAssessmentsTheme.regularDarkBlueHeader;
                textDisabledColor: IngeScapeAssessmentsTheme.lighterDarkBlueHeader

                padding.left: 16
                padding.right: 16

                font {
                    pixelSize: 16
                    family: IngeScapeTheme.textFontFamily
                }
            }
        }


        Rectangle {
            id: stateFeedback

            anchors {
                left: parent.left
                verticalCenter: textStateMessage.verticalCenter
            }

            width: 20
            height: 20

            color: (rootPopup.modelManager && rootPopup.modelManager.isConnectedToDatabase) ? "green" : "red"
            radius: 10
        }

        // State message
        Text {
            id: textStateMessage

            anchors {
                top: txtDatabaseAddress.bottom
                topMargin: 15
                left: stateFeedback.right
                leftMargin: 10
                right: parent.right
            }

            wrapMode: Text.WordWrap

            color: (rootPopup.modelManager && rootPopup.modelManager.isConnectedToDatabase) ? IngeScapeAssessmentsTheme.regularDarkBlueHeader : IngeScapeTheme.orangeColor

            font {
                family: IngeScapeTheme.textFontFamily
                bold: true
                pixelSize: 16
            }

            text: rootPopup.modelManager ? rootPopup.modelManager.errorMessageWhenDatabaseConnectionFailed : ""

            //visible: (text.length !== 0)
        }


    }


    Row {
        anchors {
            right: parent.right
            rightMargin: 28
            bottom : parent.bottom
            bottomMargin: 28
        }
        spacing : 15

        Button {
            id: cancelButton

            property var boundingBox: IngeScapeTheme.svgFileIngeScape.boundsOnElement("button");

            anchors {
                verticalCenter: parent.verticalCenter
            }

            height: boundingBox.height
            width: boundingBox.width

            activeFocusOnPress: true

            style: ButtonStyle {
                background: Rectangle {
                    anchors.fill: parent
                    radius: 5
                    color: control.pressed ? IngeScapeTheme.lightGreyColor : (control.hovered ? IngeScapeTheme.veryLightGreyColor : "transparent")
                }

                label: Text {
                    text: "Cancel"
                    verticalAlignment: Text.AlignVCenter
                    horizontalAlignment: Text.AlignHCenter
                    color: IngeScapeAssessmentsTheme.regularDarkBlueHeader

                    font {
                        family: IngeScapeTheme.textFontFamily
                        weight: Font.Medium
                        pixelSize: 16
                    }
                }
            }

            onClicked: {
                // Close the popup
                rootPopup.close();
            }
        }

        Button {
            id: okButton

            property var boundingBox: IngeScapeTheme.svgFileIngeScape.boundsOnElement("button");

            anchors {
                verticalCenter: parent.verticalCenter
            }

            height: boundingBox.height
            width: boundingBox.width

            activeFocusOnPress: true

            enabled: rootPopup.modelManager && (rootPopup.modelManager.databaseAddress !== txtDatabaseAddress.text)

            style: IngeScapeAssessmentsButtonStyle {
                text: "OK"
            }

            onClicked: {
                //console.log("Update database address " + txtDatabaseAddress.text);

                if (rootPopup.modelManager)
                {
                    rootPopup.modelManager.databaseAddress = txtDatabaseAddress.text;

                    rootPopup.modelManager.connectToDatabase();

                    if (rootPopup.modelManager.isConnectedToDatabase)
                    {
                        console.log("QML: Connected to the Cassandra database on " + rootPopup.modelManager.databaseAddress);

                        // Close the popup
                        //rootPopup.close();
                    }
                    else
                    {
                        console.warn("QML: Could not connect to the Cassandra database on " + rootPopup.modelManager.databaseAddress);
                    }
                }
            }
        }
    }
}
