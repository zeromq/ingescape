/*
 *	IngeScape Assessments
 *
 *  Copyright © 2019 Ingenuity i/o. All rights reserved.
 *
 *	See license terms for the rights and conditions
 *	defined by copyright holders.
 *
 *
 *	Contributors:
 *      Vincent Peyruqueou <peyruqueou@ingenuity.io>
 *
 */

import QtQuick 2.9
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4

import I2Quick 1.0

import INGESCAPE 1.0

//import "theme" as Theme


Item {
    id: rootItem

    //anchors.fill: parent

    width: parent.width
    height: 30


    //--------------------------------------------------------
    //
    //
    // Properties
    //
    //
    //--------------------------------------------------------

    property SubjectM modelM: null;

    property var allCharacteristics: null;

    property bool isCurrentlyEditing: false;


    //--------------------------------
    //
    //
    // Signals
    //
    //
    //--------------------------------

    // Delete Subject
    signal deleteSubject();



    //--------------------------------------------------------
    //
    //
    // Content
    //
    //
    //--------------------------------------------------------

    Rectangle {
        id: background

        anchors.fill: parent

        color: "#44222222"
        border {
            color: "black"
            width: 1
        }

        /*Text {
            id: txtUID

            anchors {
                left: parent.left
                leftMargin: 5
                verticalCenter: parent.verticalCenter
            }
            width: 150

            text: rootItem.modelM ? rootItem.modelM.uid : ""

            color: IngeScapeTheme.whiteColor
            font {
                family: IngeScapeTheme.textFontFamily
                //weight : Font.Medium
                pixelSize : 12
            }
        }*/

        /*Text {
            id: txtName

            anchors {
                left: txtUID.right
                leftMargin: 5
                verticalCenter: parent.verticalCenter
            }
            width: 150

            text: rootItem.modelM ? rootItem.modelM.name : ""

            color: IngeScapeTheme.whiteColor
            font {
                family: IngeScapeTheme.textFontFamily
                //weight : Font.Medium
                pixelSize : 12
            }
        }*/

        Row {
            anchors.fill: parent

            spacing: 0

            Repeater {
                model: rootItem.allCharacteristics

                delegate: Rectangle {
                    id: valueColumn

                    property CharacteristicM characteristic: model.QtObject

                    anchors {
                        top: parent.top
                        bottom: parent.bottom
                    }
                    width: 150

                    color: "transparent"
                    border {
                        color: "black"
                        width: 1
                    }

                    Text {

                        anchors.centerIn: parent

                        visible: !rootItem.isCurrentlyEditing

                        text: (rootItem.modelM && valueColumn.characteristic) ? rootItem.modelM.getValueOfCharacteristic(valueColumn.characteristic.name)
                                                                              : ""

                        color: IngeScapeTheme.whiteColor
                        font {
                            family: IngeScapeTheme.textFontFamily
                            //weight : Font.Medium
                            pixelSize : 12
                        }
                    }

                    TextField {
                        id: txtEditor

                        anchors {
                            fill: parent
                            margins: 1
                        }

                        visible: rootItem.isCurrentlyEditing && valueColumn.characteristic
                                 && (valueColumn.characteristic.valueType !== CharacteristicValueTypes.CHARACTERISTIC_ENUM)

                        text: (rootItem.modelM && valueColumn.characteristic) ? rootItem.modelM.getValueOfCharacteristic(valueColumn.characteristic.name)
                                                                              : ""

                        style: I2TextFieldStyle {
                            backgroundColor: IngeScapeTheme.darkBlueGreyColor
                            borderColor: IngeScapeTheme.whiteColor
                            borderErrorColor: IngeScapeTheme.redColor
                            radiusTextBox: 1
                            borderWidth: 0;
                            borderWidthActive: 1
                            textIdleColor: IngeScapeTheme.whiteColor;
                            textDisabledColor: IngeScapeTheme.darkGreyColor

                            padding.left: 3
                            padding.right: 3

                            font {
                                pixelSize:15
                                family: IngeScapeTheme.textFontFamily
                            }
                        }

                        onTextChanged: {
                            if (rootItem.modelM && valueColumn.characteristic)
                            {
                                //console.log("QML: on Text Changed " + txtEditor.text);
                                rootItem.modelM.setValueOfCharacteristic(txtEditor.text, valueColumn.characteristic);
                            }
                        }
                    }

                    // FIXME: use a Loader instead of visible
                    I2ComboboxStringList {
                        id: cmbEditor

                        anchors {
                            fill: parent
                            margins: 1
                        }

                        visible: rootItem.isCurrentlyEditing && valueColumn.characteristic
                                 && (valueColumn.characteristic.valueType === CharacteristicValueTypes.CHARACTERISTIC_ENUM)

                        model: valueColumn.characteristic ? valueColumn.characteristic.enumValues : null

                        onSelectedItemChanged: {
                            if (rootItem.modelM && cmbEditor.selectedItem && valueColumn.characteristic)
                            {
                                //console.log("QML on Selected Item Changed " + cmbEditor.selectedItem);
                                rootItem.modelM.setValueOfCharacteristic(cmbEditor.selectedItem, valueColumn.characteristic);
                            }
                        }
                    }
                }
            }
        }
    }

    Row {
        anchors {
            right: parent.right
        }
        height: parent.height

        Button {
            id: btnEdit

            height: parent.height

            checkable: true

            checked: rootItem.isCurrentlyEditing

            text: rootItem.isCurrentlyEditing ? "Validate" : "Edit"

            //visible: mouseArea.containsMouse

            onClicked: {
                if (rootItem.modelM)
                {
                    if (rootItem.isCurrentlyEditing)
                    {
                        console.log("QML: Validate Subject " + rootItem.modelM.uid);

                        rootItem.isCurrentlyEditing = false;
                    }
                    else
                    {
                        console.log("QML: Edit Subject " + rootItem.modelM.uid);

                        rootItem.isCurrentlyEditing = true;
                    }
                }
            }
        }

        Button {
            id: btnDelete

            height: parent.height

            text: "Delete"

            //visible: mouseArea.containsMouse

            onClicked: {
                if (rootItem.modelM)
                {
                    //console.log("QML: Delete Subject " + rootItem.modelM.uid);

                    // Emit the signal "Delete Subject"
                    rootItem.deleteSubject();
                }
            }
        }
    }

}
