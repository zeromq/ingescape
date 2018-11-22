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
import QtQuick.Window 2.3

import INGESCAPE 1.0


// agent sub-directory
import "agent" as Agent

// host sub-directory
import "host" as Host

// record sub-directory
import "record" as Record

// scenario sub-directory
import "scenario" as Scenario


Item {
    id: rootItem

    anchors.fill: parent


    //--------------------------------------------------------
    //
    //
    // Properties
    //
    //
    //--------------------------------------------------------



    //--------------------------------------------------------
    //
    //
    // Functions
    //
    //
    //--------------------------------------------------------

    //
    // function allowing to open the history panel
    //
    function openHistory() {
        //console.log("QML: function openHistory()");

        historyPanel.show();

        // Raises the window in the windowing system
        historyPanel.raise();
    }


    //
    // function allowing to open the network configuration popup
    //
    function openNetworkConfiguration() {
        if (IngeScapeEditorC.networkC) {
            IngeScapeEditorC.networkC.updateAvailableNetworkDevices();
        }

        networkConfigurationPopup.open();
    }


    // When the QML is loaded, we check the value of the error message when a connection attempt fails
    Component.onCompleted: {
        if (IngeScapeEditorC.errorMessageWhenConnectionFailed !== "")
        {
            //console.error("On Completed: Error Message = " + IngeScapeEditorC.errorMessageWhenConnectionFailed);
            networkConfigurationPopup.open();
        }
    }



    //--------------------------------------------------------
    //
    //
    // Content
    //
    //
    //--------------------------------------------------------


    //
    // Center panel
    //
    Item {
        id: centerPanel

        anchors {
            left: leftPanel.right
            right: parent.right
            top: parent.top
            bottom: bottomPanel.top
        }


        AgentsMappingView {
            id: agentsMappingView

            anchors.fill: parent

            controller: IngeScapeEditorC.agentsMappingC
        }
    }


    //
    // Bottom panel
    //
    Scenario.ScenarioTimeLine {
        id : bottomPanel
        anchors {
            left: leftPanel.right
            right: parent.right
            bottom: parent.bottom
        }
        height: 0
        controller: IngeScapeEditorC.scenarioC;
    }


    //
    // Left panel
    //
    Rectangle {
        id: leftPanel

        width: IngeScapeTheme.leftPanelWidth

        anchors {
            left: parent.left
            top: parent.top
            bottom: parent.bottom
        }

        color : IngeScapeTheme.agentsMappingBackgroundColor;

        I2CustomRectangle {
            anchors {
                fill: parent
                topMargin: 9
            }
            color: IngeScapeTheme.leftPanelBackgroundColor

            fuzzyRadius: 8
            topRightRadius : 5

            borderWidth: 1
            borderColor: IngeScapeTheme.selectedTabsBackgroundColor

            // tabs of left panel
            I2TabView {
                id : leftPanelTabs

                anchors.fill: parent

                style: I2TabViewStyle {
                    frameOverlap: 1

                    tab: I2CustomRectangle {
                        color: styleData.selected ? IngeScapeTheme.selectedTabsBackgroundColor : "transparent"
                        implicitWidth: (IngeScapeEditorC.recordsSupervisionC && (IngeScapeEditorC.recordsSupervisionC.isRecorderON === true)) ? leftPanelTabs.width / 4
                                                                                                                                              : leftPanelTabs.width / 3
                        implicitHeight: 26
                        topRightRadius : 5

                        visible: (styleData.index === 3) ? (IngeScapeEditorC.recordsSupervisionC && (IngeScapeEditorC.recordsSupervisionC.isRecorderON === true))
                                                         : true

                        Text {
                            id: text
                            anchors.fill: parent
                            horizontalAlignment: Text.AlignHCenter
                            verticalAlignment: Text.AlignVCenter;
                            text: styleData.title
                            color: styleData.selected ? IngeScapeTheme.agentsListLabelColor : IngeScapeTheme.greyColor
                            wrapMode: Text.Wrap;

                            font {
                                family: IngeScapeTheme.labelFontFamily;
                                weight: Font.ExtraBold;
                                pixelSize:18;
                                capitalization: Font.AllUppercase;
                            }
                        }
                    }

                    frame: Rectangle {
                        color: IngeScapeTheme.selectedTabsBackgroundColor
                    }
                }

                currentIndex: 0

                /*onCurrentIndexChanged: {
                    console.log("on Current Index changed: " + currentIndex);
                }*/

                Connections {
                    target: IngeScapeEditorC.recordsSupervisionC

                    onIsRecorderONChanged: {
                        //console.log("on Is Recorder ON changed: " + IngeScapeEditorC.recordsSupervisionC.isRecorderON);

                        if (IngeScapeEditorC.recordsSupervisionC && (IngeScapeEditorC.recordsSupervisionC.isRecorderON === false)) {
                            leftPanelTabs.currentIndex = 0;
                        }
                    }
                }


                Tab {
                    id: tabAgents

                    title: qsTr("Agents");
                    active : true

                    Agent.AgentsList {
                        id: agentsList

                        anchors.fill: parent

                        controller: IngeScapeEditorC.agentsSupervisionC
                    }
                }

                Tab {
                    id: tabHosts

                    title: qsTr("Hosts");

                    Host.HostsList {
                        id: hostsList

                        anchors.fill: parent

                        controller: IngeScapeEditorC.hostsSupervisionC
                    }
                }

                Tab {
                    id: idActions

                    title: qsTr("ACTIONS");
                    active : false

                    Item {
                        anchors.fill: parent

                        Scenario.ActionsList {
                            id: actionsList

                            anchors {
                                fill : parent
                                bottomMargin: actionsPanel.height
                            }

                            controller: IngeScapeEditorC.scenarioC
                        }

                        Scenario.ActionsPanel {
                            id : actionsPanel
                            anchors {
                                left : parent.left
                                right: parent.right
                                bottom : parent.bottom
                            }
                            height : 320
                            controller: IngeScapeEditorC.scenarioC
                        }
                    }
                }

                Tab {
                    id: tabRecords

                    title: qsTr("Records");

                    Record.RecordsList {
                        controller: IngeScapeEditorC.recordsSupervisionC
                        id: recordsList
                        anchors.fill: parent
                    }
                }
            }
        }
    }




    //
    // DEBUG: list of all "Agents Grouped by Name"
    //
    ListView {
        id: debug

        anchors {
            bottom: parent.bottom
            bottomMargin: 100
            left: parent.left
            right: parent.right
        }
        height: contentHeight

        model: IngeScapeEditorC.modelManager.allAgentsGroupedByName

        delegate: Rectangle {

            property var groupByNameVM: model.QtObject

            anchors {
                left: parent.left
                leftMargin: 10
                //right: parent.right
                //rightMargin: 10
            }
            width: childrenRect.width + 5
            height: columnListOfGroupsByDefinition.height + 5

            color: groupByNameVM.isON ? "#22CCCC" : "#227777"

            Row {
                spacing: 15

                Text {
                    text: groupByNameVM.name
                    width: 150
                    color: groupByNameVM.isON ? "white" : "#888888"
                    font {
                        pointSize: 14
                        weight: Font.Bold
                    }
                }

                Text {
                    text: groupByNameVM.models.count + " M"
                    color: groupByNameVM.isON ? "white" : "#888888"
                    font.pointSize: 14
                }

                Text {
                    text: groupByNameVM.numberOfAgentsON + " ON"
                    color: "#00FF00"
                    font {
                        pointSize: 14
                        weight: Font.Bold
                    }
                }

                Text {
                    text: groupByNameVM.numberOfAgentsOFF + " OFF"
                    color: "#FF0000"
                    font {
                        pointSize: 14
                        weight: Font.Bold
                    }
                }

                Column {
                    id: columnListOfGroupsByDefinition

                    height: childrenRect.height + 5

                    Repeater {
                        model: groupByNameVM.allGroupsByDefinition

                        Rectangle {

                            property var groupByDefinitionVM: model.QtObject

                            width: childrenRect.width + 5
                            height: childrenRect.height + 5

                            color: groupByDefinitionVM.isON ? "#22CC22" : "#227722"

                            Row {
                                spacing: 15

                                Text {
                                    text: groupByDefinitionVM.definition ? groupByDefinitionVM.definition.name : "Def is NULL"
                                    color: groupByDefinitionVM.isON ? "white" : "#888888"
                                    width: 150
                                    elide: Text.ElideMiddle
                                    font.pointSize: 14
                                }

                                Text {
                                    text: groupByDefinitionVM.models.count + " M"
                                    color: groupByDefinitionVM.isON ? "white" : "#888888"
                                    font.pointSize: 14
                                }


                                Column {
                                    id: columnAgents

                                    height: childrenRect.height + 5

                                    Repeater {
                                        model: groupByDefinitionVM.models

                                        Rectangle {

                                            property var agentM: model.QtObject

                                            width: childrenRect.width
                                            height: 25

                                            color: agentM.isON ? "#2222CC" : "#222277"

                                            Row {
                                                spacing: 15

                                                Text {
                                                    text: agentM.hostname
                                                    color: agentM.isON ? "white" : "#888888"
                                                    font {
                                                        pointSize: 14
                                                        weight: Font.Bold
                                                    }
                                                }

                                                Text {
                                                    text: agentM.peerId
                                                    color: agentM.isON ? "white" : "#888888"
                                                }

                                                Text {
                                                    text: agentM.commandLine
                                                    color: agentM.isON ? "white" : "#888888"
                                                    width: 400
                                                    elide: Text.ElideLeft
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }






    // List of "Agent Definition Editor(s)"
    Repeater {
        model: (IngeScapeEditorC.modelManager ? IngeScapeEditorC.modelManager.openedDefinitions : 0)

        delegate: Item {
            Agent.AgentDefinitionEditor {
                id: agentDefinitionEditor

                visible: true

                Component.onCompleted: {
                    x = rootItem.Window.window.x + rootItem.Window.width/2 - agentDefinitionEditor.width/2 + (index * 40);
                    y = rootItem.Window.window.y + rootItem.Window.height/2  - agentDefinitionEditor.height / 2.0 + (index * 40);

                    // Add this window to the list of opened windows
                    IngeScapeEditorC.addOpenedWindow(agentDefinitionEditor);
                }

                onClosing: {
                    IngeScapeEditorC.closeDefinition(model.QtObject);
                }

                onOpenHistory : {
                    //console.log("QML: slot onOpenHistory");

                    historyPanel.show();

                    // Raises the window in the windowing system
                    historyPanel.raise();
                }
            }
        }
    }


    // List of "Actions Editor(s)"
    Repeater {
        model: IngeScapeEditorC.scenarioC ? IngeScapeEditorC.scenarioC.openedActionsEditorsControllers : 0;

        delegate: Item {
            Scenario.ActionEditor {
                id: actionEditor

                controller : IngeScapeEditorC.scenarioC
                panelController: model.QtObject

                visible: true

                Component.onCompleted: {
                    x = rootItem.Window.window.x + rootItem.Window.width/2 - actionEditor.width/2  + (index * 40);
                    y = rootItem.Window.window.y + rootItem.Window.height/2 - actionEditor.height/2 + (index * 40);

                    // Add this window to the list of opened windows
                    IngeScapeEditorC.addOpenedWindow(actionEditor);
                }

                onClosingVersion: {
                    IngeScapeEditorC.closeActionEditor(model.QtObject);
                }
            }
        }
    }


    //
    // List of "Log Stream Viewer"
    //
    Repeater {
        model : IngeScapeEditorC.openedLogStreamControllers

        delegate: Item {
            Agent.LogStreamPanel {
                id: logStreamPanel

                controller: model.QtObject

                visible: true

                Component.onCompleted: {
                    x = rootItem.Window.window.x + rootItem.Window.width/2 - logStreamPanel.width/2  + (index * 40);
                    y = rootItem.Window.window.y + rootItem.Window.height/2 - logStreamPanel.height/2 + (index * 40);

                    // Add this window to the list of opened windows
                    IngeScapeEditorC.addOpenedWindow(logStreamPanel);
                }

                onClosingVersion: {
                    IngeScapeEditorC.closeLogStreamController(model.QtObject);
                }
            }
        }
    }


    //
    // History Panel
    //
    Agent.HistoryPanel {
        id: historyPanel

        Component.onCompleted: {
            // Center window
            x = rootItem.Window.window.x + rootItem.Window.width/2 - historyPanel.width/2;
            y = rootItem.Window.window.y + rootItem.Window.height/2 - historyPanel.height/2;
        }

        onVisibleChanged: {
            //console.log("onVisibleChanged of historyPanel: visible = " + historyPanel.visible);

            if (visible) {
                // Add this window to the list of opened windows
                IngeScapeEditorC.addOpenedWindow(historyPanel);
            }
            else {
                // Remove this window from the list of opened windows
                IngeScapeEditorC.removeOpenedWindow(historyPanel);
            }
        }
    }


    //
    // Network Configuration (Popup)
    //
    NetworkConfiguration {
        id: networkConfigurationPopup

        anchors.centerIn: parent
    }


    // Overlay layer used to display streaming
    I2Layer {
        id: streamingLayer

        objectName: "streamingLayer"

        anchors {
            left: leftPanel.right
            top: parent.top
            bottom: bottomPanel.top
            right: parent.right
            fill: null
        }
    }
}
