import QtQuick 2.8
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4

import I2Quick 1.0

import MASTIC 1.0
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

    width: MasticTheme.leftPanelWidth
    height: 42


    //--------------------------------
    //
    // Content
    //
    //--------------------------------

    Rectangle {
        anchors {
            fill: parent
        }


        color: actionItemIsHovered? MasticTheme.actionsListItemRollOverBackgroundColor : MasticTheme.actionsListItemBackgroundColor

        Rectangle {
            anchors {
                bottom: parent.bottom
                left: parent.left
                right: parent.right
            }

            color: MasticTheme.leftPanelBackgroundColor

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
                    color : MasticTheme.selectedAgentColor
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
                        fileCache: MasticTheme.svgFileMASTIC

                        pressedID: releasedID + "-pressed"
                        releasedID: "supprimer"
                        disabledID : releasedID
                    }

                    onClicked: {
                        if (controller)
                        {
                            // Delete our action
                            controller.deleteAction(model.QtObject);
                        }
                    }
                }
            }


            // Action name
            MouseArea {
                id : actionNameBtn

                anchors {
                    left : parent.left
                    leftMargin: 45
                    verticalCenter: parent.verticalCenter
                }

                height : actionName.height
                width : childrenRect.width - (removeButton.width + 12)

                hoverEnabled: true
                onClicked: {
                    if (controller && root.action) {
                        // Open the editor of our action
                        controller.openActionEditor(root.action);
                    }
                }


                TextMetrics {
                    id : actName

                    elideWidth: actionRow.width
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
                    color: actionNameBtn.pressed ? MasticTheme.actionsListPressedLabelColor : MasticTheme.actionsListLabelColor

                    font: MasticTheme.headingFont
                }

                // underline
                Rectangle {
                    visible: actionNameBtn.containsMouse

                    anchors {
                        left : actionNameBtn.left
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
