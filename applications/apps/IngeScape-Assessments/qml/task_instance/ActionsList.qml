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
            bottom: actionsList.top
            left: parent.left
            right: parent.right
        }

        color: IngeScapeTheme.blackColor

        height: 1
    }

    Column {
        id: actionsList

        anchors {
            top: parent.top
            topMargin: 10
            left: parent.left
            right: parent.right
            bottom: parent.bottom
        }

        spacing: 0

        Repeater {

            model: (rootItem.taskInstanceController && rootItem.taskInstanceController.scenarioC) ? rootItem.taskInstanceController.scenarioC.actionsList : null

            delegate: MouseArea {
                id: mouseArea

                width: parent.width
                height: 42

                hoverEnabled: true

                ActionsListItem {
                    id : actionInList

                    anchors.fill: parent

                    areActionsButtonsActivated: false

                    action: model.QtObject
                    controller: rootItem.taskInstanceController

                    actionItemIsHovered: mouseArea.containsMouse
                    actionItemIsPressed: mouseArea.pressed
                }

//                onPressed: {
//                    if (rootItem.taskInstanceController && rootItem.taskInstanceController.scenarioC)
//                    {
//                        Q
//                        if (rootItem.taskInstanceController.scenarioC.selectedAction === model.QtObject) {
//                            rootItem.taskInstanceController.scenarioC.selectedAction = null;
//                        }
//                        else {
//                            rootItem.taskInstanceController.scenarioC.selectedAction = model.QtObject;
//                        }
//                    }
//                }
            }
        }
    }
}
