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
 *      Justine Limoges <limoges@ingenuity.io>
 */

import QtQuick 2.8
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4

import I2Quick 1.0

import INGESCAPE 1.0

import "../theme" as Theme;


Item {
    id : combobox

    height : 25
    width : 148

    onVisibleChanged: {
        if (!visible) {
            close();
        }
    }

    onSelectedItemChanged: {
        updateCurrentSelection();
    }

    onModelChanged: {
        updateCurrentSelection();
    }

    //selected Item
    property var selectedItem;

    //Model
    property alias model: combolist.model

    // number of inputs and outputs in IOP list (in order to place the separators)
    property int inputsNumber: 0;
    property int outputsNumber: 0;
    property int parametersNumber: 0;


    /////////////////////////////////////////////////////
    function modelToString(model) {
        return model.name;
    }

    /**
         * Function called when we need to update our current selection
         */
    function updateCurrentSelection() {
        if (selectedItem)
        {
            comboText.text = modelToString(combobox.selectedItem);
        }  else {
            comboText.text = "";
        }
    }

    /***
    * open function : open the combobox
    ***/
    function open() {
        comboButton.checked = true;
        popup.open();
    }

    /***
    * close function : close the combobox
    ***/
    function close() {
        comboButton.checked = false;
        combobox.forceActiveFocus();
        popup.close();
    }

    Rectangle {
        id: comboButton

        property bool checked: false

        width: parent.width
        height: parent.height
        radius: 1

        border.width: _mouseAreaCombo.containsPress ? 1 : 0;
        border.color: IngeScapeTheme.darkBlueGreyColor
        color : _mouseAreaCombo.containsPress? IngeScapeTheme.darkGreyColor2 : IngeScapeTheme.darkBlueGreyColor

        Text {
            id:comboPlaceholder

            visible: (combobox.selectedItem === null);
            text : (combolist.count === 0 ? "- No Item -" : "- Select an item -")
            anchors {
                verticalCenter: parent.verticalCenter;
                left: parent.left;
                leftMargin: 10
                right: _imageCombo.left;
                rightMargin: 10
            }


            font {
                pixelSize: 15
                family: IngeScapeTheme.textFontFamily;
                italic : true;
            }

            color : IngeScapeTheme.greyColor
            verticalAlignment: Text.AlignVCenter;
            elide : Text.ElideRight;
        }


        Rectangle {
            id : circleSelected
            anchors {
                left : parent.left
                leftMargin: 5
                verticalCenter: parent.verticalCenter
            }

            visible: (combobox.selectedItem !== null);
            width : 11
            height : width
            radius : width/2

            color : if (combobox.selectedItem) {
                        switch (combobox.selectedItem.agentIOPValueType)
                        {
                        case AgentIOPValueTypes.INTEGER:
                            IngeScapeTheme.yellowColor
                            break;
                        case AgentIOPValueTypes.BOOL:
                            IngeScapeTheme.yellowColor
                            break;
                        case AgentIOPValueTypes.DOUBLE:
                            IngeScapeTheme.yellowColor
                            break;
                        case AgentIOPValueTypes.STRING:
                            IngeScapeTheme.greenColor
                            break;
                        case AgentIOPValueTypes.IMPULSION:
                            IngeScapeTheme.purpleColor
                            break;
                        case AgentIOPValueTypes.DATA:
                            IngeScapeTheme.redColor2
                            break;
                        case AgentIOPValueTypes.MIXED:
                            IngeScapeTheme.whiteColor
                            break;
                        case AgentIOPValueTypes.UNKNOWN:
                            "#000000"
                            break;
                        default:
                            IngeScapeTheme.whiteColor;
                            break;
                        }
                    }
                    else {
                        IngeScapeTheme.whiteColor;
                    }
        }


        Text {
            id:comboText

            anchors {
                verticalCenter: parent.verticalCenter;
                left: circleSelected.right;
                leftMargin: 5
                right: _imageCombo.left;
                rightMargin: 10
            }

            font {
                pixelSize: 15
                family: IngeScapeTheme.textFontFamily;
            }

            color : IngeScapeTheme.whiteColor
            verticalAlignment: Text.AlignVCenter;
            elide : Text.ElideRight;
        }


        Image {
            id:_imageCombo;
            anchors.verticalCenter: parent.verticalCenter;
            anchors.right: parent.right;
            anchors.rightMargin: 5
            rotation : (comboButton.checked ? 180 : 0);
            source : "image://I2svg/resources/SVG/ingescape-pictos.svg#iconCombo";

            Behavior on rotation {
                NumberAnimation {}
            }
        }


        MouseArea {
            id:_mouseAreaCombo;
            anchors.fill: parent;
            activeFocusOnTab: true;
            enabled : (combolist.count !== 0)

            onClicked: {
                _mouseAreaCombo.forceActiveFocus();
                (comboButton.checked) ? combobox.close() : combobox.open();
            }
        }

        onVisibleChanged: {
            if(!visible)
                combobox.close();
        }
    }

    I2PopupBase {
        id : popup
        anchors.top:comboButton.bottom;

        width: comboButton.width;
        height: ((combolist.count < 8) ? (combolist.count)*(comboButton.height+1) : 8*(comboButton.height+1) );


        isModal: true;
        layerColor: "transparent"
        layerObjectName: "overlayLayerComboBox";
        dismissOnOutsideTap : true;

        keepRelativePositionToInitialParent : true;

        onClosed: {
            combobox.close();
        }

        onOpened: {

        }

        Rectangle {
            id : popUpBackground
            anchors.fill : parent
            color: "#2C333E"
        }

        ScrollView {
            id : _scrollView

            visible: comboButton.checked;

            anchors {
                top: parent.top
                bottom: parent.bottom
            }

            width: comboButton.width
            height: ((combolist.count < 5) ? (combolist.count) * (comboButton.height + 1)
                                           : 5 * (comboButton.height + 1) )

            style: IngeScapeScrollViewStyle {
            }

            // Prevent drag overshoot on Windows
            flickableItem.boundsBehavior: Flickable.OvershootBounds

            // Content of our scrollview
            ListView {
                id:combolist

                boundsBehavior: Flickable.StopAtBounds

                anchors {
                   top : parent.top
                   topMargin: 2
                }
                width: parent.width;
                height: ( (combolist.count<5) ? combolist.count*(comboButton.height+1) : 5*(comboButton.height+1) );

                visible: parent.visible;

                delegate: Rectangle {
                    anchors {
                        left: parent.left
                        right: parent.right
                    }

                    width: comboButton.width
                    height: comboButton.height

                    color: _mouseAreaItem.containsPress ? IngeScapeTheme.darkBlueGreyColor : "transparent"

                    // Inputs / Outputs separator
                    Rectangle {
                        anchors {
                            left: parent.left
                            leftMargin: 5
                            right: parent.right
                            rightMargin: 5
                            top : parent.top
                        }
                        height : 1
                        color: IngeScapeTheme.lightGreyColor
                        visible: ((combobox.inputsNumber > 0) && (index === combobox.inputsNumber))
                    }

                    // Outputs / Parameters separator
                    Rectangle {
                        anchors {
                            left: parent.left
                            leftMargin: 5
                            right: parent.right
                            rightMargin: 5
                            top : parent.top
                        }
                        height : 1
                        color: IngeScapeTheme.lightGreyColor
                        visible: ((combobox.parametersNumber > 0) && (index === (combobox.inputsNumber + combobox.outputsNumber)))
                    }

                    Rectangle {
                        id : circle
                        anchors {
                            left : parent.left
                            leftMargin: 5
                            verticalCenter: parent.verticalCenter
                        }

                        width : 11
                        height : width
                        radius : width/2

                        color : switch (model.agentIOPValueType)
                                {
                                case AgentIOPValueTypes.INTEGER:
                                    IngeScapeTheme.yellowColor
                                    break;
                                case AgentIOPValueTypes.BOOL:
                                    IngeScapeTheme.yellowColor
                                    break;
                                case AgentIOPValueTypes.DOUBLE:
                                    IngeScapeTheme.yellowColor
                                    break;
                                case AgentIOPValueTypes.STRING:
                                    IngeScapeTheme.greenColor
                                    break;
                                case AgentIOPValueTypes.IMPULSION:
                                    IngeScapeTheme.purpleColor
                                    break;
                                case AgentIOPValueTypes.DATA:
                                    IngeScapeTheme.redColor2
                                    break;
                                case AgentIOPValueTypes.MIXED:
                                    IngeScapeTheme.whiteColor
                                    break;
                                case AgentIOPValueTypes.UNKNOWN:
                                    "#000000"
                                    break;
                                default:
                                    IngeScapeTheme.whiteColor;
                                    break;
                                }
                    }

                    Text {
                        anchors {
                            verticalCenter: parent.verticalCenter
                            verticalCenterOffset: 1
                            left : circle.right
                            leftMargin: 5
                            right: parent.right
                        }

                        color: (combobox.selectedItem === model.QtObject)?  IngeScapeTheme.orangeColor : IngeScapeTheme.whiteColor

                        text: model.name
                        elide: Text.ElideRight

                        font {
                            family: IngeScapeTheme.textFontFamily
                            pixelSize: 16
                        }

                    }

                    MouseArea {
                        id:_mouseAreaItem
                        anchors.fill : parent
                        onClicked: {
                            close();
                            combobox.selectedItem = model.QtObject;

                        }
                    }
                }

            }
        }

    }
}

