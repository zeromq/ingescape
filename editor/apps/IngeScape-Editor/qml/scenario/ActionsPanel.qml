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
 *      Vincent Peyruqueou <peyruqueou@ingenuity.io>
 *      Alexandre Lemort   <lemort@ingenuity.io>
 *
 */

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
    property var controller : null;



    //--------------------------------
    //
    // Content
    //
    //--------------------------------

    // Title
    Text {
        id : titleActionsPanel

        anchors {
            bottom : gridActionsPanel.top
            bottomMargin: 25
            left : parent.left
            leftMargin: 20
        }
        color : IngeScapeTheme.whiteColor
        text : "ACTIONS PANEL"

        font {
            family : IngeScapeTheme.labelFontFamily
            pixelSize: 18
            weight : Font.ExtraBold
        }
    }


    // "Palette" of actions
    Grid {
        id : gridActionsPanel

        anchors {
            bottom: parent.bottom
            bottomMargin: 18
            left : parent.left
            leftMargin: 20
            rightMargin: 20
            right : parent.right
        }
        height : childrenRect.height
        columns : 3
        layoutDirection: Grid.LeftToRight
        columnSpacing : 6
        rowSpacing : 4

        Repeater {
            model: controller.actionsInPaletteList

            I2Rectangle {
                id: panelRectangle
                width: (gridActionsPanel.width - (gridActionsPanel.columnSpacing * (gridActionsPanel.columns - 1))) / gridActionsPanel.columns
                height: 78
                radiusX: 5
                radiusY: 5

                strokeDashArray: "3, 3"
                stroke : IngeScapeTheme.darkGreyColor
                strokeWidth : 1
                fill : "transparent"

                DropArea {

                    anchors {
                        fill : parent
                    }
                    enabled: model.modelM === null

                    keys: ["ActionsListItem"]

                    onEntered: {
                        // Restore color of panel Rectangle
                        panelRectangle.stroke = IngeScapeTheme.whiteColor
                    }

                    onExited: {
                        // Restore color of panel Rectangle
                        panelRectangle.stroke = IngeScapeTheme.darkGreyColor
                    }

                    onDropped: {
                        var dragItem = drag.source;
                        if ((typeof dragItem.action !== 'undefined') && controller)
                        {
                            controller.setActionInPalette(index, dragItem.action);

                            // Restore color of panel Rectangle
                            panelRectangle.stroke = IngeScapeTheme.darkGreyColor
                        }
                    }
                }


                Button {
                    anchors.fill: parent
                    visible: model.modelM !== null
                    enabled: visible

                    text: model.modelM ? model.modelM.name : ""

                    style: I2ColorButtonStyle {
                        backgroundColorDisabled: IngeScapeTheme.darkBlueGreyColor;
                        backgroundColorReleased: IngeScapeTheme.darkBlueGreyColor;
                        backgroundColorPressed: IngeScapeTheme.darkBlueGreyColor;
                        labelColorDisabled: IngeScapeTheme.lightGreyColor;
                        labelColorReleased: IngeScapeTheme.whiteColor;
                        labelColorPressed: IngeScapeTheme.lightGreyColor;
                        borderWidth: 0;
                        cornerRadius: 5;
                        labelMargin: 0;
                        elideMode:Text.ElideRight;

                        font {
                            family: IngeScapeTheme.labelFontFamily;
                            pixelSize: 17
                            weight : Font.Black
                        }
                    }

                    onClicked: {
                        if (controller && model.modelM)
                        {
                            if (controller.isPlaying) {
                                controller.addActionVMAtCurrentTime(model.modelM);
                            }
                            else {
                                controller.executeEffectsOfAction(model.modelM);
                            }
                        }
                    }
                }



                Button {
                    id: btnremoveAction

                    anchors {
                        top: parent.top
                        right : parent.right
                        margins : -2
                    }
                    visible : model.modelM !== null
                    enabled : visible

                    activeFocusOnPress: true

                    style: LabellessSvgButtonStyle {
                        fileCache: IngeScapeEditorTheme.svgFileIngeScapeEditor

                        pressedID: releasedID + "-pressed"
                        releasedID: "removeActionInPalette"
                        disabledID : releasedID
                    }

                    onClicked: {
                        if (controller) {
                            // remove action in palette
                            controller.setActionInPalette(index, null);
                        }
                    }
                }


            }
        }
    }

}
