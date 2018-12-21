import QtQuick 2.0

// Dependency for Tooltip
import QtQuick.Controls 1.4
import QtQuick.Controls.Private 1.0

import I2Quick 1.0
import INGESCAPE 1.0
import "qrc:/qml/styles/combobox/" as I2StyleComboboxPath

/**
  * This component is the based on the implementation of I2ComboboxBaseDelegate.
  *
  * It adds tooltips to the delegate component.
  * Tooltip mechanics are "old school" (pre QtQuick Controls 2.0) because
  * of a bug with trackpads, at least under Mac OSX.
  */
Item {
    id: comboboxDelegate

    property Item comboboxStyle: I2StyleComboboxPath.I2ComboboxStyle{}
    property Component component: component
    property int selection: -1

    // number of inputs and outputs in IOP list (in order to place the separators)
    property int inputsNumber: 0;
    property int outputsNumber: 0;
    property int parametersNumber: 0;

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
            border.color: comboboxStyle.listCellBorderColor
            radius: 0;

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
                visible: ((comboboxDelegate.inputsNumber > 0) && (index === comboboxDelegate.inputsNumber))
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
                visible: ((comboboxDelegate.parametersNumber > 0) && (index === (comboboxDelegate.inputsNumber + comboboxDelegate.outputsNumber)))
            }

            Rectangle {
                id: circle
                anchors {
                    left: parent.left
                    leftMargin: 5
                    verticalCenter: parent.verticalCenter
                }

                width: 11
                height: width
                radius: width/2

                color: switch (model.firstModel.agentIOPValueType)
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
                id: text
                anchors {
                    verticalCenter: parent.verticalCenter
                    left: circle.right
                    leftMargin: 10
                    right: parent.right
                }
                elide : Text.ElideRight
                font:comboboxStyle.itemsFont;
                color: (mouseArea.containsPress ? comboboxStyle.listItemTextColorPressed
                                                : (selection === index ? comboboxStyle.listItemTextColorSelected
                                                                       : comboboxStyle.listItemTextColorIdle));
                text: index >= 0 ? getItemText(index) : "";
            }

            MouseArea {
                id: mouseArea
                anchors.fill: parent
                hoverEnabled: true
                onClicked: {
                    onDelegateClicked(index);
                }

                // Tooltip is handled without QtQuick.Controls 2.0
                // Because of an issues with the trackpad (Window is no more interactive)
                onExited: {
                    Tooltip.hideText();
                }
                onCanceled: {
                    Tooltip.hideText();
                }

                Timer {
                    interval: 400
                    running: mouseArea.containsMouse

                    onTriggered: {
                        var text = modelToString(model) + " (" + AgentIOPValueTypes.enumToString(model.firstModel.agentIOPValueType) + " " + AgentIOPTypes.enumToString(model.firstModel.agentIOPType).toLowerCase() + ")";
                        Tooltip.showText(mouseArea, Qt.point(mouseArea.mouseX, mouseArea.mouseY), text);
                    }
                }
            }
        }
    }
}
