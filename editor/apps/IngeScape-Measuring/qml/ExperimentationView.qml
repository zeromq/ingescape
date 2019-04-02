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

import QtQuick 2.8
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4

import I2Quick 1.0

import INGESCAPE 1.0

//import "theme" as Theme
import "popup" as Popup


Item {
    id: rootItem

    //anchors.fill: parent


    //--------------------------------------------------------
    //
    //
    // Properties
    //
    //
    //--------------------------------------------------------

    property ExperimentationController controller: null;

    property IngeScapeModelManager modelManager: null;

    property ExperimentationM experimentation: modelManager ? modelManager.currentExperimentation : null;



    //--------------------------------
    //
    //
    // Signals
    //
    //
    //--------------------------------

    //
    signal goBackToHome();


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

        color: "#AAEEAA"
    }

    Button {
        id: btnHome

        anchors {
            left: parent.left
            top: parent.top
        }

        text: "HOME"

        onClicked: {
            console.log("QML: Go back to home");

            rootItem.goBackToHome();
        }
    }

    Column {
        id: headers

        anchors {
            top: parent.top
            horizontalCenter: parent.horizontalCenter
        }
        spacing: 10

        Text {
            //id: title

            text: rootItem.experimentation ? rootItem.experimentation.name : ""

            anchors {
                horizontalCenter: parent.horizontalCenter
            }
        }

        Text {
            //id: title

            text: (rootItem.modelManager && rootItem.modelManager.currentExperimentationsGroup) ? rootItem.modelManager.currentExperimentationsGroup.name : ""

            anchors {
                horizontalCenter: parent.horizontalCenter
            }
        }

        Text {
            //id: title

            text: rootItem.experimentation ? rootItem.experimentation.creationDate.toLocaleString(Qt.locale(), "dd/MM/yyyy hh:mm:ss") : ""

            anchors {
                horizontalCenter: parent.horizontalCenter
            }
        }
    }


    Rectangle {
        id: configurationPanel

        anchors {
            top: headers.bottom
            topMargin: 50
            left: parent.left
            bottom: parent.bottom
        }

        width: 250

        color: "#33770000"

        Column {


            Button {
                text: "Subjects"

                onClicked: {
                    console.log("QML: Open Subjects");
                }
            }

            Button {
                text: "Tasks"

                onClicked: {
                    console.log("QML: Open Tasks");
                }
            }

            Button {
                text: "Coding"

                enabled: false
            }

            Button {
                text: "Clean"

                enabled: false
            }

            Button {
                text: "Export"

                onClicked: {
                    console.log("QML: Export...");
                }
            }
        }
    }


    Rectangle {
        id: recordsPanel

        anchors {
            top: headers.bottom
            topMargin: 50
            left: configurationPanel.right
            right: parent.right
            bottom: parent.bottom
        }
        color: "#33000077"

        Row {
            id: recordsHeader

            spacing: 20

            Text {
                id: titleRecords

                text: qsTr("Records")
            }

            Button {
                id: btnNewRecord

                text: "New Record"

                onClicked: {
                    createRecordPopup.open();
                }
            }
        }


        Column {

            anchors {
                top: recordsHeader.bottom
                topMargin: 20
                left: parent.left
                right: parent.right
            }

            Repeater {
                model: rootItem.experimentation ? rootItem.experimentation.allRecords : null

                delegate: componentRecord
            }
        }
    }


    //
    // Create Experimentation Popup
    //
    Popup.CreateRecordPopup {
        id: createRecordPopup

        //anchors.centerIn: parent

        controller: rootItem.controller
        experimentation: rootItem.experimentation
    }


    //
    // Component for "Record (Model)"
    //
    Component {
        id: componentRecord

        Rectangle {
            id: rootRecord

            property RecordM record: model.QtObject

            width: parent.width
            height: 30

            color: "#44333333"
            border {
                color: "black"
                width: 1
            }

            Row {
                spacing: 30

                anchors {
                    left: parent.left
                    verticalCenter: parent.verticalCenter
                }

                Text {
                    text: rootRecord.record ? rootRecord.record.name : ""

                    color: IngeScapeTheme.whiteColor
                    font {
                        family: IngeScapeTheme.textFontFamily
                        weight : Font.Medium
                        pixelSize : 12
                    }
                }

                Text {
                    /*text: rootRecord.record ? rootRecord.record.startDateTime.toLocaleString(Qt.locale(), "dd/MM/yyyy hh:mm:ss")
                                            : "../../.... ..:..:.."*/
                    text: "../../.... ..:..:.."

                    color: IngeScapeTheme.whiteColor
                    font {
                        family: IngeScapeTheme.textFontFamily
                        weight : Font.Medium
                        pixelSize : 12
                    }
                }

                Text {
                    /*text: rootRecord.record ? rootRecord.record.duration.toLocaleString(Qt.locale(), "hh:mm:ss.zzz")
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
                spacing: 20

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
                        if (rootRecord.record && rootItem.controller)
                        {
                            console.log("QML: Open " + rootRecord.record.name);

                            // Open the record
                            rootItem.controller.openRecord(rootRecord.record);
                        }
                    }
                }

                Button {
                    id: btnDelete

                    text: "Delete"

                    width: 100
                    height: parent.height

                    onClicked: {
                        if (rootRecord.record && rootItem.controller)
                        {
                            console.log("QML: Delete " + rootRecord.record.name);

                            // Delete the record
                            rootItem.controller.deleteRecord(rootRecord.record);
                        }
                    }
                }
            }
        }
    }

}
