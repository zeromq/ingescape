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
import QtQuick 2.0

I2PopupBase {
    id: rootPopup

    height: 500
    width: 500

    anchors.centerIn: parent

    isModal: true
    dismissOnOutsideTap: false
    keepRelativePositionToInitialParent: false


    //--------------------------------
    //
    // Signals
    //
    //--------------------------------

    //
    //signal cancelTODO();


    //--------------------------------
    //
    // Slots
    //
    //--------------------------------

    onOpened: {
        // Reset check-boxes
        //checkControl.checked = false;
        //checkObserve.checked = false;
        //checkCancel.checked = false;
    }


    //--------------------------------
    //
    // Content
    //
    //--------------------------------

    Rectangle {

        anchors {
            fill: parent
        }
        radius: 5
        border {
            width: 2
            color: IngeScapeTheme.editorsBackgroundBorderColor
        }
        color: IngeScapeTheme.editorsBackgroundColor


        Text {
            id: title

            anchors {
                left: parent.left
                top: parent.top
            }
            height: 25

            //horizontalAlignment: Text.AlignHCenter

            text: qsTr("New experimentation:")

            color: IngeScapeTheme.whiteColor
            font {
                family: IngeScapeTheme.textFontFamily
                pixelSize: 16
            }
        }
    }

}
