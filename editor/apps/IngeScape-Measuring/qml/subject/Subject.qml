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
    height: 25


    //--------------------------------------------------------
    //
    //
    // Properties
    //
    //
    //--------------------------------------------------------

    property SubjectM modelM: null;

    property var allCharacteristics: null;

    property bool isCurrentlyEditing: false;


    //--------------------------------
    //
    //
    // Signals
    //
    //
    //--------------------------------

    // Delete Subject
    signal deleteSubject();



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

        /*Text {
            id: txtUID

            anchors {
                left: parent.left
                leftMargin: 5
                verticalCenter: parent.verticalCenter
            }
            width: 150

            text: rootItem.modelM ? rootItem.modelM.uid : ""

            color: IngeScapeTheme.whiteColor
            font {
                family: IngeScapeTheme.textFontFamily
                //weight : Font.Medium
                pixelSize : 12
            }
        }*/

        /*Text {
            id: txtName

            anchors {
                left: txtUID.right
                leftMargin: 5
                verticalCenter: parent.verticalCenter
            }
            width: 150

            text: rootItem.modelM ? rootItem.modelM.name : ""

            color: IngeScapeTheme.whiteColor
            font {
                family: IngeScapeTheme.textFontFamily
                //weight : Font.Medium
                pixelSize : 12
            }
        }*/

        Row {
            anchors.fill: parent

            spacing: 0

            Repeater {
                model: rootItem.allCharacteristics

                delegate: Rectangle {
                    id: valueColumn

                    anchors {
                        top: parent.top
                        bottom: parent.bottom
                    }
                    width: 150

                    color: "transparent"
                    border {
                        color: "black"
                        width: 1
                    }

                    Text {

                        anchors.centerIn: parent

                        text: rootItem.modelM ? rootItem.modelM.getValueOfCharacteristic(model.name) : ""

                        color: IngeScapeTheme.whiteColor
                        font {
                            family: IngeScapeTheme.textFontFamily
                            //weight : Font.Medium
                            pixelSize : 12
                        }
                    }
                }
            }
        }
    }

    Button {
        id: btnDelete

        anchors {
            //top: parent.top
            right: parent.right
        }
        //width: 100
        height: parent.height

        text: "Delete"

        //visible: mouseArea.containsMouse

        onClicked: {
            if (rootItem.modelM)
            {
                //console.log("QML: Delete Subject " + rootItem.modelM.uid);

                // Emit the signal "Delete Subject"
                rootItem.deleteSubject();
            }
        }
    }

}
