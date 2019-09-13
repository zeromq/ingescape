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

    // Agents Grouped by Definition
    //property AgentsGroupedByDefinitionVM agentsGroupedByDefinition: null;
    property var agent: null;

    // true if agent Item contains the mouse (rollover)
    property bool agentItemIsHovered : false

    // Licenses controller
    property LicensesController licensesController: IngeScapeEditorC.licensesC;

    // Flag indicating if the user have a valid license for the editor
    property bool isEditorLicenseValid: rootItem.licensesController && rootItem.licensesController.mergedLicense && rootItem.licensesController.mergedLicense.editorLicenseValidity

    width: IngeScapeEditorTheme.leftPanelWidth
    height: 85


    //--------------------------------
    //
    // Signals
    //
    //--------------------------------

    // signal emitted when the delete confirmation popup is needed because the agent is already used in the platform
    signal needConfirmationToDeleteAgentInList();

    // signal emitted when the user clicks on the option "Set Path for Definition/Mapping"
    signal configureFilesPaths(var agent);

    // Signal emitted when the user tries to perform an action forbidden by the license
    signal unlicensedAction();


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

        border.width: 0

        color: agentItemIsHovered ? IngeScapeEditorTheme.agentsListItemRollOverBackgroundColor : IngeScapeEditorTheme.agentsListItemBackgroundColor

        Rectangle {
            anchors {
                bottom: parent.bottom
                left: parent.left
                right: parent.right
            }

            color: IngeScapeTheme.blackColor

            height: 1
        }


        // Selection feedback
        Rectangle {
            id: selectionFeedback

            anchors {
                left : parent.left
                top : parent.top
                bottom: parent.bottom
            }
            width : 6

            visible: controller && rootItem.agent && (controller.selectedAgent === rootItem.agent)

            color: IngeScapeTheme.selectionColor
        }

        // Remove button
        LabellessSvgButton {
            id: removeButton

            anchors {
                top: parent.top
                topMargin: 10
                right: parent.right
                rightMargin: 10
            }

            visible: (model.isON === false)
            opacity: agentItemIsHovered ? 1 : 0

            releasedID: "delete"
            pressedID: releasedID + "-pressed"
            disabledID : releasedID


            onClicked: {
                if (IngeScapeEditorC.isAgentUsedInPlatform(rootItem.agent))
                {
                    // Emit the signal "Need Confirmation to Delete Agent in List"
                    rootItem.needConfirmationToDeleteAgentInList();
                }
                else
                {
                    if (controller)
                    {
                        // Delete our agent
                        controller.deleteAgentInList(rootItem.agent);
                    }
                }
            }
        }


        Column {
            id: columnName

            anchors {
                left: parent.left
                leftMargin: 28
                top: parent.top
                topMargin: 12
                right: middleRow.left
                //rightMargin: 5
            }
            height: childrenRect.height

            spacing: 4

            TextMetrics {
                id: textMetricsName

                elideWidth: (columnName.width - textState.width - textState.anchors.leftMargin)
                elide: Text.ElideRight

                text: rootItem.agent ? rootItem.agent.name : ""
                font: IngeScapeTheme.headingFont
            }

            // Name
            Text {
                id: agentName

                anchors {
                    left: parent.left
                    //right: parent.right
                }

                text: textMetricsName.elidedText

                color: (rootItem.agent && (rootItem.agent.isON === true)) ? IngeScapeEditorTheme.agentsListLabelColor : IngeScapeEditorTheme.agentOFFLabelColor

                font: IngeScapeTheme.headingFont

                Text {
                    id: textState

                    anchors {
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
                    height: 16
                    width: height
                    radius: height/2

                    visible: (model.clonesNumber > 0)

                    anchors {
                        verticalCenter: parent.verticalCenter
                        verticalCenterOffset: 1
                        right: parent.left
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
                id: definitionNameBtn

                height: definitionNameTxt.height
                width: childrenRect.width

                hoverEnabled: true
                onClicked: {
                    if (IngeScapeEditorC.modelManager && rootItem.agent && rootItem.agent.definition)
                    {
                        IngeScapeEditorC.modelManager.openDefinition(rootItem.agent.definition);
                    }
                }

                TextMetrics {
                    id: textMetricsDefinition

                    elideWidth: (columnName.width - versionName.width - versionName.anchors.leftMargin)
                    elide: Text.ElideRight

                    text: (rootItem.agent && rootItem.agent.definition) ? rootItem.agent.definition.name : ""
                    font: IngeScapeTheme.heading2Font
                }

                Text {
                    id : definitionNameTxt

                    anchors {
                        left : parent.left
                    }

                    text: textMetricsDefinition.elidedText

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
                                           IngeScapeEditorTheme.agentsListPressedLabel2Color
                                       }
                                       else {
                                           IngeScapeEditorTheme.agentsListLabel2Color
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
                                           IngeScapeEditorTheme.agentOFFPressedLabel2Color
                                       }
                                       else {
                                           IngeScapeEditorTheme.agentOFFLabel2Color
                                       }
                                   }
                               }
                           }
                           else {
                               IngeScapeEditorTheme.agentOFFLabel2Color
                           }

                    font: IngeScapeTheme.heading2Font
                }

                Text {
                    id : versionName
                    anchors {
                        bottom: definitionNameTxt.bottom
                        bottomMargin: 2
                        left: definitionNameTxt.right
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

                // Underline
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

                color: (rootItem.agent && (rootItem.agent.isON === true)) ? IngeScapeEditorTheme.agentsListTextColor : IngeScapeEditorTheme.agentOFFTextColor
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
            width: childrenRect.width

            spacing: 5

            // Button Mute
            LabellessSvgButton {
                id: muteButton

                visible: model.isON

                fileCache: IngeScapeEditorTheme.svgFileIngeScapeEditor

                pressedID: releasedID + "-pressed"
                releasedID: model.isMuted ? "active-mute" : "inactive-mute"
                disabledID : releasedID


                onClicked: {
                    if (rootItem.isEditorLicenseValid)
                    {
                        model.QtObject.changeMuteAllOutputs();
                    }
                    else
                    {
                        rootItem.unlicensedAction();
                    }
                }
            }

            // Button ON/OFF
            LabellessSvgButton {
                id: btnOnOff

                // Agent is "ON" OR Agent can be restarted
                visible: (rootItem.agent && (rootItem.agent.isON || rootItem.agent.canBeRestarted))
                enabled: visible

                fileCache: IngeScapeEditorTheme.svgFileIngeScapeEditor

                pressedID: releasedID + "-pressed"
                releasedID: model.isON ? "on" : "off"
                disabledID : releasedID


                onClicked: {
                    if (rootItem.isEditorLicenseValid)
                    {
                        model.QtObject.changeState();
                    }
                    else
                    {
                        rootItem.unlicensedAction();
                    }
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
            width: childrenRect.width

            spacing: 5

            // Button Freeze
            LabellessSvgButton {
                id: freezeButton

                visible: model.canBeFrozen && model.isON
                enabled : visible

                fileCache: IngeScapeEditorTheme.svgFileIngeScapeEditor

                pressedID: releasedID + "-pressed"
                releasedID: model.isFrozen ? "active-freeze" : "inactive-freeze"
                disabledID : releasedID


                onClicked: {
                    if (rootItem.isEditorLicenseValid)
                    {
                        model.QtObject.changeFreeze();
                    }
                    else
                    {
                        rootItem.unlicensedAction();
                    }
                }
            }

            // Options button
            LabellessSvgButton {
                id: btnOptions

                visible: model.isON


                releasedID: "button-options"
                pressedID: releasedID + "-pressed"
                disabledID : releasedID

                onClicked: {
                    // Open the popup with options
                    if (rootItem.isEditorLicenseValid)
                    {
                        popupOptions.openInScreen();
                    }
                    else
                    {
                        rootItem.unlicensedAction();
                    }
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

                    enabled: (rootItem.agent && rootItem.agent.definition) ? true : false

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


