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
            title: qsTr("Supervision")

            MenuItem {
                text: qsTr("Create a new Agent")
                enabled: false
                onTriggered: {
                    //console.log("Create a new Agent");
                }
            }
            MenuItem {
                text: qsTr("Open agents")
                onTriggered: {
                    //console.log("QML: Open agents");

                    if (MasticEditorC.modelManager) {
                        MasticEditorC.modelManager.importAgentsListFromSelectedFile();
                    }
                }
            }
            MenuItem {
                text: qsTr("Save agents")

                onTriggered: {
                    //console.log("QML: Save agents");

                    if (MasticEditorC.agentsSupervisionC) {
                        MasticEditorC.agentsSupervisionC.exportAgentsListToSelectedFile();
                    }
                }
            }

            MenuItem {
                text: qsTr("Show history")
                onTriggered: {
                    //console.log("Show history");

                    if (applicationLoader.item) {
                        applicationLoader.item.openHistory();
                    }
                }
            }
        }

        Menu {
            title: qsTr("Mapping")

            MenuItem {
                text: qsTr("Create a new mapping")

                onTriggered: {
                    //console.log("Create a new mapping");

                    if (MasticEditorC.agentsMappingC) {
                        MasticEditorC.agentsMappingC.createNewMapping();
                    }
                }
            }

            MenuItem {
                text: qsTr("Open mapping")

                onTriggered: {
                    console.log("Open mapping");
                    if (MasticEditorC.agentsMappingC) {
                        MasticEditorC.agentsMappingC.openMapping();
                    }
                }
            }

            MenuItem {
                text: qsTr("Save mapping")

                onTriggered: {
                    console.log("Save mapping");
                    if (MasticEditorC.agentsMappingC) {
                        MasticEditorC.agentsMappingC.saveMapping();
                    }
                }
            }

            MenuItem {
                text: (MasticEditorC.modelManager && MasticEditorC.modelManager.isActivatedMapping) ? qsTr("Deactivate mapping") : qsTr("Activate mapping")
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
            title: qsTr("Scenario")

            MenuItem {
                text: qsTr("Open scenario")
                onTriggered: {
                    if (MasticEditorC.scenarioC) {
                        MasticEditorC.scenarioC.importScenarioFromFile();
                    }
                }
            }
            MenuItem {
                text: qsTr("Save scenario")
                onTriggered: {
                    if (MasticEditorC.scenarioC) {
                        MasticEditorC.scenarioC.exportScenarioToSelectedFile();
                    }
                }
            }
        }

        /*Menu {
            title: qsTr("Enregistrement")

            MenuItem {
                text: qsTr("TODO 1")
                onTriggered: {
                    console.log("Enregistrement TODO 1");
                }
            }
            MenuItem {
                text: qsTr("TODO 2")
                onTriggered: {
                    console.log("Enregistrement TODO 2");
                }
            }
        }*/

        Menu {
            title: qsTr("Tools")

            MenuItem {
                text: qsTr("Create snapshot")
                onTriggered: {
                    I2SnapshotHelper.saveWindowOfItem(content, Qt.size(0,0), "MASTIC");
                }
            }

            /*MenuItem {
                text: qsTr("Fermer l'application")
                onTriggered: {
                    console.info("QML: close application");
                    Qt.quit();
                }
            }*/
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
        if (MasticEditorC.agentsSupervisionC) {
            MasticEditorC.agentsSupervisionC.exportAgentsListToDefaultFile();
        }
    }


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

            text: qsTr("Erreur QML fatale !!");

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

        I2Layer {
            id: overlayLayerComboBox
            objectName: "overlayLayerComboBox"

            anchors.fill: parent
        }


    }

}
