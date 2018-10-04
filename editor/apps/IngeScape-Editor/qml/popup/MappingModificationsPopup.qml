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

    height: 250
    width: 360

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
    // Slots
    //
    //--------------------------------

    onOpened: {
        // Reset check-boxes
        checkControl.checked = false;
        checkObserve.checked = false;
        checkCancel.checked = false;
    }


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

        Column {
            anchors {
                left: parent.left
                leftMargin: 20
                top: parent.top
                topMargin: 20
            }
            spacing: 15

            Text {
                id: title

                anchors {
                    left: parent.left
                }
                height: 25

                //horizontalAlignment: Text.AlignHCenter

                text: qsTr("Mapping has changed while disconnected. You can")

                color: IngeScapeTheme.whiteColor
                font {
                    family: IngeScapeTheme.textFontFamily
                    pixelSize: 16
                }
            }

            ExclusiveGroup {
                id: exclusiveGroup
            }

            CheckBox {
                id: checkControl

                anchors {
                    left: parent.left
                }
                height: 25

                checked: false
                exclusiveGroup: exclusiveGroup
                activeFocusOnPress: true;

                style: CheckBoxStyle {
                    label: Text {
                        anchors {
                            verticalCenter: parent.verticalCenter
                            verticalCenterOffset: 2
                        }
                        color: control.enabled ? (control.checked ? IngeScapeTheme.whiteColor : IngeScapeTheme.lightGreyColor)
                                               : IngeScapeTheme.disabledTextColor

                        text: qsTr("Apply modified mapping to the platform")

                        font {
                            family: IngeScapeTheme.textFontFamily
                            pixelSize: 16
                        }
                    }

                    indicator: Rectangle {
                        implicitWidth: 14
                        implicitHeight: 14
                        radius: height / 2
                        border.width: 0
                        color: control.enabled ? IngeScapeTheme.darkBlueGreyColor : IngeScapeTheme.disabledTextColor

                        Rectangle {
                            anchors.centerIn: parent
                            visible: control.checked
                            width: 8
                            height: 8
                            radius: height / 2
                            border.width: 0
                            color: IngeScapeTheme.whiteColor
                        }
                    }
                }
            }

            CheckBox {
                id: checkObserve

                anchors {
                    left: parent.left
                }
                height: 25

                checked: false
                exclusiveGroup: exclusiveGroup
                activeFocusOnPress: true;

                style: CheckBoxStyle {
                    label: Text {
                        anchors {
                            verticalCenter: parent.verticalCenter
                            verticalCenterOffset: 2
                        }
                        color: control.enabled ? (control.checked ? IngeScapeTheme.whiteColor : IngeScapeTheme.lightGreyColor)
                                               : IngeScapeTheme.disabledTextColor

                        text: qsTr("Go back to the current mapping")

                        font {
                            family: IngeScapeTheme.textFontFamily
                            pixelSize: 16
                        }
                    }

                    indicator: Rectangle {
                        implicitWidth: 14
                        implicitHeight: 14
                        radius: height / 2
                        border.width: 0
                        color: control.enabled ? IngeScapeTheme.darkBlueGreyColor : IngeScapeTheme.disabledTextColor

                        Rectangle {
                            anchors.centerIn: parent
                            visible: control.checked
                            width: 8
                            height: 8
                            radius: height / 2
                            border.width: 0
                            color: IngeScapeTheme.whiteColor
                        }
                    }
                }
            }

            CheckBox {
                id: checkCancel

                anchors {
                    left: parent.left
                }
                height: 25

                checked: false
                exclusiveGroup: exclusiveGroup
                activeFocusOnPress: true;

                style: CheckBoxStyle {
                    label: Text {
                        anchors {
                            verticalCenter: parent.verticalCenter
                            verticalCenterOffset: 2
                        }
                        color: control.enabled ? (control.checked ? IngeScapeTheme.whiteColor : IngeScapeTheme.lightGreyColor)
                                               : IngeScapeTheme.disabledTextColor

                        text: qsTr("Remain disconnected")

                        font {
                            family: IngeScapeTheme.textFontFamily
                            pixelSize: 16
                        }
                    }

                    indicator: Rectangle {
                        implicitWidth: 14
                        implicitHeight: 14
                        radius: height / 2
                        border.width: 0
                        color: control.enabled ? IngeScapeTheme.darkBlueGreyColor : IngeScapeTheme.disabledTextColor

                        Rectangle {
                            anchors.centerIn: parent
                            visible: control.checked
                            width: 8
                            height: 8
                            radius: height / 2
                            border.width: 0
                            color: IngeScapeTheme.whiteColor
                        }
                    }
                }
            }
        }

        Button {
            id: btnControl

            anchors {
                bottom: parent.bottom
                bottomMargin: 20
                right: parent.right
                rightMargin: 20
            }

            property var boundingBox: IngeScapeTheme.svgFileINGESCAPE.boundsOnElement("button");
            height: boundingBox.height
            width: boundingBox.width

            text: "OK"
            enabled: checkControl.checked || checkObserve.checked || checkCancel.checked

            activeFocusOnPress: true

            style: I2SvgButtonStyle {
                fileCache: IngeScapeTheme.svgFileINGESCAPE

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
                rootItem.close();

                // CONTROL
                if (checkControl.checked)
                {
                    // Emit the signal "Switch to Control"
                    rootItem.switchToControl();
                }
                // OBSERVE
                else if (checkObserve.checked)
                {
                    // Emit the signal "Stay to Observe"
                    rootItem.stayToObserve();
                }
                else if (checkCancel.checked)
                {
                    // Emit the signal "Cancel Mapping Activation"
                    rootItem.cancelMappingActivation();
                }
            }
        }
    }
}
