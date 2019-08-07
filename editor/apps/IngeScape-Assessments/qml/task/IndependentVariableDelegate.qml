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

Rectangle {
    id: rootItem

    //--------------------------------------------------------
    //
    //
    // Properties
    //
    //
    //--------------------------------------------------------

    // Model to diplay
    property var indeVarModel: null
    // Task controller
    property var taskController: null
    // Flag indicating if the mouse is hovering the item
    property bool isMouseHovering: itemMouseArea.containsMouse || deleteButton.containsMouse
    // Width of the columns (bound by the parent)
    property var columnWidths: [ 0, 0, 0 ]

    color: rootItem.isMouseHovering ? IngeScapeTheme.veryLightGreyColor : IngeScapeTheme.whiteColor


    //--------------------------------------------------------
    //
    //
    // Content
    //
    //
    //--------------------------------------------------------

    MouseArea {
        id: itemMouseArea
        anchors.fill: parent
        hoverEnabled: true
    }

    Row {
        id: row
        anchors {
            fill: parent
        }

        Repeater {
            model: rootItem.indeVarModel ? [ rootItem.indeVarModel.name, rootItem.indeVarModel.description, IndependentVariableValueTypes.enumToString(rootItem.indeVarModel.valueType) ] : ["", "", ""]

            delegate: Item {
                anchors {
                    top: parent.top
                    bottom: parent.bottom
                }

                width: rootItem.columnWidths[index]

                Text {
                    anchors{
                        fill: parent
                        leftMargin: 15
                        rightMargin: 15
                    }

                    verticalAlignment: Text.AlignVCenter
                    elide: Text.ElideRight
                    text: modelData
                    color: IngeScapeAssessmentsTheme.regularDarkBlueHeader
                    font {
                        family: IngeScapeTheme.textFontFamily
                        weight: Font.Medium
                        pixelSize: 16
                        bold: index == 0
                    }
                }
            }
        }
    }

    Button {
        id: deleteButton

        anchors {
            verticalCenter: parent.verticalCenter
            right: parent.right
            rightMargin: 15
        }

        property bool containsMouse: __behavior.containsMouse

        opacity: rootItem.isMouseHovering ? 1 : 0
        enabled: opacity > 0

        width: 40
        height: 30

        style: IngeScapeAssessmentsSvgButtonStyle {
            releasedID: "delete-blue"
            disabledID: releasedID
        }

        onClicked: {
            if (rootItem.taskController && rootItem.indeVarModel) {
                rootItem.taskController.deleteIndependentVariable(rootItem.indeVarModel)
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
