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
 *      Justine Limoges <limoges@ingenuity.io>
 *      Vincent Peyruqueou <peyruqueou@ingenuity.io>
 */

import QtQuick 2.0

import QtQuick 2.8
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4
import QtQuick.Window 2.3

import I2Quick 1.0

import INGESCAPE 1.0


I2PopupBase {
    id: rootItem

    height: 350
    width: 550

    anchors.centerIn: parent

    isModal: true
    dismissOnOutsideTap: false
    keepRelativePositionToInitialParent: false


    //--------------------------------
    //
    // Signals
    //
    //--------------------------------

    //
    signal cancelMappingActivation();

    //
    signal switchToControl();

    //
    signal stayToObserve();


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
            //horizontalAlignment: Text.AlignHCenter

            text: "Modifications on some links between agents have been made\n
while the mapping was unactivated. Do you want to\n
keep these modifications by passing to mode CONTROL ?\n
to cancel your modifications by staying to mode OBSERVE ?\n
or to cancel the activation of the mapping ?"

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
                bottom: parent.bottom
                bottomMargin: 16
            }
            spacing : 15

            Button {
                id: cancelButton

                anchors {
                    verticalCenter: parent.verticalCenter
                }
                property var boundingBox: IngeScapeTheme.svgFileINGESCAPE.boundsOnElement("button");
                height: boundingBox.height * 1.5
                width: boundingBox.width * 1.5

                text: "Cancel mapping activation"

                activeFocusOnPress: true

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
                    rootItem.close();

                    // Emit the signal "Cancel Mapping Activation"
                    rootItem.cancelMappingActivation();
                }
            }


            Button {
                id: btnControl

                anchors {
                    verticalCenter: parent.verticalCenter
                }

                property var boundingBox: IngeScapeTheme.svgFileINGESCAPE.boundsOnElement("button");
                height: boundingBox.height
                width: boundingBox.width

                text: "Mode CONTROL"

                activeFocusOnPress: true

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
                    rootItem.close();

                    // Emit the signal "Switch to Control"
                    rootItem.switchToControl();
                }
            }


            Button {
                id: btnObserve

                anchors {
                    verticalCenter: parent.verticalCenter
                }

                property var boundingBox: IngeScapeTheme.svgFileINGESCAPE.boundsOnElement("button");
                height: boundingBox.height
                width: boundingBox.width

                text: "Mode OBSERVE"

                activeFocusOnPress: true

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
                    rootItem.close();

                    // Emit the signal "Stay to Observe"
                    rootItem.stayToObserve();
                }
            }
        }
    }
}
