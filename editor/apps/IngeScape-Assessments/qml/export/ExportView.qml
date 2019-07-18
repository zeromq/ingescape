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

    property ExportController controller: null;

    property ExperimentationM experimentation: controller ? controller.currentExperimentation : null;


    //--------------------------------
    //
    //
    // Signals
    //
    //
    //--------------------------------

    // Close Export view
    signal closeExportView();



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

        color: IngeScapeTheme.veryDarkGreyColor
        border {
            color: IngeScapeTheme.darkGreyColor
            width: 1
        }
    }

    Button {
        id: btnClose

        anchors {
            right: parent.right
            top: parent.top
        }
        height: 30

        text: "X"

        onClicked: {
            console.log("QML: close Export view");

            // Emit the signal "Close Export View"
            rootItem.closeExportView();
        }
    }

    Row {
        id: header

        anchors {
            top: parent.top
            topMargin: 10
            horizontalCenter: parent.horizontalCenter
        }
        height: 30

        spacing: 20

        Text {
            id: title

            text: "Export"

            height: parent.height
            verticalAlignment: Text.AlignVCenter

            color: IngeScapeTheme.whiteColor
            font {
                family: IngeScapeTheme.textFontFamily
                weight : Font.Medium
                pixelSize : 20
            }
        }


        Button {
            text: rootItem.experimentation ? rootItem.experimentation.name : "..."

            height: parent.height

            onClicked: {
                console.log("QML: Export experimentation " + rootItem.experimentation.name);

                if (rootItem.controller) {
                    rootItem.controller.exportExperimentation();
                }
            }
        }
    }

}
