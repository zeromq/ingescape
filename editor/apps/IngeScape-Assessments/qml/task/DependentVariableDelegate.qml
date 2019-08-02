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
    id: rootItem


    property var indeVarModel: null


    // Flag indicating if the mouse is hovering the item
    property bool isMouseHovering: itemMouseArea.containsMouse

    property var columnWidths: [ 0, 0, 0 ]


    MouseArea {
        id: itemMouseArea
        anchors.fill: parent
        hoverEnabled: true
    }

    Row {
        id: row
        anchors {
            fill: parent
            leftMargin: 15
        }

        Repeater {
            model: rootItem.indeVarModel ? [ rootItem.indeVarModel.name, rootItem.indeVarModel.description, IndependentVariableValueTypes.enumToString(rootItem.indeVarModel.valueType) ] : ["", "", ""]

            delegate: Text {
                anchors {
                    top: parent.top
                    bottom: parent.bottom
                }

                width: rootItem.columnWidths[index]

                verticalAlignment: Text.AlignVCenter
                text: modelData
                color: IngeScapeAssessmentsTheme.regularDarkBlueHeader
                font {
                    family: IngeScapeTheme.textFontFamily
                    weight: index == 0 ? Font.Bold : Font.Medium
                    pixelSize: 16
                }
            }
        }
    }

    Rectangle {
        id: bottomSeparator
        anchors {
            left: parent.left
            right: parent.right
            bottom: parent.bottom
        }
        height: 2
        color: IngeScapeTheme.veryLightGreyColor
    }
}
