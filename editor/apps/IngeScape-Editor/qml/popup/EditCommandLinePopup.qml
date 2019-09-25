/*
 *	IngeScape Editor
 *
 *  Copyright © 2017-2019 Ingenuity i/o. All rights reserved.
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


I2PopupBase {
    id: rootPopup

    //----------------------------------
    //
    // Properties
    //
    //----------------------------------

    property string agentName: ""

    property string previousCommandLine: ""

    //property alias newCommandLine: textFieldNewCommandLine.text

    anchors.centerIn: parent

    height: 200
    width: 600

    isModal: true
    dismissOnOutsideTap: true
    keepRelativePositionToInitialParent: false


    //--------------------------------
    //
    // Signals
    //
    //--------------------------------

    // Signal emitted when the user clicks on the "OK" button
    signal commandLineValidated(string newCommandLine);

    // Signal emitted when the user clicks on the "Cancel" button
    signal cancelled();


    //--------------------------------
    //
    // Slots
    //
    //--------------------------------

    onOpened: {
        textFieldCommandLine.text = rootPopup.previousCommandLine;
    }



    //----------------------------------
    //
    // Content
    //
    //----------------------------------

    Rectangle {
        anchors.fill: parent

        radius: 5

        border {
            width: 2
            color: IngeScapeTheme.editorsBackgroundBorderColor
        }

        color: IngeScapeTheme.editorsBackgroundColor

        Column {

            anchors {
                left: parent.left
                leftMargin: 20
                right: parent.right
                rightMargin: 20
                top: parent.top
                topMargin: 20
            }

            spacing: 10

            Text {
                id: title

                text: "Edit command line of " + rootPopup.agentName

                horizontalAlignment: Text.AlignHCenter
                lineHeight: 24
                lineHeightMode: Text.FixedHeight
                color: IngeScapeTheme.whiteColor

                font {
                    family: IngeScapeTheme.textFontFamily
                    pixelSize: 16
                }
            }

            TextField {
                id: textFieldCommandLine

                anchors {
                    left: parent.left
                    right: parent.right
                }
                height: 25
                verticalAlignment: TextInput.AlignVCenter

                text: ""

                style: I2TextFieldStyle {
                    backgroundColor: IngeScapeTheme.darkBlueGreyColor
                    borderColor: IngeScapeTheme.whiteColor;
                    borderErrorColor: IngeScapeTheme.redColor
                    radiusTextBox: 1
                    borderWidth: 0;
                    borderWidthActive: 1
                    textIdleColor: IngeScapeTheme.whiteColor;
                    textDisabledColor: IngeScapeTheme.darkGreyColor;

                    padding.left: 3
                    padding.right: 3

                    font {
                        pixelSize:15
                        family: IngeScapeTheme.textFontFamily
                    }

                }
            }

        }


        Button {
            id: btnCancel

            anchors {
                bottom: parent.bottom
                bottomMargin: 20
                right: btnOK.left
                rightMargin: 20
            }

            property var boundingBox: IngeScapeTheme.svgFileIngeScape.boundsOnElement("button");

            height: boundingBox.height
            width: boundingBox.width

            text: "Cancel"
            enabled: true

            activeFocusOnPress: true

            style: I2SvgButtonStyle {
                fileCache: IngeScapeTheme.svgFileIngeScape

                pressedID: releasedID + "-pressed"
                releasedID: "button"
                disabledID: releasedID + "-disabled"

                font {
                    family: IngeScapeTheme.textFontFamily
                    weight: Font.Medium
                    pixelSize: 16
                }
                labelColorPressed: IngeScapeTheme.blackColor
                labelColorReleased: IngeScapeTheme.whiteColor
                labelColorDisabled: IngeScapeTheme.disabledTextColor

            }

            onClicked: {
                // Close our popup
                rootPopup.close();
            }
        }


        Button {
            id: btnOK

            anchors {
                bottom: parent.bottom
                bottomMargin: 20
                right: parent.right
                rightMargin: 20
            }

            property var boundingBox: IngeScapeTheme.svgFileIngeScape.boundsOnElement("button");

            height: boundingBox.height
            width: boundingBox.width

            text: "OK"
            enabled: true

            activeFocusOnPress: true

            style: I2SvgButtonStyle {
                fileCache: IngeScapeTheme.svgFileIngeScape

                pressedID: releasedID + "-pressed"
                releasedID: "button"
                disabledID: releasedID + "-disabled"

                font {
                    family: IngeScapeTheme.textFontFamily
                    weight : Font.Medium
                    pixelSize : 16
                }
                labelColorPressed: IngeScapeTheme.blackColor
                labelColorReleased: IngeScapeTheme.whiteColor
                labelColorDisabled: IngeScapeTheme.disabledTextColor
            }

            onClicked: {
                commandLineValidated(textFieldCommandLine.text);

                // Close our popup
                rootPopup.close();
            }
        }
    }
}
