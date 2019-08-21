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

// Needed to access to ToolTip (https://doc.qt.io/qt-5.11/qml-qtquick-controls2-tooltip.html)
import QtQuick.Controls 2.0 as Controls2

import I2Quick 1.0

import INGESCAPE 1.0

//import "theme" as Theme


Item {
    id: rootItem

    height: 30


    //--------------------------------------------------------
    //
    //
    // Properties
    //
    //
    //--------------------------------------------------------

    property IndependentVariableM variable: null;

    // Keep type "var" because the C++ use a QVariant
    property var variableValue: "";



    //--------------------------------
    //
    //
    // Signals
    //
    //
    //--------------------------------

    // Independent Variable Value Updated
    signal independentVariableValueUpdated(var value);

    //--------------------------------------------------------
    //
    //
    // Content
    //
    //
    //--------------------------------------------------------


    I2SvgItem {
        id: infoSvg

        anchors {
            left: parent.left
            verticalCenter: parent.verticalCenter
        }

        svgFileCache: IngeScapeAssessmentsTheme.svgFileIngeScapeAssessments
        svgElementId: "info-description"

        MouseArea {
            id: mouseAreaInfo
            anchors.fill: parent
            hoverEnabled: true
        }

        Controls2.ToolTip {
            visible: mouseAreaInfo.containsMouse
            delay: 800
            text: {
                var tooltipText = rootItem.variable ? IndependentVariableValueTypes.enumToString(rootItem.variable.valueType) + "\n" + rootItem.variable.description : ""
                if (tooltipText.length > 150)
                {
                    tooltipText = tooltipText.substring(0, 147) + "..."
                }
                return tooltipText
            }
        }
    }

    Text {
        id: indeVarName

        anchors {
            left: parent.left
            leftMargin: 26
            verticalCenter: parent.verticalCenter
            right: loaderEditor.left
            rightMargin: 15
        }

        text: rootItem.variable ? rootItem.variable.name + ":" : ""

        elide: Text.ElideRight
        color: IngeScapeAssessmentsTheme.regularDarkBlueHeader
        font {
            family: IngeScapeTheme.textFontFamily
            weight: Font.Medium
            pixelSize: 16
        }
    }

    Loader {
        id: loaderEditor

        anchors {
            left: parent.left
            leftMargin: 213
            right: parent.right
            top: parent.top
            bottom: parent.bottom
        }

        // Load editor in function of the value type:
        // - Enum --> combobox
        // - NOT enum --> text field
        sourceComponent: (rootItem.variable && (rootItem.variable.valueType === IndependentVariableValueTypes.INDEPENDENT_VARIABLE_ENUM)) ? componentComboboxEditor
                                                                                                                                          : componentTextFieldEditor
    }


    //
    // component Combobox Editor
    //
    Component {
        id: componentComboboxEditor

        I2ComboboxStringList {
            id: comboboxEditor

            model: rootItem.variable ? rootItem.variable.enumValues : null

            style: IngeScapeAssessmentsComboboxStyle {
                frameVisible: false
                listBackgroundColorIdle: IngeScapeTheme.veryLightGreyColor
                backgroundColorIdle: IngeScapeTheme.veryLightGreyColor
            }
            scrollViewStyle: IngeScapeAssessmentsScrollViewStyle {
                scrollBarSize: 4
                verticalScrollbarMargin: 2
            }

            Binding {
                target: comboboxEditor
                property: "selectedItem"
                value: rootItem.variableValue
            }

            onSelectedItemChanged: {

                if (comboboxEditor.selectedItem)
                {
                    // Emit the signal "Independent Variable Value Updated"
                    rootItem.independentVariableValueUpdated(comboboxEditor.selectedItem);
                }
            }

            Component.onCompleted: {
                if ((comboboxEditor.selectedIndex < 0) && (typeof rootItem.variableValue !== 'undefined'))
                {
                    var index = comboboxEditor.model.indexOf(rootItem.variableValue);
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

            text: (typeof rootItem.variableValue !== 'undefined') ? rootItem.variableValue : ""

            validator: if (rootItem.variable)
                       {
                           if (rootItem.variable.valueType === IndependentVariableValueTypes.INTEGER) {
                               return textFieldEditor.intValidator;
                           }
                           else if (rootItem.variable.valueType === IndependentVariableValueTypes.DOUBLE) {
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
                backgroundColor: IngeScapeTheme.veryLightGreyColor
                borderColor: IngeScapeAssessmentsTheme.blueButton
                borderErrorColor: IngeScapeTheme.redColor
                radiusTextBox: 5
                borderWidth: 0
                borderWidthActive: 2
                textIdleColor: IngeScapeAssessmentsTheme.regularDarkBlueHeader;
                textDisabledColor: IngeScapeAssessmentsTheme.lighterDarkBlueHeader

                padding.left: 12
                padding.right: 12

                font {
                    pixelSize: 16
                    family: IngeScapeTheme.textFontFamily
                }
            }

            onTextChanged: {
                // Emit the signal "Independent Variable Value Updated"
                rootItem.independentVariableValueUpdated(textFieldEditor.text);
            }
        }
    }
}
