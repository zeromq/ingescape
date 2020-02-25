/*
 *	IngeScape Editor
 *
 *  Copyright © 2017 Ingenuity i/o. All rights reserved.
 *
 *	See license terms for the rights and conditions
 *	defined by copyright holders.
 *
 *
 *	Contributors:
 *      Alexandre Lemort   <lemort@ingenuity.io>
 *      Vincent Peyruqueou <peyruqueou@ingenuity.io>
 *
 */

import QtQuick 2.8
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4
import QtQuick.Controls 2.0 as Controls2

import I2Quick 1.0

import INGESCAPE 1.0

// agentsmapping sub-directory
import "agentsmapping" as AgentsMapping
//import "theme" as Theme

// Popups
import "./popup/" as Popups;

Item {
    id: rootItem

    anchors.fill: parent

    //--------------------------------
    //
    // Properties
    //
    //--------------------------------

    // Controller associated to our view
    property AgentsMappingController controller : null;

    property bool isMappingControlled: IngeScapeEditorC.modelManager ? IngeScapeEditorC.modelManager.isMappingControlled : false


    //-----------------------------------------
    //
    // Functions
    //
    //-----------------------------------------




    //--------------------------------
    //
    // Content
    //
    //--------------------------------

    Item {
        id: content

        anchors.fill: parent

        onWidthChanged: {
            if (controller) {
                controller.viewWidth = width;
            }
        }
        onHeightChanged: {
            if (controller) {
                controller.viewHeight = height;
            }
        }


        //
        // Node graph view
        //
        AgentsMapping.NodeGraphView {
            id: nodeGraphView

            anchors.fill: parent

            controller: rootItem.controller

            Component.onCompleted: {
                if (controller) {
                    //console.log("QML: Graph View completed !");
                    // Update the flag
                    controller.isLoadedView = true;

                    // Open the popup about mapping modifications
                    if (IgsNetworkController.isStarted)
                    {
                        mappingModificationsPopup.open();
                    }
                }
            }
        }

        NetworkConnectionInformationItem {
            id: networkConfigurationInfo

            anchors {
                top: parent.top
                topMargin: 15
                right: parent.right
                rightMargin: 13
            }

            currentNetworkDevice: IngeScapeEditorC.networkDevice
            currentPort: IngeScapeEditorC.port

            listOfNetworkDevices: IgsNetworkController ? IgsNetworkController.availableNetworkDevices : null

            settingsReleasedId : IngeScapeEditorC.modelManager.isMappingControlled ? "mapping-mode-control" : "mapping-mode-observe"
            settingsHighlightedId: IngeScapeEditorC.modelManager.isMappingControlled ? "mapping-mode-control" : "mapping-mode-observe"
            settingsPressedId : IngeScapeEditorC.modelManager.isMappingControlled ? "mapping-mode-control" : "mapping-mode-observe"


            // Add extra selection for mapping mode
            // NB : extraContent property of NetworkConnectionInformationItem
            Item {
                id: mappingForm

                // Must have fixed height and width
                height: fullLabelMappingMode.height + fullLabelMappingMode.anchors.topMargin +
                        selectMappingModeCombobox.height + selectMappingModeCombobox.anchors.topMargin +
                        lowSeparator.height + lowSeparator.anchors.topMargin
                width: networkConfigurationInfo.contentWidth

                property bool resMappingControlled: selectMappingModeCombobox.selectedIndex === 0

                Item {
                    id: fullLabelMappingMode
                    anchors {
                        left: parent.left
                        right: parent.right
                        top: parent.top
                        topMargin: 13
                    }

                    height: childrenRect.height

                    Text {
                        id: labelMappingMode

                        anchors {
                            left: parent.left
                            top: parent.top
                        }

                        text: qsTr("Mapping mode")

                        color: IngeScapeTheme.lightGreyColor

                        font {
                            family: IngeScapeTheme.textFontFamily
                            pixelSize: 16
                        }
                    }

                    LabellessSvgButton {
                        id: infoMappingMode

                        anchors {
                            left: labelMappingMode.right
                            leftMargin: 5
                            verticalCenter: labelMappingMode.verticalCenter
                        }

                        pressedID: "mapping-mode-info"
                        releasedID: "mapping-mode-info"

                        Controls2.ToolTip {
                            delay: Qt.styleHints.mousePressAndHoldInterval
                            visible: infoMappingMode.enabled && infoMappingMode.__behavior.containsMouse
                            text: qsTr("TODO description")
                        }
                    }
                }

                I2ComboboxStringList {
                    id: selectMappingModeCombobox

                    anchors {
                        left: parent.left
                        right: parent.right
                        top: fullLabelMappingMode.bottom
                        topMargin: 9
                    }

                    height : 22

                    style: IngeScapeComboboxStyle {}
                    scrollViewStyle: IngeScapeScrollViewStyle {}

                    _mouseArea.hoverEnabled: true

                    placeholderText: qsTr("Select a network device...")

                    model: ["CONTROL", "OBSERVE"]
                    selectedIndex: IngeScapeEditorC.modelManager.isMappingControlled ? 0 : 1;


                    // We change the text anchor to shift it to the right of the picto.
                    // We cannot use
                    //   anchors.left: containerPictoSelected.right
                    // since containerPictoSelected and the text element are not siblings.
                    _textAnchor.leftMargin: containerPictoSelected.x + containerPictoSelected.width

                    Item {
                        id: containerPictoSelected

                        anchors {
                            left: parent.left
                            verticalCenter: parent.verticalCenter
                        }

                        width : 25
                        height: childrenRect.height

                        SvgImage {
                            id: picto

                            anchors {
                                horizontalCenter: parent.horizontalCenter
                            }

                            svgElementId : (selectMappingModeCombobox.selectedItem === "CONTROL") ? "mapping-mode-control" : "mapping-mode-observe"
                        }
                    }

                    delegate: customDelegate.component

                    IngeScapeToolTipComboboxDelegateMappingMode {
                        id: customDelegate

                        comboboxStyle: selectMappingModeCombobox.style
                        selection: selectMappingModeCombobox.selectedIndex

                        height: selectMappingModeCombobox.height
                        width:  selectMappingModeCombobox.width

                        // Called from the component's MouseArea
                        // 'index' is the index of the clicked component inside the model.
                        function onDelegateClicked(index) {
                            selectMappingModeCombobox.onDelegateClicked(index)
                        }

                        // Called from the component to get the text of the current item to display
                        // 'index' is the index of the component to be displayed inside the model.
                        function getItemText(index) {
                            return selectMappingModeCombobox.modelToString(selectMappingModeCombobox.model[index]);
                        }
                    }
                }

                // Second separator
                Rectangle {
                    id: lowSeparator

                    anchors {
                        left: parent.left
                        right: parent.right
                        top: selectMappingModeCombobox.bottom
                        topMargin: 15
                    }

                    height: 1

                    color: IngeScapeTheme.editorsBackgroundBorderColor
                }
            }

            onConnectChanged: {
                if (connect)
                {
                    mappingModificationsPopup.open();
                }
                else
                {
                    IgsNetworkController.stop();
                }
            }

            onWillOpenEditionMode: {
                // Update our list of available network devices
                IgsNetworkController.updateAvailableNetworkDevices();

                // Select right mapping mode
                selectMappingModeCombobox.selectedIndex = IngeScapeEditorC.modelManager.isMappingControlled ? 0 : 1;
            }

            onChangeNetworkSettings: {
                IngeScapeEditorC.port = port;
                IngeScapeEditorC.networkDevice = networkDevice;

                // Apply mapping mode
                IngeScapeEditorC.modelManager.isMappingControlled = mappingForm.resMappingControlled;
                close();
            }
        }
    }


    //----------------------------------------------------------------------------------
    //
    // Mapping Modifications Popup
    //
    //----------------------------------------------------------------------------------
    Popups.MappingModificationsPopup {
        id: mappingModificationsPopup

        onCancelMappingActivation: {
            console.log("on Cancel Mapping Activation");

            // UN-activate the mapping
//            IgsModelManager.isMappingConnected = false;
        }

        onSwitchToControl: {
            console.log("on Switch To Control");

            if (IngeScapeEditorC.modelManager)
            {
                // Switch to CONTROL
                IngeScapeEditorC.modelManager.isMappingControlled = true;

                // Start our igs Editor agent
                IgsNetworkController.start(IngeScapeEditorC.networkDevice, IngeScapeEditorC.ipAddress, IngeScapeEditorC.port);
            }
        }

        onStayToObserve: {
            console.log("on Stay To Observe");
            if (IngeScapeEditorC.modelManager && controller)
            {
                // Switch to OBSERVE
                IngeScapeEditorC.modelManager.isMappingControlled = false;

                // Start our igs Editor agent
                IgsNetworkController.start(IngeScapeEditorC.networkDevice, IngeScapeEditorC.ipAddress, IngeScapeEditorC.port);
            }
        }
    }
}
