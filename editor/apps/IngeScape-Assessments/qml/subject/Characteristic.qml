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

    //anchors.fill: parent

    width: parent.width
    height: rowHeader.height + columnEnumValues.anchors.topMargin + columnEnumValues.height


    //--------------------------------------------------------
    //
    //
    // Properties
    //
    //
    //--------------------------------------------------------

    property CharacteristicM modelM: null;



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

        color: "transparent"
        border {
            color: IngeScapeTheme.darkGreyColor
            width: 1
        }
    }

    Row {
        id: rowHeader

        height: 30

        spacing: 20

        anchors {
            left: parent.left
            leftMargin: 5
            top: parent.top
        }

        Text {
            anchors.verticalCenter: parent.verticalCenter

            text: rootItem.modelM ? rootItem.modelM.name : ""

            color: IngeScapeTheme.whiteColor
            font {
                family: IngeScapeTheme.textFontFamily
                weight: Font.Medium
                pixelSize: 14
            }
        }

        Text {
            anchors.verticalCenter: parent.verticalCenter

            text: rootItem.modelM ? CharacteristicValueTypes.enumToString(rootItem.modelM.valueType) : ""

            color: IngeScapeTheme.whiteColor
            font {
                family: IngeScapeTheme.textFontFamily
                //weight: Font.Medium
                pixelSize: 12
            }
        }
    }

    Column {
        id: columnEnumValues

        anchors {
            left: parent.left
            leftMargin: 5
            top: rowHeader.bottom
            topMargin: 5
        }

        visible: (rootItem.modelM && (rootItem.modelM.valueType === CharacteristicValueTypes.CHARACTERISTIC_ENUM))

        spacing: 3

        Repeater {
            model: rootItem.modelM ? rootItem.modelM.enumValues : null

            delegate: Text {
                text: "- " + modelData

                color: IngeScapeTheme.whiteColor
                font {
                    family: IngeScapeTheme.textFontFamily
                    //weight: Font.Medium
                    pixelSize: 12
                }
            }
        }
    }

    /*MouseArea {
            id: mouseArea

            anchors.fill: parent

            hoverEnabled: true
        }*/

    Button {
        id: btnDelete

        anchors {
            top: parent.top
            topMargin: 2
            right: parent.right
            rightMargin: 2
        }
        height: 30

        text: "DEL"

        visible: !rootItem.modelM.isSubjectId

        onClicked: {
            if (rootItem.modelM)
            {
                //console.log("QML: Delete Characteristic " + rootItem.modelM.name);

                // Emit the signal "Delete Characteristic"
                rootItem.deleteCharacteristic();
            }
        }
    }
}
