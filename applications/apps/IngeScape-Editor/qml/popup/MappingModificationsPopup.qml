/*
 *	IngeScape Editor
 *
 *  Copyright © 2017-2020 Ingenuity i/o. All rights reserved.
 *
 *	See license terms for the rights and conditions
 *	defined by copyright holders.
 *
 *	Contributors:
 *      Justine Limoges <limoges@ingenuity.io>
 *      Vincent Peyruqueou <peyruqueou@ingenuity.io>
 *      Chloé Roumieu      <roumieu@ingenuity.io>
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
    height: 230
    width: 750

    anchors.centerIn: parent

    isModal: true
    dismissOnOutsideTap: false
    keepRelativePositionToInitialParent: false


    //--------------------------------
    //
    // Signals
    //
    //--------------------------------

    signal stayDisconnect()
    signal chooseImposeMapping()
    signal chooseNotImposeMapping()


    //--------------------------------
    //
    // Slots
    //
    //--------------------------------

    onOpened: {
        // Reset check-boxes
        checkNotImpose.checked = false;
        checkImpose.checked = false;
    }


    //--------------------------------
    //
    // Content
    //
    //--------------------------------

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
                top: parent.top
                topMargin: 20
            }
            spacing: 15

            Text {
                id: title
                anchors.left: parent.left
                height: 25

                text: qsTr("You are about to connect to the network. For agents that arrived on the network, you can choose to :")
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
                id: checkNotImpose
                anchors.left: parent.left
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

                        text:qsTr("NOT impose your platform's mapping on them (and maybe lose your platform's mappings)")
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
                id: checkImpose
                anchors.left: parent.left
                height: 25

                checked: false
                exclusiveGroup: exclusiveGroup
                activeFocusOnPress: true

                style: CheckBoxStyle {
                    label: Text {
                        anchors {
                            verticalCenter: parent.verticalCenter
                            verticalCenterOffset: 2
                        }
                        color: control.enabled ? (control.checked ? IngeScapeTheme.whiteColor : IngeScapeTheme.lightGreyColor)
                                               : IngeScapeTheme.disabledTextColor

                        text: qsTr("Impose your platform's mapping on them (at the risk of losing existing mapping on the network)")
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
            id: btnCancel
            anchors {
                bottom: parent.bottom
                bottomMargin: 20
                right: btnOK.left
                rightMargin: 20
            }

            property var boundingBox: IngeScapeTheme.svgFileIngeScape.boundsOnElement("button");

            height: boundingBox.height
            width: 160

            text: "Stay disconnected"
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
                rootItem.close();
                rootItem.stayDisconnect();
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
            width: 80

            text: "Connect"
            enabled: checkImpose.checked || checkNotImpose.checked

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
                rootItem.close();
                // Raise right signal
                if (checkImpose.checked)
                {
                    rootItem.chooseImposeMapping();
                }
                else if (checkNotImpose.checked)
                {
                    rootItem.chooseNotImposeMapping();
                }
            }
        }
    }
}
