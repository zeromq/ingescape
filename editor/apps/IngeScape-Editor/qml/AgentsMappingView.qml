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

import I2Quick 1.0

import INGESCAPE 1.0

// agentsmapping sub-directory
import "agentsmapping" as AgentsMapping
import "theme" as Theme

Item {
    id: rootItem

    anchors.fill: parent

    //--------------------------------
    //
    // Properties
    //
    //--------------------------------

    // Controller associated to our view
    property var controller : null;


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


        //
        // Node graph view
        //
        AgentsMapping.NodeGraphView {
            id: nodeGraphView

            anchors.fill: parent

            controller: rootItem.controller
        }


        //
        // Mapping Activation
        //
        Item {
            id: arcBehindMappingActivationBtn

            anchors {
                bottom: parent.bottom
                left: parent.left
                leftMargin: 10
                bottomMargin: 10
            }

            width: 80
            height: 80

            I2PieSlice {
                id: pieSlice
                anchors.fill: parent

                color: "white"

                innerRadiusX: 39
                innerRadiusY: 39

                startAngle: -10
                sweepAngle: 315
            }
        }

        ParallelAnimation {
            id: openAnimation
            running: false
            NumberAnimation { target: pieSlice; property: "startAngle"; from:-10; to: 0; duration: 300 }
            NumberAnimation { target: pieSlice; property: "sweepAngle"; from: 315; to: 295; duration: 300 }
            NumberAnimation { target: chooseMappingType; property: "opacity"; from: 0; to: 1; duration: 500 }
        }
        ParallelAnimation {
            id: closeAnimation
            running: false
            NumberAnimation { target: pieSlice; property: "startAngle"; from:0; to: -10; duration: 300 }
            NumberAnimation { target: pieSlice; property: "sweepAngle"; from: 295; to: 315; duration: 300 }
            NumberAnimation { target: chooseMappingType; property: "opacity"; from: 1; to: 0; duration: 500 }
        }

        Button {
            id: activeMappingBtn

            anchors {
                bottom: parent.bottom
                left: parent.left
                leftMargin: 23
                bottomMargin: 23
            }

            activeFocusOnPress: true
            checkable: true

            style: Theme.LabellessSvgButtonStyle {
                fileCache: IngeScapeTheme.svgFileINGESCAPE

                pressedID: releasedID + "-pressed"
                releasedID: IngeScapeEditorC.modelManager.isActivatedMapping ? "connect-on" : "connect-off"
                disabledID : releasedID
            }

            onClicked: {
                IngeScapeEditorC.modelManager.isActivatedMapping = checked;
            }

            Binding {
                target : activeMappingBtn
                property : "checked"
                value : IngeScapeEditorC.modelManager.isActivatedMapping
            }
        }

        Button {
            id: typeMappingBtn
            visible: true

            anchors {
                bottom: parent.bottom
                left: parent.left
                leftMargin: 74 + (IngeScapeEditorC.modelManager.isControlledMapping ? 5 : 0)
                bottomMargin: 64 + (IngeScapeEditorC.modelManager.isControlledMapping ? 0 : 2)
            }

            activeFocusOnPress: true
            checkable: true

            style: Theme.LabellessSvgButtonStyle {
                fileCache: IngeScapeTheme.svgFileINGESCAPE

                pressedID: releasedID + "-pressed"
                releasedID: IngeScapeEditorC.modelManager.isControlledMapping ? "control" : "observe"
                disabledID : releasedID
            }

            onClicked: {
                openAnimation.running = true;

                // Show choose mapping type
                chooseMappingType.visible = true;

                // Hide our feedback button
                typeMappingBtn.visible = false;
            }
        }

        Column {
            id: chooseMappingType
            visible: false
            opacity: 0

            spacing: 5

            anchors {
                bottom: parent.bottom
                left: parent.left
                leftMargin: 80
                bottomMargin: 60
            }

            ExclusiveGroup {
                id: typeMappingGroup
            }

            CheckBox {
                exclusiveGroup: typeMappingGroup

                checked: !IngeScapeEditorC.modelManager.isControlledMapping

                style: CheckBoxStyle {
                    label: Text {
                        color: control.checked ? IngeScapeTheme.veryLightGreyColor : IngeScapeTheme.greyColor2

                        text: qsTr("OBSERVE")

                        anchors {
                            left: parent.left
                            leftMargin: 5
                        }

                        font {
                            family: IngeScapeTheme.textFontFamily
                            weight: Font.Bold
                            pixelSize: 12
                        }
                    }

                    indicator: Rectangle {
                        implicitWidth: 14
                        implicitHeight: 14
                        border.width: 0;
                        color: "transparent"

                        I2SvgItem {
                            anchors.centerIn: parent;
                            svgFileCache: IngeScapeTheme.svgFileINGESCAPE
                            svgElementId: control.checked ? "radio_observe_on" : "radio_observe_off"
                        }
                    }
                }

                onClicked: {
                    if (checked) {
                        IngeScapeEditorC.modelManager.isControlledMapping = false;
                    }

                    // Show feedback button
                    typeMappingBtn.visible = true;

                    // Hide choose mapping type
                    chooseMappingType.visible = false;

                    closeAnimation.running = true;
                }
            }

            CheckBox {
                exclusiveGroup: typeMappingGroup

                checked: IngeScapeEditorC.modelManager.isControlledMapping

                style: CheckBoxStyle {
                    label: Text {
                        color: control.checked ? IngeScapeTheme.veryLightGreyColor : IngeScapeTheme.greyColor2

                        text: qsTr("CONTROL")

                        anchors {
                            left: parent.left
                            leftMargin: 5
                        }

                        font {
                            family: IngeScapeTheme.textFontFamily
                            weight: Font.Bold
                            pixelSize: 12
                        }
                    }

                    indicator: Rectangle {
                        implicitWidth: 14
                        implicitHeight: 14
                        border.width: 0;
                        color: "transparent"

                        I2SvgItem {
                            anchors.centerIn: parent;
                            svgFileCache: IngeScapeTheme.svgFileINGESCAPE
                            svgElementId: control.checked ? "radio_control_on" : "radio_control_off"
                        }
                    }
                }

                onClicked: {
                    if (checked) {
                        IngeScapeEditorC.modelManager.isControlledMapping = true;
                    }

                    // Show feedback button
                    typeMappingBtn.visible = true;

                    // Hide choose mapping type
                    chooseMappingType.visible = false;

                    closeAnimation.running = true;
                }
            }
        }


        Column {
            anchors {
                verticalCenter: activeMappingBtn.verticalCenter
                left : activeMappingBtn.right
                leftMargin: 75
            }

            Text {
                anchors {
                    left : parent.left
                }

                visible : rootItem.controller && rootItem.controller.isEmptyMapping && !IngeScapeEditorC.modelManager.isActivatedMapping
                text : "No active mapping at the moment."

                color : IngeScapeTheme.blueGreyColor
                font {
                    family : IngeScapeTheme.textFontFamily
                    bold : true
                    pixelSize: 20

                }
            }

            Text {
                anchors {
                    left : parent.left
                }

                visible : rootItem.controller && rootItem.controller.isEmptyMapping && !IngeScapeEditorC.modelManager.isActivatedMapping
                text : "Click on the button to synchronize with the environement and get the currently active mapping.\nOr create/load a mapping and click on the button to apply it to the environment."

                color : IngeScapeTheme.blueGreyColor
                font {
                    family : IngeScapeTheme.textFontFamily
                    weight : Font.Light
                    pixelSize: 17
                }
            }
        }
    }
}
