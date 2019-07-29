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

//import "theme" as Theme


Item {
    id: rootItem

    height: 73


    //--------------------------------------------------------
    //
    //
    // Properties
    //
    //
    //--------------------------------------------------------

    property CharacteristicM modelM: null;
    property bool isMouseHovering: mouseArea.containsMouse || btnDelete.containsMouse;



    //--------------------------------
    //
    //
    // Signals
    //
    //
    //--------------------------------

    // Delete Characteristic
    signal deleteCharacteristic();



    //--------------------------------------------------------
    //
    //
    // Content
    //
    //
    //--------------------------------------------------------

    Rectangle {
        id: background
        anchors.fill: parent
        color: rootItem.isMouseHovering ? IngeScapeTheme.veryLightGreyColor : IngeScapeTheme.whiteColor
    }

    MouseArea {
        id: mouseArea
        anchors.fill: parent
        hoverEnabled: true
    }

    Column {
        id: rowHeader

        anchors {
            verticalCenter: parent.verticalCenter
            left: parent.left
            right: parent.right
            leftMargin: 28
        }

        spacing: 8

        Text {
            text: rootItem.modelM ? rootItem.modelM.name : ""

            color: IngeScapeTheme.blackColor
            font {
                family: IngeScapeTheme.textFontFamily
                weight: Font.Medium
                pixelSize: 18
            }
        }

        Text {
            text: rootItem.modelM
                  ? CharacteristicValueTypes.enumToString(rootItem.modelM.valueType)
                    + ((rootItem.modelM.valueType === CharacteristicValueTypes.CHARACTERISTIC_ENUM) ? "{ " + rootItem.modelM.enumValues.join(" ; ") + " }"
                                                                                                 : "")
                  : ""

            color: IngeScapeTheme.blackColor
            font {
                family: IngeScapeTheme.textFontFamily
                pixelSize: 16
            }
        }
    }

    Button {
        id: btnDelete

        anchors {
            verticalCenter: parent.verticalCenter
            right: parent.right
            rightMargin: 16
        }
        height: 30
        width: 40

        property bool containsMouse: __behavior.containsMouse

        text: "DEL"

        opacity: rootItem.modelM && !rootItem.modelM.isSubjectId && rootItem.isMouseHovering ? 1 : 0
        enabled: opacity > 0

        onClicked: {
            if (rootItem.modelM)
            {
                //console.log("QML: Delete Characteristic " + rootItem.modelM.name);

                // Emit the signal "Delete Characteristic"
                rootItem.deleteCharacteristic();
            }
        }
    }

    Rectangle {
        id: bottomSeparator
        anchors {
            left: parent.left
            right: parent.right
            bottom: parent.bottom
        }
        height: 2
        color: IngeScapeTheme.veryLightGreyColor
    }
}
