import QtQuick 2.0

import QtQuick.Controls 2.0 as Controls2

import I2Quick 1.0
import INGESCAPE 1.0

/**
  * Custom combobox for IngeScape editor based on I2ComboboxItemModel.
  *
  * It is used to represent IOP in comboboxes with a little color based on
  * the type of the entry (i.e. I, O or P) and a custom tooltip that differs from
  * the text shown in the combobox or the listview.
  */
I2ComboboxItemModel {
    id: agentEffectCombo

    style: IngeScapeComboboxStyle {}
    scrollViewStyle: IngeScapeScrollViewStyle {}
    _mouseArea.hoverEnabled: true

    // number of inputs and outputs in IOP list (in order to place the separators)
    property alias inputsNumber: customDelegate.inputsNumber;
    property alias outputsNumber: customDelegate.outputsNumber;
    property alias parametersNumber: customDelegate.parametersNumber;

    function modelToString(entry)
    {
        if (entry) {
            return entry.name;
        }
        else {
            return "";
        }
    }

    function _updateCurrentSelection() {
        if (selectedIndex >= 0) {
            selectedItem = model.get(selectedIndex);
            text = modelToString(selectedItem);
            tooltip.text = text + " (" + AgentIOPValueTypes.enumToString(selectedItem.firstModel.agentIOPValueType) + " " + AgentIOPTypes.enumToString(selectedItem.firstModel.agentIOPType).toLowerCase() + ")";
        }
        else {
            selectedItem = undefined;
            text = "";
            tooltip.text = "";
        }
    }

    // We change the text anchor to shift it to the right of the circle.
    // We cannot use
    //   anchors.left: circleSelected.right
    // since circleSelected and the text element are not siblings.
    _textAnchor.leftMargin: circleSelected.x + circleSelected.width + 10;

    // Definition of the little color circle (left to the text)
    Rectangle {
        id : circleSelected
        anchors {
            left : comboButton.left
            leftMargin: 5
            verticalCenter: comboButton.verticalCenter
        }

        visible: (selectedItem !== null);
        width : 11
        height : width
        radius : width/2

        color : (selectedItem && selectedItem.firstModel) ? IngeScapeEditorTheme.colorOfIOPTypeWithConditions(selectedItem.firstModel.agentIOPValueTypeGroup, true)
                                                          : IngeScapeTheme.whiteColor
    }

    Controls2.ToolTip {
        id: tooltip
        delay: 500
        visible: _mouseArea.containsMouse
        text: agentEffectCombo.text
    }

    delegate: customDelegate.component

    IngeScapeToolTipComboboxDelegateAgentsIOP {
        id: customDelegate

        comboboxStyle: agentEffectCombo.style
        selection: agentEffectCombo.selectedIndex

        height: agentEffectCombo.comboButton.height
        width:  agentEffectCombo.comboButton.width

        // Called from the component's MouseArea
        // 'index' is the index of the clicked component inside the model.
        function onDelegateClicked(index) {
            agentEffectCombo.onDelegateClicked(index)
        }

        // Called from the component to get the text of the current item to display
        // 'index' is the index of the component to be displayed inside the model.
        function getItemText(index) {
            return agentEffectCombo.modelToString(agentEffectCombo.model.get(index));
        }
    }
}
