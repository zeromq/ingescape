/*
 *	IngeScape Common
 *
 *  Copyright © 2017-2019 Ingenuity i/o. All rights reserved.
 *
 *	See license terms for the rights and conditions
 *	defined by copyright holders.
 *
 *
 *	Contributors:
 *      Mathieu Soum     <soum@ingenuity.io>
 *
 */

import QtQuick 2.8
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4
import QtQuick.Window 2.3

import I2Quick 1.0

import INGESCAPE 1.0

import "theme" as Theme;

I2PopupBase {
    id: rootPopup

    anchors.centerIn: parent
    height: 200
    width: 360

    isModal: true
    dismissOnOutsideTap: false
    keepRelativePositionToInitialParent: false

    // Licenses controller
    property LicensesController licensesController: null;


    Rectangle {

        anchors {
            fill: parent
        }
        radius: 5
        border {
            width: 2
            color: IngeScapeTheme.editorsBackgroundBorderColor
        }
        color: IngeScapeTheme.editorsBackgroundColor

        Text {
            id: text

            anchors {
                left : parent.left
                right : parent.right
                verticalCenter: parent.verticalCenter
                verticalCenterOffset: -18
                margins: 16
            }

            text: rootPopup.licensesController ? rootPopup.licensesController.errorMessageWhenLicenseFailed : ""

            horizontalAlignment: Text.AlignHCenter
            lineHeight: 24
            lineHeightMode: Text.FixedHeight
            color: IngeScapeTheme.whiteColor
            elide: Text.ElideRight
            wrapMode: Text.WordWrap

            font {
                family: IngeScapeTheme.textFontFamily
                pixelSize: 16
            }
        }

        Button {
            id: okButton

            property var boundingBox: IngeScapeTheme.svgFileIngeScape.boundsOnElement("button");

            height: boundingBox.height
            width: boundingBox.width

            enabled: visible
            activeFocusOnPress: true
            text: "OK"

            anchors {
                bottom: parent.bottom
                bottomMargin: 16
                horizontalCenter: parent.horizontalCenter
            }

            style: I2SvgButtonStyle {
                fileCache: IngeScapeTheme.svgFileIngeScape

                releasedID: "button"
                pressedID: releasedID + "-pressed"
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
                rootPopup.close();
            }
        }
    }
}
