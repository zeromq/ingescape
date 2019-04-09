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
    height: 30


    //--------------------------------------------------------
    //
    //
    // Properties
    //
    //
    //--------------------------------------------------------

    property RecordM modelM: null;



    //--------------------------------
    //
    //
    // Signals
    //
    //
    //--------------------------------

    // Open Record
    signal openRecord();

    // Delete Record
    signal deleteRecord();



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

        Row {
            spacing: 10

            anchors {
                left: parent.left
                verticalCenter: parent.verticalCenter
            }

            Text {
                width: 250

                text: rootItem.modelM ? rootItem.modelM.name : ""

                color: IngeScapeTheme.whiteColor
                font {
                    family: IngeScapeTheme.textFontFamily
                    weight : Font.Medium
                    pixelSize : 12
                }
            }

            Text {
                width: 125

                text: (rootItem.modelM && rootItem.modelM.subject) ? rootItem.modelM.subject.name : ""

                color: IngeScapeTheme.whiteColor
                font {
                    family: IngeScapeTheme.textFontFamily
                    weight : Font.Medium
                    pixelSize : 12
                }
            }

            Text {
                width: 125

                text: (rootItem.modelM && rootItem.modelM.task) ? rootItem.modelM.task.name : ""

                color: IngeScapeTheme.whiteColor
                font {
                    family: IngeScapeTheme.textFontFamily
                    weight : Font.Medium
                    pixelSize : 12
                }
            }

            Text {
                width: 125

                text: rootItem.modelM ? rootItem.modelM.startDateTime.toLocaleString(Qt.locale(), "dd/MM/yyyy hh:mm:ss")
                                      : "../../.... ..:..:.."

                color: IngeScapeTheme.whiteColor
                font {
                    family: IngeScapeTheme.textFontFamily
                    weight : Font.Medium
                    pixelSize : 12
                }
            }

            // FIXME TODO: record.duration
            Text {
                width: 125

                /*text: rootItem.modelM ? rootItem.modelM.duration.toLocaleString(Qt.locale(), "hh:mm:ss.zzz")
                                        : "00:00:00.000"*/
                text: "00:00:00.000"

                color: IngeScapeTheme.whiteColor
                font {
                    family: IngeScapeTheme.textFontFamily
                    weight : Font.Medium
                    pixelSize : 12
                }
            }
        }

        /*MouseArea {
            id: mouseAreaRecord

            anchors.fill: parent

            hoverEnabled: true
        }*/

        Row {
            spacing: 10

            anchors {
                right: parent.right
            }
            height: parent.height

            //visible: mouseAreaRecord.containsMouse

            Button {
                id: btnOpen

                text: "Open"

                width: 100
                height: parent.height

                onClicked: {
                    if (rootItem.modelM)
                    {
                        //console.log("QML: Open the record " + rootItem.modelM.name);

                        // Emit the signal "Open Record"
                        rootItem.openRecord();
                    }
                }
            }

            Button {
                id: btnDelete

                text: "Delete"

                width: 100
                height: parent.height

                onClicked: {
                    if (rootItem.modelM)
                    {
                        //console.log("QML: Delete the record " + rootItem.modelM.name);

                        // Emit the signal "Delete Record"
                        rootItem.deleteRecord();
                    }
                }
            }
        }
    }
}
