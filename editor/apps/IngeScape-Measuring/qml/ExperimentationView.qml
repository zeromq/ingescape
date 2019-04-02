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

    // Go back to "Parent" view
    signal goBackToParentView();

    // Go to "Subjects" view
    signal goToSubjectsView();

    // Go to "Tasks" view
    signal goToTasksView();



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

        color: "#FF336633"
    }

    Button {
        id: btnGoBack

        anchors {
            left: parent.left
            top: parent.top
        }

        text: "BACK"

        onClicked: {
            console.log("QML: Go back to 'Parent' view");

            // Emit the signal the "goBackToParentView"
            rootItem.goBackToParentView();
        }
    }

    Column {
        id: headers

        anchors {
            top: parent.top
            topMargin: 10
            horizontalCenter: parent.horizontalCenter
        }
        spacing: 10

        Text {
            //id: title

            anchors {
                horizontalCenter: parent.horizontalCenter
            }

            text: rootItem.experimentation ? rootItem.experimentation.name : ""

            color: IngeScapeTheme.whiteColor
            font {
                family: IngeScapeTheme.textFontFamily
                weight : Font.Medium
                pixelSize : 18
            }
        }

        Text {
            //id: title

            anchors {
                horizontalCenter: parent.horizontalCenter
            }

            text: (rootItem.modelManager && rootItem.modelManager.currentExperimentationsGroup) ? rootItem.modelManager.currentExperimentationsGroup.name : ""

            color: IngeScapeTheme.whiteColor
            font {
                family: IngeScapeTheme.textFontFamily
                weight : Font.Medium
                pixelSize : 16
            }
        }

        Text {
            //id: title

            anchors {
                horizontalCenter: parent.horizontalCenter
            }

            text: rootItem.experimentation ? rootItem.experimentation.creationDate.toLocaleString(Qt.locale(), "dd/MM/yyyy hh:mm:ss") : ""

            color: IngeScapeTheme.whiteColor
            font {
                family: IngeScapeTheme.textFontFamily
                weight : Font.Medium
                pixelSize : 16
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

        color: "#44AAAAAA"

        Column {


            Button {
                text: "Subjects"

                onClicked: {
                    console.log("QML: Go to Subjects view");

                    // Emit the signal "goToSubjectsView"
                    rootItem.goToSubjectsView();
                }
            }

            Button {
                text: "Tasks"

                onClicked: {
                    console.log("QML: Go to Tasks view");

                    // Emit the signal "goToTasksView"
                    rootItem.goToTasksView();
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
        color: "#44222222"

        Row {
            id: recordsHeader

            spacing: 20

            Text {
                id: titleRecords

                text: qsTr("Records")

                color: IngeScapeTheme.whiteColor
                font {
                    family: IngeScapeTheme.textFontFamily
                    weight : Font.Medium
                    pixelSize : 14
                }
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

            color: "#44222222"
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
