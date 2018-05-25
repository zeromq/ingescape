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
 *      Justine Limoges    <limoges@ingenuity.io>
 *      Vincent Peyruqueou <peyruqueou@ingenuity.io>
 *
 */

import QtQuick 2.8
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4

import I2Quick 1.0

import INGESCAPE 1.0
import "../theme" as Theme


Item {
    id : root

    //--------------------------------
    //
    // Properties
    //
    //--------------------------------

    // Controller associated to our view
    property var controller : null;

    // my agent
    property var agent: null;

    // true if agent Item contains the mouse (rollover)
    property bool agentItemIsHovered : false

    width: IngeScapeTheme.leftPanelWidth
    height: 85


    // signal emitted when the delete confirmation popup is needed because the agent is already used in the platform
    signal needConfirmationtoDeleteAgent();

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

    Rectangle {
        anchors {
            fill: parent
        }

        border {
            width: 0
        }

        color: agentItemIsHovered ? IngeScapeTheme.agentsListItemRollOverBackgroundColor : IngeScapeTheme.agentsListItemBackgroundColor

        Rectangle {
            anchors {
                bottom: parent.bottom
                left: parent.left
                right: parent.right
            }

            color: IngeScapeTheme.leftPanelBackgroundColor

            height: 1
        }


        Item {
            id: agentRow

            anchors {
                fill: parent
            }


            // Selected Agent
            Item {
                anchors.fill: parent
                visible : controller && root.agent && (controller.selectedAgent === root.agent);

                Rectangle {
                    anchors {
                        left : parent.left
                        top : parent.top
                        bottom: parent.bottom
                    }

                    width : 6
                    color : IngeScapeTheme.selectedAgentColor
                }

                Button {
                    id: removeButton

                    visible : (model.isON === false)
                    activeFocusOnPress: true

                    anchors {
                        top: parent.top
                        topMargin: 10
                        right : parent.right
                        rightMargin: 12
                    }

                    style: Theme.LabellessSvgButtonStyle {
                        fileCache: IngeScapeTheme.svgFileINGESCAPE

                        pressedID: releasedID + "-pressed"
                        releasedID: "supprimer"
                        disabledID : releasedID
                    }

                    onClicked: {

                        if(IngeScapeEditorC.canDeleteAgentVMFromList(model.QtObject))
                        {
                            if (controller)
                            {
                                // Delete selected agent
                                controller.deleteSelectedAgent();
                            }
                        } else {
                            root.needConfirmationtoDeleteAgent();
                        }
                    }
                }
            }
        }

        Column {
            id : columnName

            anchors {
                left : parent.left
                leftMargin: 28
                top: parent.top
                topMargin: 12
                right : bottomRow.left
            }
            height : childrenRect.height

            spacing : 4

            // Name
            Text {
                id: agentName

                anchors {
                    left : parent.left
                }
                elide: Text.ElideRight

                text: root.agent ? root.agent.name : ""
                color: (root.agent && (root.agent.isON === true)) ? IngeScapeTheme.agentsListLabelColor : IngeScapeTheme.agentOFFLabelColor


                font: IngeScapeTheme.headingFont

                Text {
                    anchors
                    {
                        left:parent.right
                        leftMargin:5
                        baseline:parent.baseline
                    }

                    text:  model.state !== "" ? qsTr("(%1)").arg(model.state): ""

                    color : IngeScapeTheme.whiteColor
                    font {
                        family: IngeScapeTheme.labelFontFamily
                        pixelSize : 12
                        italic: true
                    }
                }

                // clones
                Rectangle {
                    height : 16
                    width : height
                    radius : height/2

                    visible: (model.clonesNumber > 0)

                    anchors {
                        verticalCenter: parent.verticalCenter
                        verticalCenterOffset: 1
                        right : parent.left
                        rightMargin: 5
                    }

                    color : IngeScapeTheme.redColor

                    Text {
                        anchors.centerIn : parent
                        text: model.clonesNumber

                        color : IngeScapeTheme.whiteColor
                        font {
                            family: IngeScapeTheme.labelFontFamily
                            weight : Font.Black
                            pixelSize : 12
                        }
                    }


                }
            }

            // Definition name and version
            MouseArea {
                id : definitionNameBtn

                anchors {
                    left : parent.left
                }

                height : definitionNameTxt.height
                width : childrenRect.width

                hoverEnabled: true
                onClicked: {
                    if (controller && root.agent) {
                        // Open the definition of our agent
                        controller.openDefinition(root.agent);
                    }
                }

                TextMetrics {
                    id : definitionName

                    elideWidth: (columnName.width - versionName.width)
                    elide: Text.ElideRight

                    text: root.agent && root.agent.definition ? root.agent.definition.name : ""
                }

                Text {
                    id : definitionNameTxt

                    anchors {
                        left : parent.left
                    }

                    text : definitionName.elidedText
                    color: if (root.agent && (root.agent.isON === true)) {
                               ((model.definition && model.definition.isVariant) ?
                                    definitionNameBtn.pressed? IngeScapeTheme.middleDarkRedColor : IngeScapeTheme.redColor
                                : definitionNameBtn.pressed? IngeScapeTheme.agentsListPressedLabel2Color : IngeScapeTheme.agentsListLabel2Color)
                           }
                           else {
                               ((model.definition && model.definition.isVariant) ?
                                    definitionNameBtn.pressed? IngeScapeTheme.darkRedColor : IngeScapeTheme.middleDarkRedColor
                                : definitionNameBtn.pressed? IngeScapeTheme.agentOFFPressedLabel2Color : IngeScapeTheme.agentOFFLabel2Color)
                           }
                    font: IngeScapeTheme.heading2Font
                }

                Text {
                    id : versionName
                    anchors {
                        bottom: definitionNameTxt.bottom
                        bottomMargin : 2
                        left : definitionNameTxt.right
                        leftMargin: 5
                    }

                    text: root.agent && root.agent.definition ? "(v" + root.agent.definition.version + ")" : ""
                    color: definitionNameTxt.color

                    font {
                        family: IngeScapeTheme.textFontFamily
                        pixelSize : 10
                        italic : true
                    }
                }

                // underline
                Rectangle {
                    visible: definitionNameBtn.containsMouse

                    anchors {
                        left : definitionNameTxt.left
                        right : versionName.right
                        bottom : parent.bottom
                    }

                    height : 1

                    color : definitionNameTxt.color
                }
            }

            // Hostnames (on the network) of our agents
            Text {
                id: agentHostnames
                anchors {
                    left : parent.left
                    right : parent.right
                }
                elide: Text.ElideRight

                text: root.agent ? root.agent.hostnames: ""

                color: (root.agent && (root.agent.isON === true)) ? IngeScapeTheme.agentsListTextColor : IngeScapeTheme.agentOFFTextColor
                font: IngeScapeTheme.normalFont
            }

        }


        // Button ON/OFF
        Button {
            id: btnOnOff

            // Agent is "ON" OR Agent can be restarted
            visible: (root.agent && (root.agent.isON || root.agent.canBeRestarted))

            activeFocusOnPress: true
            enabled: visible

            anchors {
                bottom: bottomRow.top
                bottomMargin: 5
                right : parent.right
                rightMargin: 12
            }

            style: Theme.LabellessSvgButtonStyle {
                fileCache: IngeScapeTheme.svgFileINGESCAPE

                pressedID: releasedID + "-pressed"
                releasedID: model.isON? "on" : "off"
                disabledID : releasedID
            }

            onClicked: {
                model.QtObject.changeState();
            }
        }


        Row {
            id: bottomRow

            anchors {
                bottom: parent.bottom
                bottomMargin: 10
                right : parent.right
                rightMargin: 12
            }

            spacing: 5


            // Button Freeze
            Button {
                id: freezeButton

                visible: model.canBeFrozen && (model.isON === true)
                enabled : visible
                activeFocusOnPress: true

                style: Theme.LabellessSvgButtonStyle {
                    fileCache: IngeScapeTheme.svgFileINGESCAPE

                    pressedID: releasedID + "-pressed"
                    releasedID: model.isFrozen? "freezeactif" : "freezeinactif"
                    disabledID : releasedID
                }

                onClicked: {
                    model.QtObject.changeFreeze();
                }
            }


            // Button Mute
            Button {
                id: muteButton

                visible: (model.isON === true)
                activeFocusOnPress: true

                style: Theme.LabellessSvgButtonStyle {
                    fileCache: IngeScapeTheme.svgFileINGESCAPE

                    pressedID: releasedID + "-pressed"
                    releasedID: model.isMuted? "muteactif" : "muteinactif"
                    disabledID : releasedID
                }

                onClicked: {
                    model.QtObject.changeMuteAllOutputs();
                }
            }


            // Button Options
            Button {
                id: btnOptions

                visible: (model.isON === true)
                activeFocusOnPress: true

                style: Theme.LabellessSvgButtonStyle {
                    fileCache: IngeScapeTheme.svgFileINGESCAPE

                    pressedID: "empty-button"
                    releasedID: "empty-button"
                    disabledID : releasedID
                }

                Text {
                    text: "..."

                    anchors {
                        left: parent.left
                        right: parent.right
                        bottom: parent.bottom
                        bottomMargin: 5
                    }
                    horizontalAlignment: Text.AlignHCenter

                    color: btnOptions.pressed ? "gray" : "black"

                    font {
                        family: IngeScapeTheme.labelFontFamily
                        pixelSize: 20
                        //bold: true
                    }
                }

                onClicked: {
                    console.log("Open options...");
                    popupOptions.open();
                }
            }
        }

        I2PopupBase {
            id : popupOptions
            anchors {
                top: bottomRow.top
                left: bottomRow.right
            }

            width: 220
            height: 300

            isModal: true;
            layerColor: "transparent"
            dismissOnOutsideTap : true;

            keepRelativePositionToInitialParent : true;

            onClosed: {

            }
            onOpened: {

            }

            Rectangle {
                id: popUpBackground
                anchors {
                    fill: parent
                }
                color: "black"

                Column {
                    anchors {
                        fill: parent
                        margins: 10
                    }

                    spacing: 4

                    Button {
                        id: optionLoadDefinition

                        text: qsTr("Load Definition")

                        onClicked: {
                            if (root.agent) {
                                root.agent.loadDefinition();

                                popupOptions.close();
                            }
                        }
                    }

                    Button {
                        id: optionLoadMapping

                        text: qsTr("Load Mapping")

                        onClicked: {
                            if (root.agent) {
                                root.agent.loadMapping();

                                popupOptions.close();
                            }
                        }
                    }

                    Button {
                        id: optionDownloadDefinition

                        text: qsTr("Download Definition")

                        onClicked: {
                            if (root.agent) {
                                root.agent.downloadDefinition();

                                popupOptions.close();
                            }
                        }
                    }

                    Button {
                        id: optionDownloadMapping

                        text: qsTr("Download Mapping")

                        onClicked: {
                            if (root.agent) {
                                root.agent.downloadMapping();

                                popupOptions.close();
                            }
                        }
                    }

                    Button {
                        id: optionSetPath

                        text: qsTr("Set Path for Def./Mapping")

                        onClicked: {
                            console.log("Set Path for Definition/Mapping");
                        }
                    }

                    Button {
                        id: optionSaveToPath

                        text: qsTr("Save Def./Mapping to Path")

                        onClicked: {
                            console.log("Save Definition/Mapping to Path");
                        }
                    }

                    Button {
                        id: optionLogFile

                        text: qsTr("Enable/Disable Log File")

                        onClicked: {
                            console.log("Enable/Disable Log File");
                        }
                    }

                    Button {
                        id: optionLogStream

                        text: qsTr("Enable/Disable Log Stream")

                        onClicked: {
                            console.log("Enable/Disable Log Stream");
                        }
                    }
                }
            }
        }


    }
}


