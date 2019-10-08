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

    // Task controller
    property var taskController: null

    // Model of protocol that contains our model of independent variable
    property TaskM protocol: null

    // Current model of independent variable
    property IndependentVariableM independentVarModel: null

    // Flag indicating if the mouse is hovering the item
    property bool isMouseHovering: itemMouseArea.containsMouse || editIndepVarButton.containsMouse || deleteIndepVarButton.containsMouse

    // Flag indicating if the current independent variable is being edited
    property bool isCurrentlyEditing: false

    // Width of the columns (bound by the parent)
    property var columnWidths: [ 0, 0, 0 ]

    // Flag indicating if an independent variable, among all independent variables, is being edited
    // Bound by the parent
    property bool indepVarEditionInProgress: false

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
        anchors.fill: parent

        Repeater {
            model: rootItem.independentVarModel ? [ rootItem.independentVarModel.name, rootItem.independentVarModel.description, IndependentVariableValueTypes.enumToString(rootItem.independentVarModel.valueType) ] : ["", "", ""]

            delegate: Item {

                anchors.verticalCenter: parent.verticalCenter

                width: rootItem.columnWidths[index]
                height: 30

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


    //
    // Buttons Edit / Delete
    //
    Row {
        spacing: 12

        anchors {
            right: parent.right
            rightMargin: 15
            verticalCenter: parent.verticalCenter
        }

        Button {
            id: editIndepVarButton
            height: 30
            width: 40

            property bool containsMouse: __behavior.containsMouse

            opacity: (rootItem.isMouseHovering && !rootItem.indepVarEditionInProgress) ? 1 : 0
            enabled: opacity > 0

            style: IngeScapeAssessmentsSvgButtonStyle {
                releasedID: "edit"
                disabledID: releasedID
            }

            onClicked: {
                if (rootItem.protocol && rootItem.independentVarModel)
                {
                    rootItem.protocol.initTemporaryIndependentVariable(rootItem.independentVarModel)
                }

                rootItem.isCurrentlyEditing = true;
            }
        }

        Button {
            id: deleteIndepVarButton

            property bool containsMouse: __behavior.containsMouse

            height: 30
            width: 40

            opacity: rootItem.isMouseHovering && !rootItem.indepVarEditionInProgress ? 1 : 0
            enabled: opacity > 0

            style: IngeScapeAssessmentsSvgButtonStyle {
                releasedID: "delete-blue"
                disabledID: releasedID
            }

            onClicked: {
                if (rootItem.taskController && rootItem.independentVarModel)
                {
                    rootItem.taskController.deleteIndependentVariable(rootItem.independentVarModel)
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
        color: rootItem.isCurrentlyEditing ? IngeScapeTheme.lightGreyColor : IngeScapeTheme.veryLightGreyColor
    }
}
