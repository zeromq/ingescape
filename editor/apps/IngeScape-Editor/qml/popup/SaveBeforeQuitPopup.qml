/*
 *	IngeScape Editor
 *
 *  Copyright © 2019 Ingenuity i/o. All rights reserved.
 *
 *	See license terms for the rights and conditions
 *	defined by copyright holders.
 *
 *
 *	Contributors:
 *      Mathieu Soum      <soum@ingenuity.io>
 *
 */

import QtQuick 2.8
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4

import I2Quick 1.0

import INGESCAPE 1.0


I2PopupBase {
    id: popupMessage

    //----------------------------------
    //
    // Properties
    //
    //----------------------------------


    height: 135
    width: 450

    isModal: true
    dismissOnOutsideTap: true
    keepRelativePositionToInitialParent: false


    //----------------------------------
    //
    // Signals
    //
    //----------------------------------


    signal cancel()
    signal discard()
    signal saveAs()


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
    }

    Text {
        id: popupText

        text: "Changes in the current platform have not yet been saved."

        anchors {
            left: parent.left
            right: parent.right
            verticalCenter: parent.verticalCenter
            verticalCenterOffset: -20
        }

        horizontalAlignment: Text.AlignHCenter
        lineHeight: 24
        lineHeightMode: Text.FixedHeight
        color: IngeScapeTheme.whiteColor

        font {
            family: IngeScapeTheme.textFontFamily
            pixelSize: 16
        }
    }

    Button {
        anchors {
            left: parent.left
            leftMargin: 16
            bottom : parent.bottom
            bottomMargin: 16
        }

        property var boundingBox: IngeScapeTheme.svgFileIngeScape.boundsOnElement("button");

        height: boundingBox.height
        width: 115

        activeFocusOnPress: true
        text: "Do not save"

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
            labelColorDisabled: IngeScapeTheme.whiteColor
        }

        onClicked: {
            discard();

            // Close our popup
            popupMessage.close();
        }
    }

    Row {
        anchors {
            right: parent.right
            rightMargin: 16
            bottom : parent.bottom
            bottomMargin: 16
        }
        spacing: 12

        Button {
            property var boundingBox: IngeScapeTheme.svgFileIngeScape.boundsOnElement("button");

            height: boundingBox.height
            width: boundingBox.width

            activeFocusOnPress: true
            text: "Cancel"

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
                labelColorDisabled: IngeScapeTheme.whiteColor
            }

            onClicked: {
                cancel();

                // Close our popup
                popupMessage.close();
            }
        }

        Button {
            property var boundingBox: IngeScapeTheme.svgFileIngeScape.boundsOnElement("button");

            height: boundingBox.height
            width: 110

            activeFocusOnPress: true
            text: "Save as..."


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
                labelColorDisabled: IngeScapeTheme.whiteColor
            }

            onClicked: {
                saveAs();

                // Close our popup
                popupMessage.close();

            }
        }
    }

}
