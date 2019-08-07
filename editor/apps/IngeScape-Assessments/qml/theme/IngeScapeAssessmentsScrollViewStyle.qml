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
    id: rootStyle


    //--------------------------------
    //
    // Configure our ScrollViewStyle
    //
    //---------------------------------

    property int scrollBarSize: 11
    property int verticalScrollbarMargin: 3
    property int horizontalScrollbarMargin: 3

    transientScrollBars: false

    // Handles of scrollbars
    handle: Item {
        implicitWidth: rootStyle.scrollBarSize + rootStyle.verticalScrollbarMargin
        implicitHeight: rootStyle.scrollBarSize + rootStyle.horizontalScrollbarMargin

        Rectangle {
            color: IngeScapeTheme.lightGreyColor

            anchors {
                fill: parent
                leftMargin: verticalScrollbarMargin
                topMargin: horizontalScrollbarMargin
            }

            opacity : 0.8
            radius: 10
        }
    }

    // Area between scrollbars
    corner: Rectangle {
        color: "transparent"
    }


    // Background of our scrollbar
    scrollBarBackground: Item {
        implicitWidth: rootStyle.scrollBarSize + rootStyle.verticalScrollbarMargin
        implicitHeight: rootStyle.scrollBarSize + rootStyle.horizontalScrollbarMargin
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
