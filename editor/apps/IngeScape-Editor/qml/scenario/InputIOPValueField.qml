import QtQuick 2.0

import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4

import I2Quick 1.0
import INGESCAPE 1.0

// Item holding a text field and a combobox with "TRUE" and "FALSE" as values.
// Will be used for user inputs for IOP values.
//
// IOP may be of different types, requiering different input methods.
// * IMPULSION don't have a proper value. No field will be shown.
// * BOOLEAN can only be "TRUE" or "FALSE". The input field will be a combobox.
// * Others, such as STRING, INTEGER or DOUBLE, require the user to type in a value. The input field will have a validator limiting the input values regarding the IOP's value type.
Item {
    id: rootItem

    // IOP view model. MUST be set on client side
    property var iopVM: undefined
    // Set this bool to false to force the item to be hidden. MUST be set on client side.
    property bool forceHide: true

    // Utility properties to ease code readability
    property bool _modelIopIsNotImpulsion: {
        if (!forceHide && iopVM && iopVM.firstModel)
        {
            return (iopVM.firstModel.agentIOPValueType !== AgentIOPValueTypes.IMPULSION)
        }
        else
        {
            return false
        }
    }
    property bool _modelIopIsBool: {
        if (!forceHide && iopVM && iopVM.firstModel)
        {
            return (iopVM.firstModel.agentIOPValueType === AgentIOPValueTypes.BOOL)
        }
        else
        {
            return false
        }
    }

    // Force the text field and the combobox to revalidate themselves when the IOP has been change by the user.
    // This ensure that the input in the text field or the combobox is always valid against the currently selected IOP's type.
    function revalidateInput()
    {
        textField.revalidateText()
        comboboxBooleanValue.revalidateCombo()
    }

    // Returns the actual value associated with our model
    // The value will be shown in the field and may be modified by the user.
    // This method MUST me implememented on client side
    function getModelValue() {}

    // Sets the actual value associated with our model
    // The value will be shown in the field and may be modified by the user.
    // This method MUST me implememented on client side
    function setModelValue(value) {}

    height: 25

    // Target Value
    TextField {
        id: textField
        anchors.fill: parent

        height: 25

        visible: !forceHide && rootItem._modelIopIsNotImpulsion && !rootItem._modelIopIsBool
        enabled: visible

        horizontalAlignment: TextInput.AlignLeft
        verticalAlignment: TextInput.AlignVCenter

        // Force the content's format according to the IOP value type.
        // e.g. Switching from DOUBLE to INTEGER will truncate the value to its integer part (no decimals).
        function revalidateText()
        {
            if (visible) {
                if (iopVM)
                {
                    if (iopVM.firstModel)
                    {
                        var iopValueType = iopVM.firstModel.agentIOPValueType
                        if (iopValueType === AgentIOPValueTypes.INTEGER)
                        {
                            // Checking Number conversion to always show a valid number (instead of "nan")
                            var integerValue = Number(getModelValue())
                            if (isNaN(integerValue))
                            {
                                setModelValue(0)
                            }
                            else
                            {
                                setModelValue(Math.max(Math.min(NumberConstants.MAX_INTEGER, integerValue), NumberConstants.MIN_INTEGER).toFixed(0))
                            }
                        }
                        else if (iopValueType === AgentIOPValueTypes.DOUBLE)
                        {
                            // Checking Number conversion to always show a valid number (instead of "nan")
                            var doubleValue = Number(getModelValue())
                            if (isNaN(doubleValue))
                            {
                                setModelValue(0)
                            }
                            else
                            {
                                setModelValue(Math.max(Math.min(NumberConstants.MAX_DOUBLE, doubleValue), NumberConstants.MIN_DOUBLE).toPrecision())
                            }
                        }
                    }
                    text = getModelValue()
                }
                else
                {
                    text = ""
                }
            }
        }

        property var intValidator:    Int32Validator {}
        property var doubleValidator: BoundsDoubleValidator {}
        property var stringValidator: StringValidator {}

        validator: {
            if (iopVM && iopVM.firstModel)
            {
                var iopValueType = iopVM.firstModel.agentIOPValueType
                if (iopValueType === AgentIOPValueTypes.DOUBLE)
                {
                    return doubleValidator
                }
                else if (iopValueType === AgentIOPValueTypes.INTEGER)
                {
                    return intValidator
                }
            }

            // StringValidator is the default validator
            return stringValidator
        }

        text: iopVM ? getModelValue() : ""

        style: I2TextFieldStyle {
            backgroundColor: IngeScapeTheme.darkBlueGreyColor
            borderColor: IngeScapeTheme.whiteColor;
            borderErrorColor: IngeScapeTheme.redColor
            radiusTextBox: 1
            borderWidth: 0;
            borderWidthActive: 1
            textIdleColor: IngeScapeTheme.whiteColor;
            textDisabledColor: IngeScapeTheme.darkGreyColor;

            padding.left: 3
            padding.right: 3

            font {
                pixelSize:15
                family: IngeScapeTheme.textFontFamily
            }

        }

        onActiveFocusChanged: {
            if (!activeFocus)
            {
                // Move cursor to our first character when we lose focus
                // (to always display the beginning or our text instead of
                // an arbitrary part if our text is too long)
                cursorPosition = 0;
            }
            else
            {
                textField.selectAll();
            }
        }

        Binding {
            target: textField
            property: "text"
            value: if (iopVM)
                   {
                       getModelValue()
                   }
                   else
                   {
                       "";
                   }
        }

        onTextChanged: {
            if (activeFocus && iopVM)
            {
                setModelValue(text)
            }
        }
    }

    I2ComboboxStringList {
        id: comboboxBooleanValue

        anchors.fill: parent

        visible: !forceHide && rootItem._modelIopIsBool
        enabled: visible

        // Force the value to "1" (aka. "TRUE") for every value that is not "0" (aka. "FALSE")
        // e.g. "1337.42" will be transformed to "1" while "0" will stay "0"
        function revalidateCombo()
        {
            if (visible)
            {
                if (iopVM)
                {
                    if (Number(getModelValue()) !== 0)
                    {
                        setModelValue("1")
                    }
                }
            }
        }

        model: [ "FALSE", "TRUE" ]

        style: IngeScapeComboboxStyle {}

        Binding {
            target: comboboxBooleanValue
            property: "selectedIndex"
            value: if (iopVM && getModelValue() !== "") // Empty values from the text field won't change the value of the combobox
                   {
                       // Only "1" and "0" values from the TextField update the combobox since its the two values assigned to the model by this combobox
                       if (Number(getModelValue()) === 0)
                       {
                           comboboxBooleanValue.model.indexOf("FALSE")
                       }
                       else if (Number(getModelValue()) === 1)
                       {
                           comboboxBooleanValue.model.indexOf("TRUE")
                       }
                   }
        }

        onSelectedItemChanged: {
            if (selectedIndex >= 0 && iopVM)
            {
                setModelValue( (selectedItem === "TRUE") ? "1" : "0" )
            }
        }
    }
}
