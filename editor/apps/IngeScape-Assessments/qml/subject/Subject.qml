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
 *      Mathieu Soum <soum@ingenuity.io>
 *
 */

import QtQuick 2.9
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4

import I2Quick 1.0

import INGESCAPE 1.0


Rectangle {
    id: rootItem

    // Width of a column in the list view
    property real characteristicValueColumnWidth: 0

    // Current experimentation
    property var experimentation: null

    // Current subject. 'model' of this item
    property var subject: null

    // Flag indicating if the mouse is hovering the item
    property bool isMouseHovering: itemMouseArea.containsMouse || editSubjectButton.containsMouse || deleteSubjectButton.containsMouse

    // Flag indicating if the current subject is being edited
    property bool isCurrentlyEditing: false

    // Flag indicating if a subject, amongst all subjects, is being edited
    // Bound by the parent
    property bool subjectEditionInProgress: false

    color: rootItem.isCurrentlyEditing ? IngeScapeTheme.lightGreyColor
                                       : (rootItem.isMouseHovering ? IngeScapeTheme.veryLightGreyColor
                                                                   : IngeScapeTheme.whiteColor)


    //
    // Signals
    //

    signal deleteSubject();


    //
    // Content
    //

    MouseArea {
        id: itemMouseArea
        anchors.fill: parent
        hoverEnabled: true
    }

    Row {
        anchors {
            fill: parent
        }

        Repeater {
            model: rootItem.experimentation ? rootItem.experimentation.allCharacteristics : null

            delegate: Item {
                id: characteristicDelegate
                height: rootItem.height
                width: rootItem.characteristicValueColumnWidth

                property var characteristic: model ? model.QtObject : null

                Text {
                    anchors{
                        fill: parent
                        leftMargin: 15
                    }
                    text: rootItem.subject
                          && characteristicDelegate.characteristic
                          && (typeof rootItem.subject.mapCharacteristicValues[characteristicDelegate.characteristic.name] !== "undefined")
                          ? rootItem.subject.mapCharacteristicValues[characteristicDelegate.characteristic.name]
                          : ""

                    verticalAlignment: Text.AlignVCenter
                    visible: !rootItem.isCurrentlyEditing

                    color: IngeScapeTheme.blackColor
                    font {
                        family: IngeScapeTheme.textFontFamily
                        //weight: Font.Medium
                        pixelSize: 14
                    }
                }

                Loader {
                    id: loaderEditor

                    anchors {
                        fill: parent
                        margins: 5
                        rightMargin: 33
                    }

                    visible: rootItem.isCurrentlyEditing

                    // Load editor in function of the value type:
                    // - Enum --> combobox
                    // - NOT enum --> text field
                    sourceComponent: (model && (model.valueType === CharacteristicValueTypes.CHARACTERISTIC_ENUM)) ? componentComboboxEditor
                                                                                                                   : componentTextFieldEditor
                }


                //
                // component Combobox Editor
                //
                Component {
                    id: componentComboboxEditor

                    I2ComboboxStringList {
                        id: comboboxEditor

                        model: characteristicDelegate.characteristic ? characteristicDelegate.characteristic.enumValues : null

                        style: I2ComboboxStyle {
                            borderColorIdle: IngeScapeTheme.veryLightGreyColor
                            currentTextColorIdle: IngeScapeTheme.blackColor
                            font {
                                family: IngeScapeTheme.textFontFamily
                                //weight: Font.Medium
                                pixelSize: 14
                            }
                        }

                        Binding {
                            target: comboboxEditor
                            property: "selectedItem"
                            value: rootItem.subject.tempMapCharacteristicValues[characteristicDelegate.characteristic.name]
                        }

                        onSelectedItemChanged: {
                            if (comboboxEditor.selectedItem && rootItem.subject)
                            {
                                rootItem.subject.tempMapCharacteristicValues[characteristicDelegate.characteristic.name] = comboboxEditor.selectedItem
                            }
                        }

                        onVisibleChanged: {
                            if (visible && rootItem.subject)
                            {
                                var index = comboboxEditor.model.indexOf(rootItem.subject.tempMapCharacteristicValues[characteristicDelegate.characteristic.name]);
                                if (index > -1) {
                                    comboboxEditor.selectedIndex = index;
                                }
                            }
                        }
                    }
                }


                //
                // component TextField Editor
                //
                Component {
                    id: componentTextFieldEditor

                    TextField {
                        id: textFieldEditor

                        property var intValidator: IntValidator {}
                        property var doubleValidator: DoubleValidator {}

                        text: {
                            if (rootItem.subject
                                    && characteristicDelegate.characteristic
                                    && (typeof rootItem.subject.tempMapCharacteristicValues[characteristicDelegate.characteristic.name]) !== "undefined") {
                                return rootItem.subject.tempMapCharacteristicValues[characteristicDelegate.characteristic.name]
                            }
                            return ""
                        }

                        validator: if (characteristicDelegate.characteristic)
                                   {
                                       if (characteristicDelegate.characteristic.valueType === CharacteristicValueTypes.INTEGER) {
                                           return textFieldEditor.intValidator;
                                       }
                                       else if (characteristicDelegate.characteristic.valueType === CharacteristicValueTypes.DOUBLE) {
                                           return textFieldEditor.doubleValidator;
                                       }
                                       else {
                                           return null;
                                       }
                                   }
                                   else {
                                       return null;
                                   }

                        style: I2TextFieldStyle {
                            backgroundColor: IngeScapeTheme.whiteColor
                            borderColor: IngeScapeTheme.lightGreyColor
                            borderErrorColor: IngeScapeTheme.redColor
                            radiusTextBox: 5
                            borderWidth: 0
                            borderWidthActive: 1
                            textIdleColor: IngeScapeTheme.blackColor
                            textDisabledColor: IngeScapeTheme.veryLightGreyColor

                            padding.left: 10
                            padding.right: 5

                            font {
                                pixelSize: 14
                                family: IngeScapeTheme.textFontFamily
                            }
                        }

                        onTextChanged: {
                            if (rootItem.subject && characteristicDelegate.characteristic)
                            {
                                rootItem.subject.tempMapCharacteristicValues[characteristicDelegate.characteristic.name] = text
                            }
                        }
                    }
                }
            }
        }
    }

    Row {
        spacing: 12

        anchors {
            right: parent.right
            rightMargin: 18
            verticalCenter: parent.verticalCenter
        }

        Button {
            id: aplpyEditionSubjectButton
            height: 30
            width: 86

            opacity: rootItem.isCurrentlyEditing ? 1 : 0
            enabled: opacity > 0

            style: IngeScapeAssessmentsButtonStyle {
                text: "APPLY"
            }

            onClicked: {
                if (rootItem.subject)
                {
                    rootItem.subject.applyTemporaryPropertyValues()
                }

                rootItem.isCurrentlyEditing = false
            }
        }

        Button {
            id: cancelEditionSubjectButton
            height: 30
            width: 40

            opacity: rootItem.isCurrentlyEditing ? 1 : 0
            enabled: opacity > 0

            style: IngeScapeAssessmentsButtonStyle {
                text: "C"
            }

            onClicked: {
                rootItem.isCurrentlyEditing = false
            }
        }
    }

    Row {
        spacing: 12

        anchors {
            right: parent.right
            rightMargin: 18
            verticalCenter: parent.verticalCenter
        }

        Button {
            id: deleteSubjectButton
            height: 30
            width: 40

            property bool containsMouse: __behavior.containsMouse

            opacity: rootItem.isMouseHovering && !rootItem.subjectEditionInProgress ? 1 : 0
            enabled: opacity > 0

            style: IngeScapeAssessmentsButtonStyle {
                text: "D"
            }

            onClicked: {
                deleteSubject()
            }
        }

        Button {
            id: editSubjectButton
            height: 30
            width: 40

            property bool containsMouse: __behavior.containsMouse

            opacity: rootItem.isMouseHovering && !rootItem.subjectEditionInProgress ? 1 : 0
            enabled: opacity > 0

            style: IngeScapeAssessmentsButtonStyle {
                text: "E"
            }

            onClicked: {
                // Clean previous temporary
                if (rootItem.subject)
                {
                    rootItem.subject.resetTemporaryPropertyValues()
                }

                rootItem.isCurrentlyEditing = true
            }
        }
    }

    Rectangle {
        id: bottomSeparator
        anchors {
            left: parent.left
            right: parent.right
            bottom: parent.bottom
        }
        height: 2
        color: rootItem.isCurrentlyEditing ? IngeScapeTheme.lightGreyColor : IngeScapeTheme.veryLightGreyColor
    }
}

