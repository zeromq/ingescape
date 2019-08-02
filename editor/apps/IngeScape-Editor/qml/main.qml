/*
 *	IngeScape Editor
 *
 *  Copyright © 2017-2019 Ingenuity i/o. All rights reserved.
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
import QtQuick.Window 2.2
import QtQuick.Controls 1.4
import QtQml 2.2

import I2Quick 1.0

import INGESCAPE 1.0

// popup sub-directory
import "popup" as Popups


ApplicationWindow {
    id: mainWindow

    //----------------------------------
    //
    // Properties
    //
    //----------------------------------

    visible: true

    // Default size
    width: IngeScapeEditorTheme.appDefaultWidth
    height: IngeScapeEditorTheme.appDefaultHeight

    // Minimum size
    minimumWidth: IngeScapeEditorTheme.appMinWidth
    minimumHeight: IngeScapeEditorTheme.appMinHeight

    title: qsTr("IngeScape Editor - v%1").arg(Qt.application.version)

    color: IngeScapeTheme.windowBackgroundColor


    //----------------------------------
    //
    // Menu
    //
    //----------------------------------

    menuBar: MenuBar {

        // Platform
        Menu {
            id: menuPlatform
            title: qsTr("Platform")

            MenuItem {
                text: qsTr("Start a new platform")
                shortcut: StandardKey.New

                onTriggered: {
                    IngeScapeEditorC.clearCurrentPlatform();
                }
            }

            MenuItem {
                text: qsTr("Open a platform...")
                shortcut: StandardKey.Open

                onTriggered: {
                    IngeScapeEditorC.loadPlatformFromSelectedFile();
                }
            }

            MenuItem {
                text: qsTr("Save the current platform...")
                shortcut: StandardKey.Save

                onTriggered: {
                    IngeScapeEditorC.savePlatformToSelectedFile();
                }
            }

            MenuSeparator {
            }

            MenuItem {
                text: qsTr("Platform network...")
                shortcut: StandardKey.Preferences

                onTriggered: {
                    if (applicationLoader.item) {
                        //console.log("QML: open network configuration");
                        applicationLoader.item.openNetworkConfiguration();
                    }
                }
            }
            MenuItem {
                text: qsTr("Create snapshot")
                shortcut: StandardKey.AddTab

                onTriggered: {
                    I2SnapshotHelper.saveWindowOfItem(content, Qt.size(0,0), "IngeScape-Editor");
                }
            }
            MenuItem {
                id: menuItemModelVisualizer

                text: "" // (IngeScapeEditorC.isVisibleModelVisualizer ? qsTr("Hide the model visualizer") : qsTr("Show the model visualizer"))
                visible: false

                onTriggered: {
                    if (IngeScapeEditorC.isVisibleModelVisualizer) {
                        console.log("Hide the Model and View Model Visualizer");
                        IngeScapeEditorC.isVisibleModelVisualizer = false;
                    }
                    else {
                        console.log("Show the Model and View Model Visualizer");
                        IngeScapeEditorC.isVisibleModelVisualizer = true;
                    }
                }
            }
        }


        // Mapping
        Menu {
            title: qsTr("Mapping")

            MenuItem {
                id: menuPlugUNplugMapping

                text: "" // (IngeScapeEditorC.modelManager && IngeScapeEditorC.modelManager.isMappingConnected) ? qsTr("Disconnect mapping") : qsTr("Connect mapping")

                onTriggered: {
                    if (IngeScapeEditorC.modelManager && IngeScapeEditorC.modelManager.isMappingConnected) {
                        //console.log("DE-activate mapping");
                        IngeScapeEditorC.modelManager.isMappingConnected = false;
                    }
                    else {
                        //console.log("Activate mapping");
                        IngeScapeEditorC.modelManager.isMappingConnected = true;
                    }
                }
            }

            MenuSeparator {
            }

            MenuItem {
                text: qsTr("Zoom In")
                shortcut: StandardKey.ZoomIn

                onTriggered: {
                    if (IngeScapeEditorC.agentsMappingC)
                    {
                        IngeScapeEditorC.agentsMappingC.zoomIn();
                    }
                }
            }

            MenuItem {
                text: qsTr("Zoom Out")
                shortcut: StandardKey.ZoomOut

                onTriggered: {
                    if (IngeScapeEditorC.agentsMappingC)
                    {
                        IngeScapeEditorC.agentsMappingC.zoomOut();
                    }
                }
            }

            MenuItem {
                text: qsTr("Fit all in view")

                onTriggered: {
                    if (IngeScapeEditorC.agentsMappingC)
                    {
                        IngeScapeEditorC.agentsMappingC.fitToView();
                    }
                }
            }

            MenuItem {
                text: qsTr("Scale to 100%")

                onTriggered: {
                    if (IngeScapeEditorC.agentsMappingC)
                    {
                        IngeScapeEditorC.agentsMappingC.resetZoom();
                    }
                }
            }
        }


        // Supervision
        Menu {
            title: qsTr("Agents")

            MenuItem {
                text: qsTr("Create a new Agent...")
                enabled: false

                onTriggered: {
                    //console.log("Create a new Agent");
                }
            }

            MenuItem {
                text: qsTr("Import agents...")

                onTriggered: {
                    if (IngeScapeEditorC.modelManager)
                    {
                        var success = IngeScapeEditorC.modelManager.importAgentOrAgentsListFromSelectedFile();
                        if (!success) {
                            popupErrorMessage.open();
                        }
                    }
                }
            }

            MenuItem {
                text: qsTr("Export agents...")

                onTriggered: {
                    if (IngeScapeEditorC.modelManager) {
                        IngeScapeEditorC.modelManager.exportAgentsListToSelectedFile();
                    }
                }
            }

            MenuItem {
                text: qsTr("Show outputs history")

                onTriggered: {
                    if (applicationLoader.item) {
                        applicationLoader.item.openHistory();
                    }
                }
            }
        }


        // Windows
        Menu {
            id: menuWindows

            title: qsTr("Windows")

            MenuItem {
                text: qsTr("Main window")

                onTriggered: {
                    // Raises our window in the windowing system
                    mainWindow.raise();
                }
            }

            Instantiator {
                id: subWindowsInstantiator
                model: 0 // IngeScapeEditorC.openedWindows

                delegate: MenuItem {
                    text: model.QtObject.title

                    onTriggered: {
                        //console.log("click on " + model.QtObject.title + " (" + model.QtObject + ")")

                        // Raises the window in the windowing system
                        model.QtObject.raise();
                    }
                }

                onObjectAdded: {
                    //console.log("onObjectAdded " + index)
                    menuWindows.insertItem(index, object)
                }
                onObjectRemoved: {
                    //console.log("onObjectRemoved")
                    menuWindows.removeItem(object)
                }
            }
        }


        // Debug
        Menu {
            title: qsTr("Debug Qt Quick")

            visible: SHOW_DEBUG_MENU

            ExclusiveGroup {
                id: visualizeGroup
            }

            MenuItem {
                id: visualizeNormal

                text: qsTr("Normal rendering")
                checkable: true
                checked: true
                exclusiveGroup: visualizeGroup

                onTriggered: {
                      DebugQuickInspector.currentWindowRenderingMode = DebugWindowRenderingMode.Normal;
                }
            }

            MenuItem {
                id: visualizeClipping

                text: qsTr("Visualize clipping")
                checkable: true
                exclusiveGroup: visualizeGroup

                onTriggered: {
                      DebugQuickInspector.currentWindowRenderingMode = DebugWindowRenderingMode.VisualizeClipping;
                }
            }

            MenuItem {
                id: visualizeBatches

                text: qsTr("Visualize batches")
                checkable: true
                exclusiveGroup: visualizeGroup

                onTriggered: {
                     DebugQuickInspector.currentWindowRenderingMode = DebugWindowRenderingMode.VisualizeBatches;
                }
            }

            MenuItem {
                id: visualizeOverdraw

                text: qsTr("Visualize overdraw")
                checkable: true
                exclusiveGroup: visualizeGroup

                onTriggered: {
                      DebugQuickInspector.currentWindowRenderingMode = DebugWindowRenderingMode.VisualizeOverdraw;
                }
            }

            MenuItem {
                id: visualizeChanges

                text: qsTr("Visualize changes")
                checkable: true
                exclusiveGroup: visualizeGroup

                onTriggered: {
                      DebugQuickInspector.currentWindowRenderingMode = DebugWindowRenderingMode.VisualizeChanges;
                }
            }
        }


        // Help
        Menu {
            title: qsTr("&Help")

            MenuItem {
                text: qsTr("Qt Quick infos")

                onTriggered: qtQuickInfoPopup.open();
            }
        }
    }


    //----------------------------------
    //
    // Behaviors
    //
    //----------------------------------

    Component.onCompleted: {
        // Define the window associated to Qt Quick inspector
        DebugQuickInspector.currentWindow = mainWindow;

        // Start our loader delay animation when our initial content is ready
        loaderDelayAnimation.start();
    }

    // When user clicks on window close button
    onClosing: {
        console.info("QML: Close Window");
        IngeScapeEditorC.processBeforeClosing();
    }


    //----------------------------------
    //
    // Content
    //
    //----------------------------------


    // Content of our window
    Item {
        id: content

        anchors.fill: parent

        // LoadingScreen i.e. content displayed at startup
        LoadingScreen {
            id: loadingScreen

            anchors.fill: parent

            visible: (opacity !== 0)
            opacity: ((applicationLoader.status === Loader.Error) || applicationLoader.visible) ? 0 : 1

            Behavior on opacity {
                NumberAnimation {
                    duration: 200
                    easing.type: Easing.OutQuad;
                }
            }
        }


        // Error feedback displayed when the content of our application
        // can not be loaded due to a QML error
        Text {
            anchors. fill: parent

            visible: (applicationLoader.status === Loader.Error)

            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter

            text: qsTr("!! QML FATAL ERROR !!");

            color: "red"

            font {
                bold: true
                pixelSize: 72
            }
        }


        // Real application content load asynchronously
        Loader {
            id: applicationLoader

            anchors.fill: parent

            asynchronous: true

            visible: false
            opacity: (visible ? 1 : 0)

            Behavior on opacity {
                NumberAnimation {
                    duration: 200
                    easing.type: Easing.OutQuad;
                }
            }

            /*onLoaded: {
                console.log("onLoaded " + applicationLoader.item)
            }*/

            onVisibleChanged: {
                if (visible) {
                    if (IngeScapeEditorC.agentsMappingC) {
                        IngeScapeEditorC.agentsMappingC.fitToView();
                    }
                }
            }
        }


        // This animation is used to introduce a small delay to give QML
        // the necessary time to load and start our loading screen
        PauseAnimation {
            id: loaderDelayAnimation

            duration: 100

            onStopped: {
                // Init controller
                IngeScapeEditorC.forceCreation();

                // Set the directory used to store snasphots of our application
                I2SnapshotHelper.directory = IngeScapeEditorC.snapshotDirectory;

                // Binding to display our application loader
                applicationLoader.visible = Qt.binding(function() {
                    return ((applicationLoader.status === Loader.Ready) && (IngeScapeEditorC.modelManager !== null));
                });

                mainWindow.title = Qt.binding(function() {
                    return qsTr("IngeScape Editor - v%1 - %2").arg(Qt.application.version).arg(IngeScapeEditorC.currentPlatformName);
                });

                menuPlugUNplugMapping.text = Qt.binding(function() {
                    return (((IngeScapeEditorC.modelManager !== null) && IngeScapeEditorC.modelManager.isMappingConnected) ? qsTr("Unplug mapping") : qsTr("Plug mapping"));
                });

                subWindowsInstantiator.model = Qt.binding(function() {
                    return IngeScapeEditorC.openedWindows;
                });

                // Load our QML UI
                applicationLoader.source = "IngeScapeEditor.qml";

                // Model and View Model Visualizer
                if (IngeScapeEditorC.isAvailableModelVisualizer)
                {
                    menuItemModelVisualizer.visible = true;

                    menuItemModelVisualizer.text = Qt.binding(function() {
                        return (IngeScapeEditorC.isVisibleModelVisualizer ? qsTr("Hide the model visualizer") : qsTr("Show the model visualizer"));
                    });
                }
            }
        }


        //----------------------------------
        //
        // Layers
        //
        //----------------------------------

        // Overlay layer used to display popups above the content of our window
        I2Layer {
            id: overlayLayer

            objectName: "overlayLayer"

            anchors.fill: parent


            //
            // Popup for Error messages
            //
            Popups.MessagePopup {
                id: popupErrorMessage

                anchors.centerIn: parent
            }
        }


        // Overlay layer used to display draggable agent item above the content of our window and the popups
        I2Layer {
            id: overlayLayerDraggableItem
            objectName: "overlayLayerDraggableItem"

            anchors.fill: parent
        }


        // Overlay layer used by comboboxes
        I2Layer {
            id: overlayLayerComboBox
            objectName: "overlayLayerComboBox"

            anchors.fill: parent
        }


        //----------------------------------
        //
        // Popups
        //
        //----------------------------------

        // Qt Quick infos
        QtQuickInfoPopup {
            id: qtQuickInfoPopup
        }
    }
}
