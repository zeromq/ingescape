/*
 *	IngeScape Editor
 *
 *  Copyright © 2017 Ingenuity i/o. All rights reserved.
 *
 *	See license terms for the rights and conditions
 *	defined by copyright holders.
 *
 *
 *	Contributors:
 *      Alexandre Lemort   <lemort@ingenuity.io>
 *      Justine Limoges    <limoges@ingenuity.io>
 *      Vincent Peyruqueou <peyruqueou@ingenuity.io>
 *
 */

import QtQuick 2.8
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4

import INGESCAPE 1.0

ButtonStyle {

    property bool isVisibleSeparation: true

    background: Rectangle {
        color: "transparent"

        Rectangle {
            anchors {
                fill: parent
            }

            color: control.hovered ? IngeScapeTheme.lightBlueGreyColor : "transparent"
            opacity: 0.1
        }

        Rectangle {
            color: IngeScapeTheme.greyColor
            height: 1
            visible: isVisibleSeparation

            anchors {
                left: parent.left
                leftMargin: 12
                right: parent.right
                rightMargin: 12
                bottom: parent.bottom
            }
        }
    }

    label: Text {
        text: control.text

        color: control.enabled ? (control.pressed ? IngeScapeTheme.orangeColor : "white")
                               : IngeScapeTheme.darkGreyColor

        verticalAlignment: Text.AlignVCenter

        anchors {
            left: parent.left
            leftMargin: 18
            right: parent.right
            rightMargin: 18
        }
    }
}
