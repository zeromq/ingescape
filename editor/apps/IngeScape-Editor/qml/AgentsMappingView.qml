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
                anchors.fill: parent

                color: "white"

                innerRadiusX: 39
                innerRadiusY: 39

                startAngle: -10
                sweepAngle: 310
            }
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
                leftMargin: 78
                bottomMargin: 70
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
                // Show choose mapping type
                chooseMappingType.visible = true;

                // Hide our feedback button
                typeMappingBtn.visible = false;
            }
        }

        Column {
            id: chooseMappingType
            visible: false

            anchors {
                bottom: parent.bottom
                left: parent.left
                leftMargin: 78
                bottomMargin: 64
            }

            ExclusiveGroup {
                id: typeMappingGroup
            }
            RadioButton {
                text: "OBSERVE"
                exclusiveGroup: typeMappingGroup
                checked: !IngeScapeEditorC.modelManager.isControlledMapping

                onCheckedChanged: {
                    if (checked) {
                        IngeScapeEditorC.modelManager.isControlledMapping = false;

                        // Show feedback button
                        typeMappingBtn.visible = true;

                        // Hide choose mapping type
                        chooseMappingType.visible = false;
                    }
                }
            }
            RadioButton {
                text: "CONTROL"
                exclusiveGroup: typeMappingGroup
                checked: IngeScapeEditorC.modelManager.isControlledMapping

                onCheckedChanged: {
                    if (checked) {
                        IngeScapeEditorC.modelManager.isControlledMapping = true;

                        // Show feedback button
                        typeMappingBtn.visible = true;

                        // Hide choose mapping type
                        chooseMappingType.visible = false;
                    }
                }
            }
        }


        Column {
            anchors {
                verticalCenter: activeMappingBtn.verticalCenter
                left : activeMappingBtn.right
                leftMargin: 15
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
