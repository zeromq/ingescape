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

    //property IngeScapeModelManager modelManager: null;



    //--------------------------------
    //
    //
    // Signals
    //
    //
    //--------------------------------

    // Close Tasks view
    signal closeTasksView();



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

        color: "#FF663366"
    }

    Button {
        id: btnClose

        anchors {
            right: parent.right
            top: parent.top
        }

        text: "X"

        onClicked: {
            console.log("QML: close Tasks view");

            // Emit the signal "closeTasksView"
            rootItem.closeTasksView();
        }
    }

    Text {
        id: title

        anchors {
            top: parent.top
            topMargin: 10
            horizontalCenter: parent.horizontalCenter
        }

        text: "Tasks"

        color: IngeScapeTheme.whiteColor
        font {
            family: IngeScapeTheme.textFontFamily
            weight : Font.Medium
            pixelSize : 20
        }
    }
}
