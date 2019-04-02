/*
 *	IngeScape Measuring
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
    height: 30


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

        color: "#44222222"
        border {
            color: "black"
            width: 1
        }
    }

    Row {
        spacing: 30

        anchors {
            left: parent.left
            verticalCenter: parent.verticalCenter
        }

        Text {
            text: rootItem.modelM ? rootItem.modelM.name : ""

            color: IngeScapeTheme.whiteColor
            font {
                family: IngeScapeTheme.textFontFamily
                weight : Font.Medium
                pixelSize : 12
            }
        }

        Text {
            text: rootItem.modelM ? CharacteristicValueTypes.enumToString(rootItem.modelM.valueType) : ""

            color: IngeScapeTheme.whiteColor
            font {
                family: IngeScapeTheme.textFontFamily
                //weight : Font.Medium
                pixelSize : 12
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
            right: parent.right
        }
        //width: 100

        text: "Delete"

        //visible: mouseArea.containsMouse

        onClicked: {
            if (rootItem.modelM)
            {
                console.log("QML: Delete Characteristic " + rootItem.modelM.name);

                // Emit the signal "Delete Characteristic"
                rootItem.deleteCharacteristic();
            }
        }
    }
}
