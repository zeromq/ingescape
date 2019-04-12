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
//import "../popup" as Popup


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

    //property TasksController controller: null;

    //property IngeScapeModelManager modelManager: null;

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


    Text {
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
    }



}
