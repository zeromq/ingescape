import QtQuick 2.0

import QtQuick.Controls 2.0 as Controls2

import I2Quick 1.0
import INGESCAPE 1.0


I2ComboboxItemModel {
    id: agentEffectCombo

    style: IngeScapeComboboxStyle {}
    scrollViewStyle: IngeScapeScrollViewStyle {}
    mouseArea.hoverEnabled: true

    function modelToString(entry)
    {
        return entry.name;
    }


    Controls2.ToolTip {
        delay: 500
        visible: agentEffectCombo.containsMouse
        text: agentEffectCombo.text
    }

    delegate: customDelegate.component

    IngeScapeToolTipComboboxDelegate {
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
