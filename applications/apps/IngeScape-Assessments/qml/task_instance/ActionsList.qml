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
            bottom: actionsList.top
            left: parent.left
            right: parent.right
        }

        color: IngeScapeTheme.blackColor

        height: 1
    }

    ScrollView {
        id: actionsList

        anchors {
            top: parent.top
            topMargin: 10
            left: parent.left
            right: parent.right
            bottom: parent.bottom
        }

        //spacing: 0

        style: IngeScapeScrollViewStyle {
        }

         ListView {

            model: (rootItem.sessionController && rootItem.sessionController.scenarioC) ? rootItem.sessionController.scenarioC.actionsList : null

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
                    controller: rootItem.sessionController

                    actionItemIsHovered: mouseArea.containsMouse
                    actionItemIsPressed: mouseArea.pressed
                }

//                onPressed: {
//                    if (rootItem.sessionController && rootItem.sessionController.scenarioC)
//                    {
//                        Q
//                        if (rootItem.sessionController.scenarioC.selectedAction === model.QtObject) {
//                            rootItem.sessionController.scenarioC.selectedAction = null;
//                        }
//                        else {
//                            rootItem.sessionController.scenarioC.selectedAction = model.QtObject;
//                        }
//                    }
//                }
            }
        }
    }
}
