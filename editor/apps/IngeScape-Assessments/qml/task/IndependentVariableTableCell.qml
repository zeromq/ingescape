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

Item {
    id: rootItem

    anchors.fill: parent


    //--------------------------------
    //
    //
    // Properties
    //
    //
    //--------------------------------

    // Text of our cell
    property string cellText: ""

    // FLag indicating if our cell is in the first column
    property bool isFirstColumn: (styleData.column === 0)


    //--------------------------------
    //
    //
    // Signals
    //
    //
    //--------------------------------

    // Delete Independent Variable
    signal deleteIndependentVariable();


    //--------------------------------
    //
    //
    // Content
    //
    //
    //--------------------------------

    // FIXME: Loader instead of visible on "rootItem.isFirstColumn" ?
    // --> What is the least expensive between a Loader and a Button ?

    Button {
        id: btnDeleteIndependentVariable

        anchors {
            left: parent.left
            top: parent.top
        }
        width: 50
        height: 30

        text: "DEL"

        //visible: styleData.selected
        visible: styleData.selected && rootItem.isFirstColumn

        onClicked: {
            // Emit the signal "Delete Independent Variable"
            rootItem.deleteIndependentVariable();
        }
    }

    Rectangle {
        id: leftSeparator

        anchors {
            left: parent.left
            leftMargin: 50
            top: parent.top
            bottom: parent.bottom
        }
        width: 1
        color: "silver"

        visible: rootItem.isFirstColumn
    }

    Text {
        anchors {
            fill: parent
            leftMargin: rootItem.isFirstColumn ? 55 : 5
        }

        text: rootItem.cellText

        verticalAlignment: Text.AlignVCenter
        //color: styleData.selected ? IngeScapeTheme.whiteColor : IngeScapeTheme.blackColor
        font {
            family: IngeScapeTheme.textFontFamily
            //weight: Font.Medium
            pixelSize: 12
        }
    }

    Rectangle {
        id: rightSeparator

        anchors {
            right: parent.right
            top: parent.top
            bottom: parent.bottom
        }
        width: 1
        color: "silver"
    }
}


