import QtQuick 2.8
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4

import I2Quick 1.0

import INGESCAPE 1.0
import "../theme" as Theme

Item {
    id: rootItem

    //--------------------------------
    //
    // Properties
    //
    //--------------------------------

    // Controller associated to our view
    property var controller: null;

    // my action
    property var action: null;

    // true if action item contains the mouse (rollover)
    property bool actionItemIsHovered: false;

    // Flag indicating if our action is pressed
    property bool actionItemIsPressed: false;

    // Flag indicating if our action is selected
    property bool actionItemIsSelected: rootItem.controller && rootItem.action && (rootItem.controller.selectedAction === rootItem.action);


    width: IngeScapeTheme.leftPanelWidth
    height: 42


    //--------------------------------
    //
    // Signals
    //
    //--------------------------------

    // signal emitted when the delete confirmation popup is needed because the action is already used in the platform
    signal needConfirmationtoDeleteAction(var action);


    //--------------------------------
    //
    // Content
    //
    //--------------------------------

    Rectangle {
        anchors {
            fill: parent
        }

        color: actionItemIsHovered ? IngeScapeTheme.actionsListItemRollOverBackgroundColor : IngeScapeTheme.actionsListItemBackgroundColor

        Rectangle {
            anchors {
                bottom: parent.bottom
                left: parent.left
                right: parent.right
            }

            color: IngeScapeTheme.leftPanelBackgroundColor

            height: 1
        }

        Rectangle {
            id: selectionFeedback

            anchors {
                left: parent.left
                top: parent.top
                bottom: parent.bottom
            }

            width: 6
            color: IngeScapeTheme.selectedAgentColor

            visible: rootItem.actionItemIsSelected
        }

        I2SvgItem {
            id: grip

            anchors {
                left: selectionFeedback.right
                leftMargin: 8
                verticalCenter: parent.verticalCenter
            }

            svgFileCache: IngeScapeTheme.svgFileINGESCAPE
            svgElementId: rootItem.actionItemIsPressed ? "grip-drag-drop-rollover-pressed" : "grip-drag-drop-rollover"

            visible: rootItem.actionItemIsHovered
        }

        // Feedback visible if all conditions are valids
        Rectangle {
            id: feedbackValidConditions

            anchors {
                left: grip.right
                leftMargin: 8
                verticalCenter: parent.verticalCenter
            }
            height: 8
            width: 8

            radius: 4
            color: IngeScapeTheme.selectedAgentColor
            opacity: (rootItem.action && rootItem.action.isValid) ? 1 : 0
        }

        // Action name
        MouseArea {
            id: btnActionName

            anchors {
                left : parent.left
                leftMargin: 44
                verticalCenter: parent.verticalCenter
            }
            height: actionName.height
            width: actionName.width

            hoverEnabled: true

            onClicked: {
                if (controller && rootItem.action) {
                    // Open the editor of our action
                    controller.openActionEditorWithModel(rootItem.action);
                }
            }

            TextMetrics {
                id: textMetrics_ActionName

                elideWidth: rootItem.width - btnActionName.anchors.leftMargin - rightRow.width - rightRow.anchors.rightMargin - 5
                elide: Text.ElideRight

                text: rootItem.action ? rootItem.action.name : ""
                font: IngeScapeTheme.headingFont
            }

            // Name
            Text {
                id: actionName

                anchors {
                    left: parent.left
                }

                color: btnActionName.pressed ? IngeScapeTheme.actionsListPressedLabelColor : IngeScapeTheme.actionsListLabelColor

                text: textMetrics_ActionName.elidedText
                font: IngeScapeTheme.headingFont
            }

            // Underline
            Rectangle {
                anchors {
                    left: actionName.left
                    right: actionName.right
                    bottom: parent.bottom
                }
                height: 2

                visible: btnActionName.containsMouse
                color: actionName.color
            }
        }

        Row {
            id: rightRow

            anchors {
                top: parent.top
                bottom: parent.bottom
                right: parent.right
                rightMargin: 6
            }

            spacing: 12

            Button {
                id: playButton

                anchors.verticalCenter: parent.verticalCenter

                activeFocusOnPress: true

                //visible: rootItem.actionItemIsSelected
                visible: rootItem.actionItemIsHovered

                style: Theme.LabellessSvgButtonStyle {
                    fileCache: IngeScapeTheme.svgFileINGESCAPE

                    pressedID: releasedID + "-pressed"
                    releasedID: "play-action-list"
                    disabledID: releasedID
                }

                onClicked: {
                    console.log("QML: play action '" + rootItem.action.name + "'");
                    if (controller) {
                        controller.executeEffectsOfAction(rootItem.action);
                    }
                }
            }

            Button {
                id: removeButton

                anchors.verticalCenter: parent.verticalCenter

                activeFocusOnPress: true

                //visible: rootItem.actionItemIsSelected
                visible: rootItem.actionItemIsHovered

                style: Theme.LabellessSvgButtonStyle {
                    fileCache: IngeScapeTheme.svgFileINGESCAPE

                    pressedID: releasedID + "-pressed"
                    releasedID: "delete"
                    disabledID: releasedID
                }

                onClicked: {
                    if (controller && rootItem.action)
                    {
                        if (controller.isInsertedInTimeLine(rootItem.action))
                        {
                            rootItem.needConfirmationtoDeleteAction(rootItem.action);
                        }
                        else {
                            // Delete our action
                            controller.deleteAction(rootItem.action);
                        }
                    }
                }
            }
        }
    }
}
