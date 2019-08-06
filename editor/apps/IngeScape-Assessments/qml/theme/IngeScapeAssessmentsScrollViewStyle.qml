/*
 *	IngeScape Editor
 *
 *  Copyright © 2019 Ingenuity i/o. All rights reserved.
 *
 *	See license terms for the rights and conditions
 *	defined by copyright holders.
 *
 *
 *	Contributors:
 *      Mathieu Soum   <soum@ingenuity.io>
 *
 */

import QtQuick 2.5
import QtQuick.Controls 1.2
import QtQuick.Controls.Styles 1.3

import INGESCAPE 1.0


ScrollViewStyle {
    id: ingescapeScrollViewStyle


    //--------------------------------
    //
    // Configure our ScrollViewStyle
    //
    //---------------------------------

    transientScrollBars: false

    // Handles of scrollbars
    handle: Item {
        implicitWidth: 11
        implicitHeight: 26

        Rectangle {
            color: IngeScapeTheme.lightGreyColor

            anchors {
                fill: parent
                topMargin: 1
                leftMargin: 1
                rightMargin: 0
                bottomMargin: 2
            }

            opacity : 0.8
            radius: 10
        }
    }


    // Background of our scrollbar
    scrollBarBackground: Item {
        implicitWidth: 11
        implicitHeight: 26
    }


    // Increment button of our scrollbars
    incrementControl: Item {
        implicitWidth: 1
        implicitHeight: 1
    }


    // Decrement button of our scrollbars
    decrementControl: Item {
        implicitWidth: 1
        implicitHeight: 1
    }
}
