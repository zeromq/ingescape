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
import QtQml 2.12

import I2Quick 1.0

import INGESCAPE 1.0

//import "theme" as Theme
import "../popup" as Popup


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

    property TasksController controller: null;

    property TaskM modelM: null;


    //--------------------------------
    //
    //
    // Signals
    //
    //
    //--------------------------------

    //
    //signal todo();



    //--------------------------------
    //
    //
    // Functions
    //
    //
    //--------------------------------




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

        color: "lightsteelblue"
        border {
            color: "black"
            width: 1
        }
    }


    /*Text {
        id: txtName

        anchors {
            top: parent.top
            topMargin: 10
            horizontalCenter: parent.horizontalCenter
        }

        text: rootItem.modelM ? rootItem.modelM.name : "..."

        color: IngeScapeTheme.whiteColor
        font {
            family: IngeScapeTheme.textFontFamily
            weight: Font.Medium
            pixelSize: 18
        }
    }*/


    Rectangle {
        id: panelIndepVar

        anchors {
            left: parent.left
            right: parent.right
            top: parent.top
        }
        height: parent.height / 2

        color: "transparent"
        border {
            width: 1
            color: "white"
        }

        Row {
            id: headerIndepVar

            anchors {
                left: parent.left
                leftMargin: 10
                top: parent.top
                topMargin: 10
            }

            spacing: 20

            Text {
                text: "Independent Variables"

                color: IngeScapeTheme.whiteColor
                font {
                    family: IngeScapeTheme.textFontFamily
                    weight: Font.Medium
                    pixelSize: 18
                }
            }

            Button {
                text: "New Independent Variable"

                height: 30

                onClicked: {
                    console.log("QML: New Independent Variable");

                    // Open the popup
                    createIndependentVariablePopup.open();
                }
            }
        }


        TableView {
            id: tableIndepVar

            anchors {
                left: parent.left
                leftMargin: 10
                right: parent.right
                rightMargin: 10
                top: headerIndepVar.bottom
                topMargin: 10
                bottom: parent.bottom
                bottomMargin: 10
            }

            model: rootItem.modelM ? rootItem.modelM.independentVariables : null

            TableViewColumn {
                role: "name"
                title: qsTr("Nom")
            }

            TableViewColumn {
                role: "description"
                title: qsTr("Description")
            }

            TableViewColumn {
                role: "valueType"
                title: qsTr("Type")
            }
        }
    }

    Rectangle {
        id: panelDepVar

        anchors {
            left: parent.left
            right: parent.right
            bottom: parent.bottom
        }
        height: parent.height / 2

        color: "transparent"
        border {
            width: 1
            color: "white"
        }

        Row {
            id: headerDepVar

            anchors {
                left: parent.left
                leftMargin: 10
                top: parent.top
                topMargin: 10
            }

            spacing: 20

            Text {
                text: "Dependent Variables"

                color: IngeScapeTheme.whiteColor
                font {
                    family: IngeScapeTheme.textFontFamily
                    weight: Font.Medium
                    pixelSize: 18
                }
            }

            Button {
                text: "New Dependent Variable"

                height: 30

                onClicked: {
                    console.log("QML: New Dependent Variable");

                    // Open the popup
                    //createTaskPopup.open();
                }
            }
        }

        TableView {
            id: tableDepVar

            anchors {
                left: parent.left
                leftMargin: 10
                right: parent.right
                rightMargin: 10
                top: headerDepVar.bottom
                topMargin: 10
                bottom: parent.bottom
                bottomMargin: 10
            }

            model: rootItem.modelM ? rootItem.modelM.dependentVariables : null

            TableViewColumn {
                role: "name"
                title: qsTr("Nom")
            }

            TableViewColumn {
                role: "description"
                title: qsTr("Description")
            }

            TableViewColumn {
                role: "agentName"
                title: qsTr("Agent")
            }

            TableViewColumn {
                role: "outputName"
                title: qsTr("Sortie")
            }
        }
    }


    //
    // Create Characteristic Popup
    //
    Popup.CreateIndependentVariablePopup {
        id: createIndependentVariablePopup

        //anchors.centerIn: parent

        controller: rootItem.controller

        task: rootItem.modelM
    }

}
