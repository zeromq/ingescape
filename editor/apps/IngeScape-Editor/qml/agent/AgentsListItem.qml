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
import "../popup" as PopUp


Item {
    id: rootItem

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

    // signal emitted when the user clicks on the option "Set Path for Definition/Mapping"
    signal configureFilesPaths(var agent);


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
                visible : controller && rootItem.agent && (controller.selectedAgent === rootItem.agent);

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

                    visible: (model.isON === false)
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

                        if (IngeScapeEditorC.canDeleteAgentFromSupervision(model.name))
                        {
                            if (controller)
                            {
                                // Delete selected agent
                                controller.deleteSelectedAgent();
                            }
                        }
                        else {
                            // Emit the signal "needConfirmationtoDeleteAgent"
                            rootItem.needConfirmationtoDeleteAgent();
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

                text: rootItem.agent ? rootItem.agent.name : ""
                color: (rootItem.agent && (rootItem.agent.isON === true)) ? IngeScapeTheme.agentsListLabelColor : IngeScapeTheme.agentOFFLabelColor


                font: IngeScapeTheme.headingFont

                Text {
                    anchors
                    {
                        left: parent.right
                        leftMargin: 5
                        baseline: parent.baseline
                    }

                    text: (model.state !== "") ? qsTr("(%1)").arg(model.state) : ""

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
                    if (IngeScapeEditorC.modelManager && rootItem.agent && rootItem.agent.definition)
                    {
                        IngeScapeEditorC.modelManager.openDefinition(rootItem.agent.definition);
                    }
                }

                TextMetrics {
                    id : definitionName

                    elideWidth: (columnName.width - versionName.width)
                    elide: Text.ElideRight

                    text: (rootItem.agent && rootItem.agent.definition) ? rootItem.agent.definition.name
                                                                        : ""
                }

                Text {
                    id : definitionNameTxt

                    anchors {
                        left : parent.left
                    }

                    text : definitionName.elidedText

                    color: if (rootItem.agent)
                           {
                               // ON
                               if (rootItem.agent.isON === true)
                               {
                                   // ON & Variant
                                   if (rootItem.agent.definition && rootItem.agent.definition.isVariant)
                                   {
                                       if (definitionNameBtn.pressed) {
                                           IngeScapeTheme.middleDarkRedColor
                                       }
                                       else {
                                           IngeScapeTheme.redColor
                                       }
                                   }
                                   // ON & (NO variant)
                                   else
                                   {
                                       if (definitionNameBtn.pressed) {
                                           IngeScapeTheme.agentsListPressedLabel2Color
                                       }
                                       else {
                                           IngeScapeTheme.agentsListLabel2Color
                                       }
                                   }
                               }
                               // OFF
                               else
                               {
                                   // OFF & Variant
                                   if (rootItem.agent.definition && rootItem.agent.definition.isVariant)
                                   {
                                       if (definitionNameBtn.pressed) {
                                           IngeScapeTheme.darkRedColor
                                       }
                                       else {
                                           IngeScapeTheme.middleDarkRedColor
                                       }
                                   }
                                   // OFF & (NO variant)
                                   else
                                   {
                                       if (definitionNameBtn.pressed) {
                                           IngeScapeTheme.agentOFFPressedLabel2Color
                                       }
                                       else {
                                           IngeScapeTheme.agentOFFLabel2Color
                                       }
                                   }
                               }
                           }
                           else {
                               IngeScapeTheme.agentOFFLabel2Color
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

                    text: (rootItem.agent && rootItem.agent.definition) ? "(v" + rootItem.agent.definition.version + ")"
                                                                        : ""

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

                text: rootItem.agent ? rootItem.agent.hostnames: ""

                color: (rootItem.agent && (rootItem.agent.isON === true)) ? IngeScapeTheme.agentsListTextColor : IngeScapeTheme.agentOFFTextColor
                font: IngeScapeTheme.normalFont
            }

        }

        Row {
            id: middleRow

            anchors {
                bottom: parent.bottom
                bottomMargin: 35
                right : parent.right
                rightMargin: 10
            }

            spacing: 5

            // Button Mute
            Button {
                id: muteButton

                visible: (model.isON === true)
                activeFocusOnPress: true

                style: Theme.LabellessSvgButtonStyle {
                    fileCache: IngeScapeTheme.svgFileINGESCAPE

                    pressedID: releasedID + "-pressed"
                    releasedID: model.isMuted ? "muteactif" : "muteinactif"
                    disabledID : releasedID
                }

                onClicked: {
                    model.QtObject.changeMuteAllOutputs();
                }
            }

            // Button ON/OFF
            Button {
                id: btnOnOff

                // Agent is "ON" OR Agent can be restarted
                visible: (rootItem.agent && (rootItem.agent.isON || rootItem.agent.canBeRestarted))

                activeFocusOnPress: true
                enabled: visible

                style: Theme.LabellessSvgButtonStyle {
                    fileCache: IngeScapeTheme.svgFileINGESCAPE

                    pressedID: releasedID + "-pressed"
                    releasedID: model.isON ? "on" : "off"
                    disabledID : releasedID
                }

                onClicked: {
                    model.QtObject.changeState();
                }
            }
        }


        Row {
            id: bottomRow

            anchors {
                bottom: parent.bottom
                bottomMargin: 10
                right : parent.right
                rightMargin: 10
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
                    releasedID: model.isFrozen ? "freezeactif" : "freezeinactif"
                    disabledID : releasedID
                }

                onClicked: {
                    model.QtObject.changeFreeze();
                }
            }


            // Button Options
            Button {
                id: btnOptions

                visible: (model.isON === true)
                activeFocusOnPress: true

                style: Theme.LabellessSvgButtonStyle {
                    fileCache: IngeScapeTheme.svgFileINGESCAPE

                    pressedID: releasedID + "-pressed"
                    releasedID: "button-options"
                    disabledID : releasedID
                }

                onClicked: {
                    //console.log("QML: Open options...");

                    // Open the popup with options
                    popupOptions.openInScreen();
                }
            }
        }
    }

    PopUp.MenuPopup {
        id : popupOptions

        anchors {
            top: rootItem.top
            left: rootItem.right
            leftMargin: 2
        }

        readonly property int optionHeight: 30

        // Get height from children
        height: popUpBackground.y + popUpBackground.height
        width: 230

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
            height: buttons.y + buttons.height
            anchors {
                right: parent.right
                left: parent.left
            }
            color: IngeScapeTheme.veryDarkGreyColor
            radius: 5
            border {
                color: IngeScapeTheme.blueGreyColor2
                width: 1
            }

            Column {
                id: buttons
                anchors {
                    right: parent.right
                    left: parent.left
                }

                Button {
                    id: optionLoadDefinition

                    height: popupOptions.optionHeight
                    width: parent.width

                    text: qsTr("Upload Definition...")

                    style: Theme.ButtonStyleOfOption {

                    }

                    onClicked: {
                        if (rootItem.agent) {
                            rootItem.agent.loadDefinition();

                            popupOptions.close();
                        }
                    }
                }

                Button {
                    id: optionLoadMapping

                    height: popupOptions.optionHeight
                    width: parent.width

                    text: qsTr("Upload Mapping...")

                    style: Theme.ButtonStyleOfOption {

                    }

                    onClicked: {
                        if (rootItem.agent) {
                            rootItem.agent.loadMapping();

                            popupOptions.close();
                        }
                    }
                }

                Button {
                    id: optionDownloadDefinition

                    height: popupOptions.optionHeight
                    width: parent.width

                    text: qsTr("Download Definition...")

                    style: Theme.ButtonStyleOfOption {

                    }

                    onClicked: {
                        if (rootItem.agent) {
                            rootItem.agent.downloadDefinition();

                            popupOptions.close();
                        }
                    }
                }

                Button {
                    id: optionDownloadMapping

                    height: popupOptions.optionHeight
                    width: parent.width

                    text: qsTr("Download Mapping...")

                    style: Theme.ButtonStyleOfOption {

                    }

                    onClicked: {
                        if (rootItem.agent) {
                            rootItem.agent.downloadMapping();

                            popupOptions.close();
                        }
                    }
                }

                Button {
                    id: optionSetPath

                    height: popupOptions.optionHeight
                    width: parent.width

                    text: qsTr("Set local paths...")

                    style: Theme.ButtonStyleOfOption {

                    }

                    onClicked: {
                        console.log("QML: Set Path for Definition/Mapping/Log files");

                        // Emit the signal "configureFilesPaths"
                        rootItem.configureFilesPaths(rootItem.agent);

                        popupOptions.close();
                    }
                }

                Button {
                    id: optionSaveDefinitionToPath

                    height: popupOptions.optionHeight
                    width: parent.width

                    text: qsTr("Save Definition to local path")

                    style: Theme.ButtonStyleOfOption {

                    }

                    onClicked: {
                        //console.log("QML: Save Definition to Path");

                        rootItem.agent.saveDefinitionToPath();

                        popupOptions.close();
                    }
                }

                Button {
                    id: optionSaveMappingToPath

                    height: popupOptions.optionHeight
                    width: parent.width

                    text: qsTr("Save Mapping to local path")

                    style: Theme.ButtonStyleOfOption {

                    }

                    onClicked: {
                        //console.log("QML: Save Mapping to Path");

                        rootItem.agent.saveMappingToPath();

                        popupOptions.close();
                    }
                }

                Button {
                    id: optionLogFile

                    height: popupOptions.optionHeight
                    width: parent.width

                    text: (rootItem.agent && (rootItem.agent.hasLogInFile === true)) ? qsTr("Disable log file") : qsTr("Enable log file")

                    style: Theme.ButtonStyleOfOption {

                    }

                    onClicked: {
                        if (rootItem.agent) {
                            rootItem.agent.changeLogInFile();
                        }

                        popupOptions.close();
                    }
                }

                /*Button {
                    id: optionLogStream

                    height: popupOptions.optionHeight
                    width: parent.width

                    text: (rootItem.agent && (rootItem.agent.hasLogInStream === true)) ? qsTr("Disable Log Stream") : qsTr("Enable Log Stream")

                    style: Theme.ButtonStyleOfOption {

                    }

                    onClicked: {
                        if (rootItem.agent) {
                            rootItem.agent.changeLogInStream();
                        }

                        popupOptions.close();
                    }
                }*/

                Button {
                    id: optionViewLogStream

                    enabled: (rootItem.agent ? rootItem.agent.isEnabledViewLogStream : false)

                    height: popupOptions.optionHeight
                    width: parent.width

                    text: qsTr("Open log stream")

                    style: Theme.ButtonStyleOfOption {
                        isVisibleSeparation: false
                    }

                    onClicked: {
                        //console.log("QML: View Log Stream");

                        rootItem.agent.openLogStream();

                        popupOptions.close();
                    }
                }
            }
        }
    }
}


