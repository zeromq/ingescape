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
 *      Chloé Roumieu      <roumieu@ingenuity.io>
 */

import QtQuick 2.8
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4
import QtQml 2.2
import QtQuick.Controls 2.0 as Controls2
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

// popups
import "popup" as Popup


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

    // Flag indicating if an explicit request to open the Getting Started window has been performed.
    // Avoid opening the window at startup is none of the internet or the local page if accessible.
    property bool requestGettingStarted: false
    property bool gettingStartedOpen: false
    property GettingStartedWindow gettingStartedWindow: null

    // "Fake" model to create output history on the fly like the other secondary windows
    property var outputHistoryFakeModel: null



    //--------------------------------------------------------
    //
    //
    // Functions
    //
    //
    //--------------------------------------------------------

    signal raiseOutputHistory();

    //
    // function allowing to open the history panel
    //
    function openHistory() {
        if (outputHistoryFakeModel)
        {
            raiseOutputHistory();
        }
        else
        {
            outputHistoryFakeModel = 1
        }
    }


    //
    // Function allowing to open the Preferences popup
    //
    function openPreferences() {
        preferencesPopup.open();
    }


    //
    // Function allowing to open the license (configuration) popup
    //
    function openLicensePopup() {
        licensePopup.open();
    }


    //
    // Function allowing to open the license error popup
    //
    function openLicenseEventPopup() {
        licenseEventPopup.open();
    }


    //
    // Function allowing to open the Getting Started popup
    //
    function openGettingStarted(forceOpen) {
        // Default forceOpen parameter value
        if (forceOpen === undefined) forceOpen = false
        rootItem.requestGettingStarted = forceOpen;

        if (gettingStartedOpen && gettingStartedWindow)
        {
            gettingStartedWindow.raise();
        }
        else
        {
            // NOTE creating the window this way is required to have it as a separate window application
            var component = Qt.createComponent("GettingStartedWindow.qml");
            var window = component.createObject("rootItem");

            gettingStartedWindow = window;
            gettingStartedWindow.show()
            gettingStartedOpen = true;
        }
    }


    //
    // Function to close the getting started window
    //
    function closeGettingStarted() {
        if (gettingStartedWindow)
        {
            gettingStartedWindow.visible = false;
        }
    }


    //--------------------------------------------------------
    //
    //
    // Behaviors
    //
    //
    //--------------------------------------------------------

    // When the QML is loaded...
    Component.onCompleted: {
        // FIXME Several popup may appear at startup depending on the current platform configuration. Need to prioritize them and maybe show them sequentialy, not on top of each other.

        // First, we check if ingescape should be started at launch
        if (IngeScapeEditorC.ingescapeShouldBeStartedAtLaunch)
        {
            mappingModificationsPopup.open();
        }

        // ...we check the value of the flag "is Valid License"
        if (IngeScapeEditorC.licensesC && IngeScapeEditorC.licensesC.mergedLicense && !IngeScapeEditorC.licensesC.mergedLicense.editorLicenseValidity)
        {
            openLicensePopup();
        }

        // ...we silently call home to signal the license user launched the editor
        if (IngeScapeEditorC.licensesC && IngeScapeEditorC.callHomeC)
        {
            IngeScapeEditorC.callHomeC.editorLaunched(IngeScapeEditorC.licensesC.mergedLicense)
        }
    }


    Connections {
        target: IngeScapeEditorC

        onOpenPopupLicense: {
            openLicensePopup();
        }
    }

    Connections {
        target: IngeScapeEditorC.licensesC.mergedLicense

        onEditorLicenseValidityChanged: {
            console.log("QML (IngeScape Editor): on is Editor License Valid Changed");
            if (IngeScapeEditorC.licensesC && IngeScapeEditorC.licensesC.mergedLicense && !IngeScapeEditorC.licensesC.mergedLicense.editorLicenseValidity) {
                openLicensePopup();
            }
        }
    }

    Connections {
        target: IngeScapeEditorC.licensesC

        onLicenseLimitationReached: {
            console.log("QML (IngeScape Editor): on License Limitation Reached");
            openLicenseEventPopup();
        }
    }


    Connections {
        target: gettingStartedWindow

        onVisibleChanged: {
            rootItem.gettingStartedOpen = gettingStartedWindow.visible;
        }
    }

    // Connection to open getting started window only when notification popup is closed
    Connections {
        target: notifPopup

        onClosed: {
            // We check if we must open the getting started window
            if (IngeScapeEditorC.gettingStartedShowAtStartup)
            {
                openGettingStarted();
            }
        }

        onNoNotification : {
            // We check if we must open the getting started window
            if (IngeScapeEditorC.gettingStartedShowAtStartup)
            {
                openGettingStarted();
            }
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

            // Add extra selection for mapping mode
            // NB : extraContent property of NetworkConnectionInformationItem
            Item {
                id: addingHeaderOnline
                anchors {
                    left: parent.left
                    right: parent.right
                }
                height: IgsNetworkController.isStarted ? 50 : 0

                opacity: IgsNetworkController.isStarted ? 1 : 0
                visible:  (height !== 0)

                Behavior on height {
                    NumberAnimation {
                        duration: 250
                    }
                }

                Behavior on opacity {
                    NumberAnimation {
                        duration: 250
                    }
                }

                // Separator
                Rectangle {
                    id: separatorHeader
                    anchors {
                        left: parent.left
                        right: parent.right
                        top: parent.top
                    }
                    height: 1
                    color: IngeScapeTheme.editorsBackgroundBorderColor
                }

                CheckBox {
                    id : imposeMappingToAgentsON
                    anchors {
                        left: parent.left
                        right: parent.right
                        top: separatorHeader.bottom
                        topMargin: 10
                    }

                    checked: IngeScapeEditorC.modelManager.isMappingControlled
                    activeFocusOnPress: true

                    style: CheckBoxStyle {
                        label: Text {
                            text: qsTr("Impose platform's mapping on agents that connect to the network")
                            elide: Text.ElideRight
                            wrapMode: Text.Wrap

                            font {
                                family: IngeScapeTheme.heading2Font
                                weight: Font.Normal
                                pixelSize: 14
                            }
                            color: control.checked ? IngeScapeTheme.whiteColor : IngeScapeTheme.lightGreyColor
                        }

                        spacing: 7

                        indicator: Rectangle {
                            id: indicatorCheckBox
                            implicitWidth: 16
                            implicitHeight: 16
                            anchors.top: parent.top
                            border.width: 0
                            color: IngeScapeTheme.blueGreyColor

                            I2SvgItem {
                                visible: control.checked
                                anchors.centerIn: parent
                                svgFileCache: IngeScapeTheme.svgFileIngeScape
                                svgElementId: "check";
                            }
                        }
                    }

                    onCheckedChanged: {
                        IngeScapeEditorC.modelManager.isMappingControlled = checked;
                    }

                    Binding {
                        target: imposeMappingToAgentsON
                        property: "checked"
                        value: IngeScapeEditorC.modelManager.isMappingControlled
                    }
                }
            }

            onConnectChanged: {
                if (connect)
                {
                    mappingModificationsPopup.open();
                }
                else
                {
                    IngeScapeEditorC.stopIngeScape(false);
                }
            }

            onChangeNetworkSettings: {
                IngeScapeEditorC.port = port;
                IngeScapeEditorC.networkDevice = networkDevice;
                if (IgsNetworkController.isStarted)
                {
                     IngeScapeEditorC.restartIngeScape(false);
                }
                close();
            }
        }
    }


    //
    // Scenario TimeLine
    //
    ScenarioTimeLine {
        id: bottomPanel

        anchors {
            left: leftPanel.right
            right: parent.right
            bottom: parent.bottom
        }
        height: 0

        scenarioController: IngeScapeEditorC.scenarioC
        timeLineController: IngeScapeEditorC.timeLineC
        licensesController: IngeScapeEditorC.licensesC
        mainController: IngeScapeEditorC

        timelineButtonContent: LabellessSvgButton {
            id: playButton

            property string currentID : IngeScapeEditorC.scenarioC && IngeScapeEditorC.scenarioC.isPlaying ? "timeline-pause" : "timeline-play"

            enabled: IgsNetworkController.isStarted

            opacity: enabled ? 1.0 : 0.4

            releasedID: currentID
            pressedID: currentID + "-pressed"
            disabledID : currentID

            onClicked: {
                if (IngeScapeEditorC.scenarioC)
                {
                    if (IngeScapeEditorC.licensesC && IngeScapeEditorC.licensesC.mergedLicense
                            && !IngeScapeEditorC.licensesC.mergedLicense.editorLicenseValidity)
                    {
                        licensePopup.open();
                    }
                    else if (IngeScapeEditorC.scenarioC.isPlaying)
                    {
                        IngeScapeEditorC.scenarioC.pauseTimeLine();
                    }
                    else {
                        IngeScapeEditorC.scenarioC.playOrResumeTimeLine();
                    }
                }
            }
        }
    }


    //
    // Left panel
    //
    Rectangle {
        id: leftPanel

        width: IngeScapeEditorTheme.leftPanelWidth

        anchors {
            left: parent.left
            top: parent.top
            bottom: parent.bottom
        }

        color: IngeScapeEditorTheme.agentsMappingBackgroundColor

        I2CustomRectangle {
            anchors {
                fill: parent
                topMargin: 9
            }
            color: IngeScapeTheme.blackColor

            fuzzyRadius: 8
            topRightRadius : 5

            borderWidth: 1
            borderColor: IngeScapeEditorTheme.selectedTabsBackgroundColor

            // tabs of left panel
            I2TabView {
                id : leftPanelTabs

                anchors.fill: parent

                style: I2TabViewStyle {
                    frameOverlap: 1

                    tab: I2CustomRectangle {
                        color: styleData.selected ? IngeScapeEditorTheme.selectedTabsBackgroundColor : "transparent"

                        implicitWidth: (IngeScapeEditorC.recordsSupervisionC && IngeScapeEditorC.recordsSupervisionC.isRecorderON) ? leftPanelTabs.width / 4
                                                                                                                                   : leftPanelTabs.width / 3
                        implicitHeight: 26
                        topRightRadius : 5

                        visible: (styleData.index === 3) ? (IngeScapeEditorC.recordsSupervisionC && IngeScapeEditorC.recordsSupervisionC.isRecorderON)
                                                         : true

                        Text {
                            id: text

                            anchors.fill: parent
                            horizontalAlignment: Text.AlignHCenter
                            verticalAlignment: Text.AlignVCenter;

                            text: styleData.title

                            color: styleData.selected ? IngeScapeTheme.whiteColor : IngeScapeTheme.greyColor
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
                        color: IngeScapeEditorTheme.selectedTabsBackgroundColor
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

                        if (IngeScapeEditorC.recordsSupervisionC && !IngeScapeEditorC.recordsSupervisionC.isRecorderON) {
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

                        agentsSupervisionController: IngeScapeEditorC.agentsSupervisionC
                        licensesController: IngeScapeEditorC.licensesC

                        onUnlicensedAction: {
                            licensePopup.open();
                        }
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

                            scenarioController: IngeScapeEditorC.scenarioC

                            onUnlicensedAction: {
                                licensePopup.open();
                            }
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
    // Model Visualizer
    //
    Loader {
        id: loaderOfModelVisualizer

        anchors {
            top: parent.top
            left: leftPanel.right
            right: parent.right
        }
        height: 250

        source: IngeScapeEditorC.isAvailableModelVisualizer ? "ModelVisualizer.qml" : ""
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
                    rootItem.openHistory();
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

                onClosing: {
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

                onClosing: {
                    IngeScapeEditorC.closeLogStreamController(model.QtObject);
                }
            }
        }
    }


    //
    // History Panel
    //
    Instantiator {
        model: rootItem.outputHistoryFakeModel

        delegate: Agent.HistoryPanel {
            id: historyPanel

            visible: true

            Connections {
                target: rootItem

                onRaiseOutputHistory: {
                    raise();
                    requestActivated();
                }
            }

            Component.onCompleted: {
                // Center window
                x = rootItem.Window.window.x + rootItem.Window.width/2 - historyPanel.width/2;
                y = rootItem.Window.window.y + rootItem.Window.height/2 - historyPanel.height/2;
            }

            onClosing: {
                console.log("CLosing output history")
                rootItem.outputHistoryFakeModel = null
            }
        }

        onObjectAdded: {
            // Add this window to the list of opened windows
            IngeScapeEditorC.addOpenedWindow(historyPanel);
        }

        onObjectRemoved: {
            // Remove this window from the list of opened windows
            IngeScapeEditorC.removeOpenedWindow(historyPanel);
        }
    }


    //
    // Preferences popup
    //
    Popup.PreferencesPopup {
        id: preferencesPopup

        anchors.centerIn: parent
    }


    //
    // License (Configuration) Popup
    //
    LicensePopup {
        id: licensePopup

        anchors.centerIn: parent

        licensesController: IngeScapeEditorC.licensesC

        extraInformationOnNoLicense: "The editor is running in demo mode with limitations."
    }


    //
    // Popup displayed when an event occurs about the license(s)
    //
    LicenseEventPopup {
        id: licenseEventPopup

        licensesController: IngeScapeEditorC.licensesC

        onClosed: {
            console.log("Popup displayed when an event occurs about the license(s) has just been closed");
            licensePopup.open();
        }
    }

    //
    // Remote notification popup
    //
    Popup.RemoteNotificationPopup {
        id: notifPopup

        anchors.centerIn: parent
    }

    //
    // Mapping Modifications Popup
    //
    Popup.MappingModificationsPopup {
        id: mappingModificationsPopup

        onCancelMappingActivation: {
            console.log("on Cancel Mapping Activation");
            IngeScapeEditorC.stopIngeScape(false);
        }

        onSwitchToControl: {
            console.log("on Switch To Control");

            if (IngeScapeEditorC.modelManager)
            {
                IngeScapeEditorC.modelManager.isMappingControlled = true;
                IngeScapeEditorC.startIngeScape();
            }
        }

        onStayToObserve: {
            console.log("on Switch To Observe");
            if (IngeScapeEditorC.modelManager)
            {
                IngeScapeEditorC.modelManager.isMappingControlled = false;
                IngeScapeEditorC.startIngeScape();
            }
        }
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
