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

    width: parent.width
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

        color: agentItemIsHovered ? IngeScapeTheme.greyColor : IngeScapeTheme.veryDarkGreyColor

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
                right: parent.left
                //rightMargin: 5
            }
            height: childrenRect.height

            spacing: 4

            // Name
            Text {
                id: agentName

                anchors {
                    left: parent.left
                    //right: parent.right
                }

                text: rootItem.agent.name

                color: (rootItem.agent && rootItem.agent.isON) ? IngeScapeTheme.whiteColor : IngeScapeTheme.lightGreyColor

                font: IngeScapeTheme.headingFont


            }
        }
    }

}


