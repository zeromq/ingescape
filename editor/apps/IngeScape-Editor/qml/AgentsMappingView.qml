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
//import "theme" as Theme

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
                //visible: IngeScapeEditorC.modelManager.isMappingConnected

                anchors.fill: parent

                color: IngeScapeTheme.lightGreyColor

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

            style: LabellessSvgButtonStyle {
                fileCache: IngeScapeEditorTheme.svgFileIngeScapeEditor

                pressedID: releasedID + "-pressed"
                releasedID: IngeScapeEditorC.modelManager.isMappingConnected ? "connect-on" : "connect-off"
                disabledID : releasedID
            }

            onClicked: {
                IngeScapeEditorC.modelManager.isMappingConnected = checked;
            }

            Binding {
                target: activeMappingBtn
                property: "checked"
                value: IngeScapeEditorC.modelManager.isMappingConnected
            }
        }

        Rectangle {
            color: "transparent"
            width: childrenRect.width
            height: childrenRect.height

            anchors {
                bottom: parent.bottom
                left: parent.left
                leftMargin: 74 + (IngeScapeEditorC.modelManager.isMappingControlled ? 5 : 0)
                bottomMargin: 64 + (IngeScapeEditorC.modelManager.isMappingControlled ? 0 : 2)
            }

            Button {
                id: typeMappingBtn
                visible: true

                activeFocusOnPress: true
                checkable: true

                style: LabellessSvgButtonStyle {
                    fileCache: IngeScapeEditorTheme.svgFileIngeScapeEditor

                    pressedID: releasedID + "-pressed"
                    releasedID: IngeScapeEditorC.modelManager.isMappingControlled ? "control" : "observe"
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
                id: checkBoxObserve

                exclusiveGroup: typeMappingGroup
                //checked: !IngeScapeEditorC.modelManager.isMappingControlled

                style: CheckBoxStyle {
                    label: Text {
                        color: control.checked ? (control.pressed ? IngeScapeTheme.orangeColor : IngeScapeTheme.veryLightGreyColor)
                                               : (control.pressed ? IngeScapeTheme.orangeColor : IngeScapeTheme.greyColor2)

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
                        border.width: 0
                        color: "transparent"

                        I2SvgItem {
                            anchors.centerIn: parent
                            svgFileCache: IngeScapeTheme.svgFileINGESCAPE

                            svgElementId: control.checked ? (control.pressed ? "radio-observe-on" : "radio-observe-white")
                                                          : (control.pressed ? "radio-observe-on" : "radio-observe-off");
                        }
                    }
                }

                onClicked: {
                    if (checked) {
                        IngeScapeEditorC.modelManager.isMappingControlled = false;
                    }

                    // Show feedback button
                    typeMappingBtn.visible = true;

                    // Hide choose mapping type
                    chooseMappingType.visible = false;

                    closeAnimation.running = true;
                }

                Binding {
                    target: checkBoxObserve
                    property: "checked"
                    value: !IngeScapeEditorC.modelManager.isMappingControlled
                }
            }

            CheckBox {
                id: checkBoxControl

                exclusiveGroup: typeMappingGroup
                //checked: IngeScapeEditorC.modelManager.isMappingControlled

                style: CheckBoxStyle {
                    label: Text {
                        color: control.checked ? (control.pressed ? IngeScapeTheme.orangeColor : IngeScapeTheme.veryLightGreyColor)
                                               : (control.pressed ? IngeScapeTheme.orangeColor : IngeScapeTheme.greyColor2)

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
                        border.width: 0
                        color: "transparent"

                        I2SvgItem {
                            anchors.centerIn: parent
                            svgFileCache: IngeScapeTheme.svgFileINGESCAPE

                            svgElementId: control.checked ? (control.pressed ? "radio-control-on" : "radio-control-white")
                                                          : (control.pressed ? "radio-control-on" : "radio-control-off");
                        }
                    }
                }

                onClicked: {
                    if (checked) {
                        IngeScapeEditorC.modelManager.isMappingControlled = true;
                    }

                    // Show feedback button
                    typeMappingBtn.visible = true;

                    // Hide choose mapping type
                    chooseMappingType.visible = false;

                    closeAnimation.running = true;
                }

                Binding {
                    target: checkBoxControl
                    property: "checked"
                    value: IngeScapeEditorC.modelManager.isMappingControlled
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

                visible : rootItem.controller && rootItem.controller.isEmptyMapping && !IngeScapeEditorC.modelManager.isMappingConnected
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

                visible : rootItem.controller && rootItem.controller.isEmptyMapping && !IngeScapeEditorC.modelManager.isMappingConnected
                text : "Click on the button to synchronize with the current platform and get the active mapping.\nOr create/load a platform description and click on the button to apply it to the platform."

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
