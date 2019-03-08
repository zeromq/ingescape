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

    //property type name: value



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

        color: "#EEAAAA"
    }

    Text {
        id: title

        anchors {
            top: parent.top
            horizontalCenter: parent.horizontalCenter
        }

        text: "Experimentations"
    }

    Button {
        id: btnNewExpe

        anchors {
            top: title.bottom
            topMargin: 20
            horizontalCenter: parent.horizontalCenter
        }

        text: "Nouvelle Expé"
    }

    ListView {


        anchors {
            top: btnNewExpe.bottom
            topMargin: 20
            left: parent.left
            right: parent.right
        }

        //model:
    }
}
