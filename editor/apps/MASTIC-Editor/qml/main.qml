/*
 *	MASTIC Editor
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
import QtQuick.Window 2.2
import QtQuick.Controls 1.4

import I2Quick 1.0

import MASTIC 1.0


ApplicationWindow {
    id: mainWindow

    //----------------------------------
    //
    // Properties
    //
    //----------------------------------

    visible: true

    // Default size
    width: MasticTheme.appDefaultWidth
    height: MasticTheme.appDefaultHeight

    // Minimum size
    minimumWidth: MasticTheme.appMinWidth
    minimumHeight: MasticTheme.appMinHeight

    title: qsTr("MASTIC Editor - v%1").arg(Qt.application.version)

    color: MasticTheme.windowBackgroundColor



    //----------------------------------
    //
    // Menu
    //
    //----------------------------------

    menuBar: MenuBar {

        Menu {
            title: qsTr("Platform")

            MenuItem {
                text: qsTr("Start a new platform description")
                onTriggered: {
                    MasticEditorC.createNewPlatform();
                }
            }
            MenuItem {
                text: qsTr("Open platform description")
                onTriggered: {
                    MasticEditorC.openPlatformFromFile();
                }
            }
            MenuItem {
                text: qsTr("Save platform description")
                onTriggered: {
                    MasticEditorC.savePlatformToSelectedFile();
                }
            }
        }

        Menu {
            title: qsTr("Mapping")

            MenuItem {
                text: (MasticEditorC.modelManager && MasticEditorC.modelManager.isActivatedMapping) ? qsTr("Unplug mapping") : qsTr("Plug mapping")
                //enabled: false

                onTriggered: {
                    if (MasticEditorC.modelManager && MasticEditorC.modelManager.isActivatedMapping) {
                        //console.log("DE-activate mapping");
                        MasticEditorC.modelManager.isActivatedMapping = false;
                    }
                    else {
                        //console.log("Activate mapping");
                        MasticEditorC.modelManager.isActivatedMapping = true;
                    }
                }
            }

            MenuSeparator {
            }

            MenuItem {
                text: qsTr("Zoom In")

                shortcut: StandardKey.ZoomIn

                onTriggered: {
                    if (MasticEditorC.agentsMappingC)
                    {
                        MasticEditorC.agentsMappingC.zoomIn();
                    }
                }
            }

            MenuItem {
                text: qsTr("Zoom Out")

                shortcut: StandardKey.ZoomOut

                onTriggered: {
                    if (MasticEditorC.agentsMappingC)
                    {
                        MasticEditorC.agentsMappingC.zoomOut();
                    }
                }
            }

            MenuItem {
                text: qsTr("Fit all in view")

                onTriggered: {
                    if (MasticEditorC.agentsMappingC)
                    {
                        MasticEditorC.agentsMappingC.fitToView();
                    }
                }
            }

            MenuItem {
                text: qsTr("100%")

                onTriggered: {
                    if (MasticEditorC.agentsMappingC)
                    {
                        MasticEditorC.agentsMappingC.resetZoom();
                    }
                }
            }
        }


        Menu {
            title: qsTr("Supervision")

            MenuItem {
                text: qsTr("Create a new Agent")
                enabled: false
                onTriggered: {
                    //console.log("Create a new Agent");
                }
            }
            MenuItem {
                text: qsTr("Import agents")
                onTriggered: {
                    if (MasticEditorC.modelManager) {
                        MasticEditorC.modelManager.importAgentsListFromSelectedFile();
                    }
                }
            }
            MenuItem {
                text: qsTr("Export agents")

                onTriggered: {
                    if (MasticEditorC.agentsSupervisionC) {
                        MasticEditorC.agentsSupervisionC.exportAgentsListToSelectedFile();
                    }
                }
            }

            MenuItem {
                text: qsTr("Show history")
                onTriggered: {
                    if (applicationLoader.item) {
                        applicationLoader.item.openHistory();
                    }
                }
            }
        }


        Menu {
            title: qsTr("Tools")

            MenuItem {
                text: qsTr("Create snapshot")
                onTriggered: {
                    I2SnapshotHelper.saveWindowOfItem(content, Qt.size(0,0), "MASTIC");
                }
            }

        }
    }


    //----------------------------------
    //
    // Behaviors
    //
    //----------------------------------

    Component.onCompleted: {
        // Start our loader delay animation when our initial content is ready
        loaderDelayAnimation.start();
    }

    // When user clicks on window close button
    onClosing: {
        console.info("QML: Close Window");
        if (MasticEditorC) {
            MasticEditorC.processBeforeClosing();
        }
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
        }


        // This animation is used to introduce a small delay to give QML
        // the necessary time to load and start our loading screen
        PauseAnimation {
            id: loaderDelayAnimation

            duration: 100

            onStopped: {
                // Init controller
                MasticEditorC.forceCreation();

                // Set the directory used to store snasphots of our application
                I2SnapshotHelper.directory = MasticEditorC.snapshotDirectory;

                // Binding to display our application loader
                applicationLoader.visible = Qt.binding(function() {
                    return ((applicationLoader.status === Loader.Ready) && (MasticEditorC.modelManager !== null));
                });

                // Load our QML UI
                applicationLoader.source = "MasticEditor.qml";
            }
        }


        // Overlay layer used to display popups above the content of our window
        I2Layer {
            id: overlayLayer

            objectName: "overlayLayer"

            anchors.fill: parent
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


    }

}
