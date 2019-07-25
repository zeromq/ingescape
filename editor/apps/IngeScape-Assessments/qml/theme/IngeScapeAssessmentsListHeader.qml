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
 *      Mathieu Soum <soum@ingenuity.io>
 *
 */

import QtQuick 2.9
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4

import I2Quick 1.0

import INGESCAPE 1.0

Item {
    id: root

    property alias radius: headerTop.radius

    height: 40

    Rectangle {
        id: headerTop
        anchors {
            top: parent.top
            left: parent.left
            right: parent.right
        }

        radius: 5
        height: headerTop.radius * 2
        color: IngeScapeTheme.darkBlueHeader
    }

    Rectangle {
        id: headerBody
        anchors {
            top: headerTop.verticalCenter
            left: parent.left
            right: parent.right
        }

        height: root.height - (headerTop.height / 2)
        color: IngeScapeTheme.darkBlueHeader
    }
}
