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
 *      Justine Limoges <limoges@ingenuity.io>
 */

import QtQuick 2.8
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4

import I2Quick 1.0

import MASTIC 1.0

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


    //selected Item & Index
    property var selectedItem;
    property alias selectedIndex: combolist.currentIndex;

    //Model
    property alias model: combolist.model

    // nuber of inputs in IOP list (in order to place the separator)
    property int inputsNumber: 0;


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
            var index = 0;
            var found = false;

            while (!found && (index < combolist.count))
            {
                var item = combolist.model.get(index);
                var textItem = modelToString(item);
                if (modelToString(selectedItem) === textItem)
                {
                    combolist.currentIndex = index;
                    comboText.text = textItem;
                    found = true;
                }

                index++;
            }
        }  else {
            combolist.currentIndex = -1;
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
        property bool checked : false;
        width:  parent.width;
        height: parent.height;
        radius: 1;

        border.width: _mouseAreaCombo.containsPress ? 1 : 0;
        border.color: MasticTheme.darkBlueGreyColor
        color : _mouseAreaCombo.containsPress? MasticTheme.darkGreyColor2 : MasticTheme.darkBlueGreyColor

        Text {
            id:comboPlaceholder

            visible: (comboText.text === "");
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
                family: MasticTheme.textFontFamily;
                italic : true;
            }

            color : MasticTheme.greyColor
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
                            MasticTheme.orangeColor2
                            break;
                        case AgentIOPValueTypes.BOOL:
                            MasticTheme.orangeColor2
                            break;
                        case AgentIOPValueTypes.DOUBLE:
                            MasticTheme.orangeColor2
                            break;
                        case AgentIOPValueTypes.STRING:
                            MasticTheme.redColor2
                            break;
                        case AgentIOPValueTypes.IMPULSION:
                            MasticTheme.purpleColor
                            break;
                        case AgentIOPValueTypes.DATA:
                            MasticTheme.greenColor
                            break;
                        case AgentIOPValueTypes.MIXED:
                            MasticTheme.whiteColor
                            break;
                        case AgentIOPValueTypes.UNKNOWN:
                            "#000000"
                            break;
                        default:
                            MasticTheme.whiteColor;
                            break;
                        }
                    } else {
                        MasticTheme.whiteColor;
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
                family: MasticTheme.textFontFamily;
            }

            color : MasticTheme.lightGreyColor
            verticalAlignment: Text.AlignVCenter;
            elide : Text.ElideRight;
        }


        Image {
            id:_imageCombo;
            anchors.verticalCenter: parent.verticalCenter;
            anchors.right: parent.right;
            anchors.rightMargin: 5
            rotation : (comboButton.checked ? 180 : 0);
            source : "image://I2svg/resources/SVG/mastic-pictos.svg#iconCombo";

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
            color:  MasticTheme.darkBlueGreyColor
        }

        ScrollView {
            id : _scrollView

            visible: comboButton.checked;

            anchors {
                top:  parent.top;
                bottom:  parent.bottom;
            }

            width: comboButton.width;
            height: ((combolist.count < 5) ? (combolist.count)*(comboButton.height+1) : 5*(comboButton.height+1) );

            style: MasticScrollViewStyle {
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

                    width:  comboButton.width
                    height: comboButton.height

                    color : _mouseAreaItem.containsPress ? MasticTheme.darkOrangeColor2 : "transparent"

                    //Separator
                    Rectangle {
                        anchors {
                            left: parent.left
                            leftMargin: 5
                            right: parent.right
                            rightMargin: 5
                            top : parent.top
                        }
                        height : 1
                        color : MasticTheme.lightGreyColor
                        visible : (combobox.inputsNumber !== 0 && index === combobox.inputsNumber)
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
                                    MasticTheme.orangeColor2
                                    break;
                                case AgentIOPValueTypes.BOOL:
                                    MasticTheme.orangeColor2
                                    break;
                                case AgentIOPValueTypes.DOUBLE:
                                    MasticTheme.orangeColor2
                                    break;
                                case AgentIOPValueTypes.STRING:
                                    MasticTheme.redColor2
                                    break;
                                case AgentIOPValueTypes.IMPULSION:
                                    MasticTheme.purpleColor
                                    break;
                                case AgentIOPValueTypes.DATA:
                                    MasticTheme.greenColor
                                    break;
                                case AgentIOPValueTypes.MIXED:
                                    MasticTheme.whiteColor
                                    break;
                                case AgentIOPValueTypes.UNKNOWN:
                                    "#000000"
                                    break;
                                default:
                                    MasticTheme.whiteColor;
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

                        color: MasticTheme.lightGreyColor

                        text: model.name
                        elide: Text.ElideRight

                        font {
                            family: MasticTheme.textFontFamily
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

