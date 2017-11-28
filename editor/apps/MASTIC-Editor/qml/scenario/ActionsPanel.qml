/*
 *	MASTIC Editor
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

import MASTIC 1.0
import "../theme" as Theme

Item {
    id: rootItem

    height : 300

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

    //Title
    Text {
        id : titleActionsPanel

        anchors {
            bottom : gridActionsPanel.top
            bottomMargin: 25
            left : parent.left
        }
        color : MasticTheme.whiteColor
        text : "ACTIONS PANEL"

        font {
            family : MasticTheme.labelFontFamily
            pixelSize: 18
            weight : Font.ExtraBold
        }
    }


    // Actions list
    Grid {
        id : gridActionsPanel

        anchors {
            bottom: parent.bottom
            bottomMargin: 18
        }
        height : childrenRect.height
        columns : 3
        layoutDirection: Grid.LeftToRight
        columnSpacing : 6
        rowSpacing : 4

        Repeater {
            model : controller.actionsInPaletteList

            I2Rectangle {
                id : panelRectangle
                width : (rootItem.width - (gridActionsPanel.columnSpacing* (gridActionsPanel.columns-1)))/ gridActionsPanel.columns
                height : 78
                radiusX : 5
                radiusY : 5

                strokeDashArray: "3, 3"
                stroke : MasticTheme.darkGreyColor
                strokeWidth : 1
                fill : "transparent"

                DropArea {

                    anchors {
                        fill : parent
                    }
                    enabled: model.actionModel === null

                    keys: ["ActionsListItem"]

                    onEntered: {
                        // Restore color of panel Rectangle
                        panelRectangle.stroke = MasticTheme.whiteColor
                    }

                    onExited: {
                        // Restore color of panel Rectangle
                        panelRectangle.stroke = MasticTheme.darkGreyColor
                    }

                    onDropped: {
                        var dragItem = drag.source;
                        if (typeof dragItem.action !== 'undefined' && controller)
                        {

                            controller.setActionInPalette(index, dragItem.action);

                            // Restore color of panel Rectangle
                            panelRectangle.stroke = MasticTheme.darkGreyColor
                        }

                    }
                }


                Button {
                    anchors.fill: parent
                    visible : model.actionModel !== null
                    enabled : visible

                    text :  model.actionModel?  model.actionModel.name : "";

                    style : I2ColorButtonStyle {
                        backgroundColorDisabled: MasticTheme.darkBlueGreyColor;
                        backgroundColorReleased: MasticTheme.darkBlueGreyColor;
                        backgroundColorPressed: MasticTheme.darkBlueGreyColor;
                        labelColorDisabled: MasticTheme.lightGreyColor;
                        labelColorReleased: MasticTheme.whiteColor;
                        labelColorPressed: MasticTheme.lightGreyColor;
                        borderWidth: 0;
                        cornerRadius: 5;
                        labelMargin: 0;

                        font {
                            family: MasticTheme.labelFontFamily;
                            pixelSize: 17
                            weight : Font.Black
                        }
                    }

                    onClicked: {
                        if (controller && model.actionModel) {
                            if (controller.isPlaying) {
                                controller.addActionVMAtCurrentTime(model.actionModel);
                            } else {
                                controller.executeEffectsOfAction(model.actionModel);
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
                    visible : model.actionModel !== null
                    enabled : visible

                    activeFocusOnPress: true
                    style: Theme.LabellessSvgButtonStyle {
                        fileCache: MasticTheme.svgFileMASTIC

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
