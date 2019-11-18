import QtQuick 2.8
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4

import I2Quick 1.0

import INGESCAPE 1.0

Item {
    id: rootItem

    anchors.fill: parent

    //--------------------------------
    //
    // Properties
    //
    //--------------------------------

    property TaskInstanceController taskInstanceController: null;

    // Licenses controller
    property LicensesController licensesController: null;

    // Flag indicating if the user have a valid license for the editor
    //FIXME: check if license is valid
    property bool isAssessmentsLicenseValid: true


    Rectangle {
        anchors {
            bottom: agentList.top
            left: parent.left
            right: parent.right
        }

        color: IngeScapeTheme.blackColor

        height: 1
    }

    Column {
        id: agentList

        anchors {
            top: parent.top
            topMargin: 10
            left: parent.left
            right: parent.right
            bottom: parent.bottom
        }

        spacing: 0

        Repeater {

            model: rootItem.taskInstanceController ? rootItem.taskInstanceController.allAgentsGroupedByNameInCurrentPlatform : null

            delegate: Rectangle {
                width: 50
                height: 50
                color: "red"
            }

            /*delegate: MouseArea {
                id: mouseArea

                width: parent.width
                height: 42

                hoverEnabled: true

                AgentsListItem {
                    id: agentInList

                    anchors.fill: parent

                    taskInstanceController: taskInstanceController
                    agent: model.QtObject
                }

            }*/
        }
    }
}
