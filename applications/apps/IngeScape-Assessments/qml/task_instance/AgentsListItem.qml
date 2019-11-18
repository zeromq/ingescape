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

import I2Quick 1.0

import INGESCAPE 1.0
import "../theme" as Theme
//import "../popup" as Popup


Item {
    id: rootItem

    //--------------------------------
    //
    // Properties
    //
    //--------------------------------

    // Controller associated to our view
    property TaskInstanceController taskInstanceController: null;

    // Agents Grouped by Definition
    //property AgentsGroupedByDefinitionVM agentsGroupedByDefinition: null;
    property var agent: null;

    // true if agent item contains the mouse (rollover)
    property bool agentItemIsHovered : false

    // Licenses controller
    property LicensesController licensesController: null;

    // Flag indicating if the user have a valid license for the editor
    property bool isEditorLicenseValid: true

    width: IngeScapeEditorTheme.leftPanelWidth
    height: 85

    //-----------------------------------------
    //
    // Functions
    //
    //-----------------------------------------


    //--------------------------------
    //
    // Content
    //
    //--------------------------------

    Rectangle {
        anchors {
            fill: parent
        }

        border.width: 0

        color: agentItemIsHovered ? IngeScapeEditorTheme.agentsListItemRollOverBackgroundColor : IngeScapeEditorTheme.agentsListItemBackgroundColor

        // bottom separator
        Rectangle {
            anchors {
                bottom: parent.bottom
                left: parent.left
                right: parent.right
            }

            color: IngeScapeTheme.blackColor

            height: 1
        }


        // Selection feedback
        Rectangle {
            id: selectionFeedback

            anchors {
                left : parent.left
                top : parent.top
                bottom: parent.bottom
            }
            width : 6

            visible: taskInstanceController && rootItem.agent

            color: IngeScapeTheme.selectionColor
        }

        Column {
            id: columnName

            anchors {
                left: parent.left
                leftMargin: 28
                top: parent.top
                topMargin: 12
                right: middleRow.left
                //rightMargin: 5
            }
            height: childrenRect.height

            spacing: 4

            TextMetrics {
                id: textMetricsName

                elideWidth: (columnName.width - textState.width - textState.anchors.leftMargin)
                elide: Text.ElideRight

                text: rootItem.agent ? rootItem.agent.name : ""
                font: IngeScapeTheme.headingFont
            }

            // Name
            Text {
                id: agentName

                anchors {
                    left: parent.left
                    //right: parent.right
                }

                text: textMetricsName.elidedText

                color: (rootItem.agent && rootItem.agent.isON) ? IngeScapeTheme.agentsListLabelColor : IngeScapeTheme.agentOFFLabelColor

                font: IngeScapeTheme.headingFont

                Text {
                    id: textState

                    anchors {
                        left: parent.right
                        leftMargin: 5
                        baseline: parent.baseline
                    }

                    text: (model.state !== "") ? qsTr("(%1)").arg(model.state) : ""

                    color : IngeScapeTheme.whiteColor
                    font {
                        family: IngeScapeTheme.labelFontFamily
                        pixelSize : 12
                        italic: true
                    }
                }
            }
        }
    }

}


