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

import "../popup" as Popup


Item {
    id: rootItem

    // Width of a column in the list view
    property real characteristicValueColumnWidth: 0

    // Current experimentation
    property var experimentation: null

    // Current subject. 'model' of this item
    property var subject: null

    // Flag indicating if the mouse is hovering the item
    property bool isMouseHovering: characteristicsMouseArea.containsMouse || buttonsMouseArea.containsMouse || editSubjectButton.containsMouse || deleteSubjectButton.containsMouse

    // Flag indicating if the current subject is being edited
    property bool isCurrentlyEditing: false

    // Flag indicating if a subject, among all subjects, is being edited
    // Bound by the parent
    property bool subjectEditionInProgress: false


    // Alias to override the characteristics values item's parent
    property alias characteristicsRowParent: characteristicsRow.parent

    // Alias to override the button item's parent
    property alias buttonsRowParent: buttonsRow.parent


    //
    // Signals
    //

    signal deleteSubject();


    //
    // Slots
    //

    // Discards any edited values and end edition
    function cancelEdition() {
        rootItem.isCurrentlyEditing = false
    }

    // Apply the edited values to the actual subjact properties
    function applyEdition() {
        if (rootItem.subject)
        {
            rootItem.subject.applyTemporaryPropertyValues()
        }

        rootItem.isCurrentlyEditing = false
    }


    //
    // Content
    //

    Rectangle {
        id: characteristicsRow

        height: rootItem.height
        width: rootItem.width

        color: rootItem.isCurrentlyEditing ? IngeScapeTheme.lightGreyColor
                                           : (rootItem.isMouseHovering ? IngeScapeTheme.veryLightGreyColor
                                                                       : IngeScapeTheme.whiteColor)

        MouseArea {
            id: characteristicsMouseArea
            anchors.fill: parent
            hoverEnabled: true
        }

        Row {
            anchors.fill: parent

            Repeater {
                model: rootItem.experimentation ? rootItem.experimentation.allCharacteristics : null

                delegate: Item {
                    id: characteristicDelegate
                    height: rootItem.height
                    width: rootItem.characteristicValueColumnWidth

                    property var characteristic: model ? model.QtObject : null

                    Text {
                        id: characteristicText
                        anchors{
                            fill: parent
                            leftMargin: 15
                        }

                        elide: Text.ElideRight
                        text: rootItem.subject
                              && characteristicDelegate.characteristic
                              && (typeof rootItem.subject.mapCharacteristicValues[characteristicDelegate.characteristic.name] !== "undefined")
                              ? rootItem.subject.mapCharacteristicValues[characteristicDelegate.characteristic.name]
                              : ""

                        verticalAlignment: Text.AlignVCenter

                        visible: !rootItem.isCurrentlyEditing

                        color: IngeScapeAssessmentsTheme.regularDarkBlueHeader
                        font {
                            family: IngeScapeTheme.textFontFamily
                            weight: index == 0 ? Font.Bold : Font.Medium
                            pixelSize: 16
                        }
                    }

                    Loader {
                        id: loaderEditor

                        anchors {
                            fill: parent
                            margins: 5
                            rightMargin: 33
                        }

                        visible: !characteristicText.visible

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

                            style: IngeScapeAssessmentsComboboxStyle {
                                frameVisible: false
                                listBackgroundColorIdle: IngeScapeTheme.veryLightGreyColor
                            }
                            scrollViewStyle: IngeScapeAssessmentsScrollViewStyle {
                                scrollBarSize: 4
                                verticalScrollbarMargin: 2
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
                            property var doubleValidator: DoubleValidator { locale: "en_US" }

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
                                    family: IngeScapeTheme.textFontFamily
                                    pixelSize: 16
                                }
                            }

                            onTextChanged: {
                                if (rootItem.subject && characteristicDelegate.characteristic)
                                {
                                    console.log("QML: on Text Changed " + text);

                                    rootItem.subject.tempMapCharacteristicValues[characteristicDelegate.characteristic.name] = text
                                }
                            }
                        }
                    }
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

    Rectangle {
        id: buttonsRow

        height: rootItem.height
        width: 146

        color: rootItem.isCurrentlyEditing ? IngeScapeTheme.lightGreyColor : "transparent"

        MouseArea {
            id: buttonsMouseArea
            anchors.fill: parent
            hoverEnabled: true
        }

        Row {
            spacing: 12

            anchors {
                right: parent.right
                rightMargin: 12
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
                    applyEdition()
                }
            }

            Button {
                id: cancelEditionSubjectButton
                anchors {
                    verticalCenter: parent.verticalCenter
                }

                height: 18
                width: 18

                opacity: rootItem.isCurrentlyEditing ? 1 : 0
                enabled: opacity > 0

                style: IngeScapeAssessmentsSvgButtonStyle {
                    releasedID: "cancel-edition"
                    disabledID: releasedID
                }

                onClicked: {
                    cancelEdition()
                }
            }
        }

        Row {
            spacing: 12

            anchors {
                right: parent.right
                rightMargin: 12
                verticalCenter: parent.verticalCenter
            }

            Button {
                id: deleteSubjectButton
                height: 30
                width: 40

                property bool containsMouse: __behavior.containsMouse

                opacity: rootItem.isMouseHovering && !rootItem.subjectEditionInProgress ? 1 : 0
                enabled: opacity > 0

                style: IngeScapeAssessmentsSvgButtonStyle {
                    releasedID: "delete-blue"
                    disabledID: releasedID
                }

                onClicked: {
                    deleteSubjectPopup.open()
                }
            }

            Button {
                id: editSubjectButton
                height: 30
                width: 40

                property bool containsMouse: __behavior.containsMouse

                opacity: rootItem.isMouseHovering && !rootItem.subjectEditionInProgress ? 1 : 0
                enabled: opacity > 0

                style: IngeScapeAssessmentsSvgButtonStyle {
                    releasedID: "edit"
                    disabledID: releasedID
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
    }

    Popup.DeleteConfirmationPopup {
        id: deleteSubjectPopup

        layerObjectName: "overlay2Layer"

        showPopupTitle: false
        anchors.centerIn: parent

        text: qsTr("Are you sure you want to delete the subject %1 ?").arg(rootItem.subject ? rootItem.subject.displayedId : "")

        height: 160
        width: 470

        onValidated: {
            // Close our Popup
            deleteSubjectPopup.close();

            // Emit the signal
            deleteSubject();
        }

        onCanceled: {
            // Close our Popup
            deleteSubjectPopup.close();
        }
    }
}

