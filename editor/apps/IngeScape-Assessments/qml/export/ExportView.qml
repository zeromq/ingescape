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


I2PopupBase {
    id: rootItem


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
    //signal closeExportView();



    //--------------------------------
    //
    //
    // Slots
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
        color: IngeScapeTheme.veryLightGreyColor
    }

    Button {
        id: btnClose

        anchors {
            top: parent.top
            topMargin: 21
            right: parent.right
            rightMargin: 21
        }

        height: 18
        width: 18

        style: IngeScapeAssessmentsSvgButtonStyle {
            releasedID: "close"
        }

        onClicked: {
            console.log("QML: close Export view");

            // Emit the signal "Close Export View"
            //rootItem.closeExportView();

            close();
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
