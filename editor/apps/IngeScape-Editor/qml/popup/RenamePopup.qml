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

    property string previousHostName: ""
    //property alias newHostName: popupText.text


    height: 150
    width: 350

    isModal: true
    dismissOnOutsideTap: true
    keepRelativePositionToInitialParent: false



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

        Text {
            id: title

            text: "Rename host " + rootPopup.previousHostName

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
                // Close our popup
                rootPopup.close();
            }
        }
    }
}
