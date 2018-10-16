import QtQuick 2.0

import I2Quick 1.0
import QtQuick.Controls 2.0 as Controls2
import "qrc:/qml/styles/combobox/" as I2StyleComboboxPath

/**
  * This component is the base delegate used by the I2ComboboxBase derived
  * components.
  *
  * It represents the manner in which elements of the combobox's list view
  * will be displayed. It may be sufficient to derive this class to custom
  * without the render of a combobox withtout deriving the entire I2ComboboxBase.
  *
  * The functions onDelegateClicked(index) and getItemText(index) MUST be
  * defined in sub-classes in order to correctly display model properties
  * and return click events to the parent combobox.
  */
Item {
    id: comboboxDelegate

    property Item comboboxStyle: I2StyleComboboxPath.I2ComboboxStyle{}
    property Component component: component
    property int selection: -1

    // Called from the component's MouseArea
    // 'index' is the index of the clicked component inside the model.
    // MUST be redefined when using I2ComboboxBaseDelegate
    function onDelegateClicked(index) {}

    // Called from the component to get the text of the current item to display
    // 'index' is the index of the component to be displayed inside the model.
    // MUST be redefined when using I2ComboboxBaseDelegate
    function getItemText(index) {}

    Component {
        id: component
        Rectangle {
            id: rect
            color:  (mouseArea.containsPress ? comboboxStyle.listBackgroundColorPressed
                                             : (selection === index ? comboboxStyle.listBackgroundColorSelected
                                                                    : comboboxStyle.listBackgroundColorIdle));
            height: comboboxDelegate.height
            width: comboboxDelegate.width

            border.width: comboboxStyle.listCellBorderWidth
            border.color :comboboxStyle.listCellBorderColor
            radius: 0;

            Text {
                id: text
                anchors {
                    verticalCenter: parent.verticalCenter
                    left: parent.left
                    leftMargin: 10
                    right: parent.right
                }
                elide : Text.ElideRight
                font:comboboxStyle.itemsFont;
                color: (mouseArea.containsPress ? comboboxStyle.listItemTextColorPressed
                                                : (selection === index ? comboboxStyle.listItemTextColorSelected
                                                                       : comboboxStyle.listItemTextColorIdle));
                text: getItemText(index);
            }

            MouseArea {
                id: mouseArea
                anchors.fill: parent
                hoverEnabled: true
                onClicked: {
                    onDelegateClicked(index);
                }
            }

            Controls2.ToolTip {
                visible: mouseArea.containsMouse
                delay: 500
                text: text.text
            }
        }
    }
}
