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
 *      Justine Limoges <limoges@ingenuity.io>
 *      Vincent Peyruqueou <peyruqueou@ingenuity.io>
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
    id: rootItem

    height: 150
    width: 450

    anchors.centerIn: parent

    isModal: true
    dismissOnOutsideTap: false
    keepRelativePositionToInitialParent: false


    property alias confirmationText: confirmationText.text

    //--------------------------------
    //
    // Signals
    //
    //--------------------------------

    // Signal emitted when the user clicks on the "YES" button
    signal confirmed();

    // Signal emitted when the user clicks on the "NO" button
    signal cancelled();


    //--------------------------------
    //
    // Content
    //
    //--------------------------------

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
            id: confirmationText

            anchors {
                left : parent.left
                right : parent.right
                verticalCenter: parent.verticalCenter
                verticalCenterOffset: -18
            }
            text: "TODO 1...\nDo you want to TODO 2?"
            horizontalAlignment: Text.AlignHCenter
            lineHeight: 24
            lineHeightMode: Text.FixedHeight
            color: IngeScapeTheme.whiteColor
            elide: Text.ElideRight

            font {
                family: IngeScapeTheme.textFontFamily
                pixelSize: 16
            }
        }

        Row {
            anchors {
                horizontalCenter: parent.horizontalCenter
                bottom : parent.bottom
                bottomMargin: 16
            }
            spacing : 15

            Button {
                id: cancelButton
                activeFocusOnPress: true

                property var boundingBox: IngeScapeTheme.svgFileIngeScape.boundsOnElement("button");

                height: boundingBox.height
                width: boundingBox.width

                enabled: visible
                text: "NO"

                anchors {
                    verticalCenter: parent.verticalCenter
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
                    rootItem.close();

                    // Emit the signal "Cancelled"
                    rootItem.cancelled();
                }
            }

            Button {
                id: okButton

                property var boundingBox: IngeScapeTheme.svgFileIngeScape.boundsOnElement("button");

                height: boundingBox.height
                width: boundingBox.width

                enabled: visible
                activeFocusOnPress: true
                text: "YES"

                anchors {
                    verticalCenter: parent.verticalCenter
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
                    rootItem.close();

                    // Emit the signal "Confirmed"
                    rootItem.confirmed();
                }
            }
        }
    }

}
