/*
 *	IngeScape Editor
 *
 *  Copyright © 2017 Ingenuity i/o. All rights reserved.
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
    id: popupMessage

    height: 150
    width: 350

    isModal: true
    dismissOnOutsideTap: true
    keepRelativePositionToInitialParent: false

    Rectangle {

        anchors.fill: parent
        radius: 5
        border {
            width: 2
            color: IngeScapeTheme.editorsBackgroundBorderColor
        }
        color: IngeScapeTheme.editorsBackgroundColor

        Text {
            id: popupText

            text: "The file does not contain valid agent definition(s) !"

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
                horizontalCenter: parent.horizontalCenter
                bottom : parent.bottom
                bottomMargin: 16
            }

            property var boundingBox: IngeScapeTheme.svgFileIngeScape.boundsOnElement("button");

            height: boundingBox.height
            width: boundingBox.width

            activeFocusOnPress: true
            text: "OK"

            style: I2SvgButtonStyle {
                fileCache: IngeScapeTheme.svgFileINGESCAPE

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
                // Close our popup
                popupMessage.close();
            }
        }
    }
}
