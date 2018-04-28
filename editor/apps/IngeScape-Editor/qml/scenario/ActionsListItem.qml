import QtQuick 2.8
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4

import I2Quick 1.0

import INGESCAPE 1.0
import "../theme" as Theme

Item {
    id : root

    //--------------------------------
    //
    // Properties
    //
    //--------------------------------

    // Controller associated to our view
    property var controller : null;

    // my action
    property var action: null;

    // true if action Item contains the mouse (rollover)
    property bool actionItemIsHovered : false

    width: IngeScapeTheme.leftPanelWidth
    height: 42



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


        color: actionItemIsHovered? IngeScapeTheme.actionsListItemRollOverBackgroundColor : IngeScapeTheme.actionsListItemBackgroundColor

        Rectangle {
            anchors {
                bottom: parent.bottom
                left: parent.left
                right: parent.right
            }

            color: IngeScapeTheme.leftPanelBackgroundColor

            height: 1
        }



        Item {
            id: actionRow

            anchors {
                fill: parent
            }


            // Selected Action
            Item {
                anchors.fill: parent
                visible : controller && root.action && (controller.selectedAction === root.action);

                Rectangle {
                    anchors {
                        left : parent.left
                        top : parent.top
                        bottom: parent.bottom
                    }

                    width : 6
                    color : IngeScapeTheme.selectedAgentColor
                }

                Button {
                    id: removeButton
                    activeFocusOnPress: true

                    anchors {
                        verticalCenter: parent.verticalCenter

                        right : parent.right
                        rightMargin: 12
                    }

                    style: Theme.LabellessSvgButtonStyle {
                        fileCache: IngeScapeTheme.svgFileINGESCAPE

                        pressedID: releasedID + "-pressed"
                        releasedID: "supprimer"
                        disabledID : releasedID
                    }

                    onClicked: {
                        if (controller && root.action)
                        {
                            if(controller.canDeleteActionFromList(root.action))
                            {
                                // Delete our action
                                controller.deleteAction(root.action);
                            } else {
                                root.needConfirmationtoDeleteAction(root.action);
                            }
                        }
                    }
                }
            }


            // display if  conditions are verified
            Rectangle {
                anchors {
                    verticalCenter: actionNameBtn.verticalCenter
                    verticalCenterOffset: 2;
                    right: actionNameBtn.left
                    rightMargin: 5
                }
                height : 2
                width : 2

                color: IngeScapeTheme.whiteColor
                visible : root.action && root.action.isValid
            }


            // Action name
            MouseArea {
                id : actionNameBtn

                anchors {
                    left : parent.left
                    leftMargin: 25
                    verticalCenter: parent.verticalCenter
                }

                height : actionName.height
                width : actionName.width

                hoverEnabled: true
                onClicked: {
                    if (controller && root.action) {
                        // Open the editor of our action
                        controller.openActionEditor(root.action);
                    }
                }


                TextMetrics {
                    id : actName
                    elideWidth: 220
                    elide: Text.ElideRight

                    text: (root.action)? root.action.name : ""
                }

                // Name
                Text {
                    id: actionName

                    anchors {
                        left : parent.left
                    }
                    text : actName.elidedText
                    color: actionNameBtn.pressed ? IngeScapeTheme.actionsListPressedLabelColor : IngeScapeTheme.actionsListLabelColor

                    font: IngeScapeTheme.headingFont
                }

                // underline
                Rectangle {
                    visible: actionNameBtn.containsMouse

                    anchors {
                        left : actionName.left
                        right : actionName.right
                        bottom : parent.bottom
                    }

                    height : 2
                    color : actionName.color
                }
            }

        }
    }
}
