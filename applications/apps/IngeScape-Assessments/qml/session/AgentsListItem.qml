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
    property SessionController sessionController: null;

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

            visible: rootItem.sessionController && rootItem.agent

            color: IngeScapeTheme.selectionColor
        }

        Rectangle{
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

            // Name
            Text {
                id: agentName

                anchors {
                    left: parent.left
                    //right: parent.right
                }

                text: rootItem.agent ? rootItem.agent.name : ""

                color: (rootItem.agent && rootItem.agent.isON) ? IngeScapeTheme.whiteColor : IngeScapeTheme.lightGreyColor

                font: IngeScapeTheme.headingFont


            }
        }

        // Button ON/OFF
        LabellessSvgButton {
            id: btnOnOff

            anchors {
                right: parent.right
                rightMargin:10
                verticalCenter: parent.verticalCenter
            }

            // Agent is "ON" OR Agent can be restarted
            visible: (rootItem.agent && (rootItem.agent.isON || rootItem.agent.canBeRestarted))
            enabled: visible

            pressedID: releasedID + "-pressed"
            releasedID: model.isON ? "on" : "off"
            disabledID : releasedID

            onClicked: {
                // ON
                if (rootItem.agent && rootItem.agent.isON)
                {
                    // Ask to stop our agent
                    rootItem.agent.askStopAgent();
                }
                // OFF
                else
                {
                    // Ask to start our agent
                    rootItem.agent.askStartAgent();
                }
            }
        }
    }
}


