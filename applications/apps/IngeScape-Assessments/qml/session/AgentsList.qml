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

    property SessionController sessionController: null;

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

    ScrollView {
        id: agentList

        anchors {
            top: parent.top
            topMargin: 10
            left: parent.left
            right: parent.right
            bottom: parent.bottom
        }

        style: IngeScapeScrollViewStyle {
        }

        // Prevent drag overshoot on Windows
        flickableItem.boundsBehavior: Flickable.OvershootBounds

        ListView {
            model: rootItem.sessionController ? rootItem.sessionController.agentsGroupedByNameInCurrentPlatform : null

            delegate: MouseArea {
                id: mouseArea

                width: parent ? parent.width : 1
                height: 42

                hoverEnabled: true

                AgentsListItem {
                    id: agentInList

                    anchors.fill: parent

                    sessionController: rootItem.sessionController
                    agent: model.QtObject
                }

            }
        }
    }
}
