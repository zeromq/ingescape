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
            bottomMargin: 8
            left : parent.left
        }
        color : MasticTheme.whiteColor
        text : "ACTIONS PANEL"

        font {
            family : MasticTheme.labelFontFamily
            pixelSize: 16
            bold : true
        }
    }


    // Actions list
    Grid {
        id : gridActionsPanel

        anchors {
            bottom: parent.bottom
            bottomMargin: 20
        }

        columns : 3
        layoutDirection: Grid.LeftToRight
        columnSpacing : 4
        rowSpacing : 4

        Repeater {
            model : controller.actionsInPaletteList

            Rectangle {
                id : panelRectangle
                width : (rootItem.width - (gridActionsPanel.columnSpacing* (gridActionsPanel.columns-1)))/ gridActionsPanel.columns
                height : width
                radius : 5

                border {
                    width : 1
                    color : MasticTheme.lightGreyColor
                }

                color : "transparent"

                DropArea {

                    anchors {
                        fill : parent
                    }
                    enabled: model.actionModel === null

                    keys: ["ActionsListItem"]

                    onEntered: {
                        // Restore color of panel Rectangle
                        panelRectangle.color = MasticTheme.greyColor
                    }

                    onExited: {
                        // Restore color of panel Rectangle
                        panelRectangle.color = "transparent"
                    }

                    onDropped: {
                        var dragItem = drag.source;
                        if (typeof dragItem.action !== 'undefined' && controller)
                        {

                            controller.setActionInPalette(index, dragItem.action);

                            // Restore color of panel Rectangle
                            panelRectangle.color = "transparent";
                        }

                    }
                }


                Button {
                    anchors.fill: parent
                    visible : model.actionModel !== null
                    enabled : visible

                    text :  model.actionModel?  model.actionModel.name : "";

                    style : I2ColorButtonStyle {
                        backgroundColorReleased: MasticTheme.darkBlueGreyColor;
                        backgroundColorPressed: MasticTheme.darkBlueGreyColor;
                        labelColorReleased: MasticTheme.whiteColor;
                        labelColorPressed: MasticTheme.lightGreyColor;

                        borderColorPressed: MasticTheme.lightGreyColor;
                        borderColorReleased :MasticTheme.whiteColor;
                        borderWidth: 1;
                        cornerRadius: 5;
                        labelMargin: 0;

                        font {
                            family: MasticTheme.textFontFamily;
                            pixelSize: 16
                        }
                    }

                    onClicked: {

                    }
                }



                Button {
                    id: btnremoveAction

                    anchors {
                        top: parent.top
                        right : parent.right
                        margins : 3
                    }
                    visible : model.actionModel !== null
                    enabled : visible

                    activeFocusOnPress: true
                    style: Theme.LabellessSvgButtonStyle {
                        fileCache: MasticTheme.svgFileMASTIC

                        pressedID: releasedID + "-pressed"
                        releasedID: "closeEditor"
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
