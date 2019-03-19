/*
 *	IngeScape Measuring
 *
 *  Copyright © 2019 Ingenuity i/o. All rights reserved.
 *
 *	See license terms for the rights and conditions
 *	defined by copyright holders.
 *
 *
 *	Contributors:
 *      Vincent Peyruqueou <peyruqueou@ingenuity.io>
 *
 */

import QtQuick 2.9
import QtQuick.Window 2.2
import QtQuick.Controls 1.4
import QtQml 2.2

import I2Quick 1.0

import INGESCAPE 1.0

//import QtQml.StateMachine 1.0


ApplicationWindow {
    id: mainWindow

    //----------------------------------
    //
    // Properties
    //
    //----------------------------------

    visible: true

    // Default size
    width: IngeScapeTheme.appDefaultWidth
    height: IngeScapeTheme.appDefaultHeight

    // Minimum size
    minimumWidth: IngeScapeTheme.appMinWidth
    minimumHeight: IngeScapeTheme.appMinHeight

    title: qsTr("IngeScape Measuring - v%1").arg(Qt.application.version)

    color: IngeScapeTheme.windowBackgroundColor


    //----------------------------------
    //
    // Menu
    //
    //----------------------------------

    menuBar: MenuBar {

        // Platform
        Menu {
            id: menuToRename
            title: qsTr("TO RENAME")

            /*MenuItem {
                text: qsTr("Start a new platform")
                shortcut: StandardKey.New

                onTriggered: {
                    IngeScapeMeasuringC.clearCurrentPlatform();
                }
            }

            MenuItem {
                text: qsTr("Open a platform...")
                shortcut: StandardKey.Open

                onTriggered: {
                    IngeScapeMeasuringC.loadPlatformFromSelectedFile();
                }
            }

            MenuItem {
                text: qsTr("Save the current platform...")
                shortcut: StandardKey.Save

                onTriggered: {
                    IngeScapeMeasuringC.savePlatformToSelectedFile();
                }
            }*/

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
                    I2SnapshotHelper.saveWindowOfItem(content, Qt.size(0,0), "IngeScape-Measuring");
                }
            }
        }


        // Windows
        /*Menu {
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
                model: 0 // IngeScapeMeasuringC.openedWindows

                MenuItem {
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
        }*/
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
        IngeScapeMeasuringC.processBeforeClosing();
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

            /*onVisibleChanged: {
                if (visible) {
                    if (IngeScapeMeasuringC.agentsMappingC) {
                        IngeScapeMeasuringC.agentsMappingC.fitToView();
                    }
                }
            }*/
        }


        // This animation is used to introduce a small delay to give QML
        // the necessary time to load and start our loading screen
        PauseAnimation {
            id: loaderDelayAnimation

            duration: 100

            onStopped: {
                // Init controller
                IngeScapeMeasuringC.forceCreation();

                // Set the directory used to store snasphots of our application
                I2SnapshotHelper.directory = IngeScapeMeasuringC.snapshotDirectory;

                // Binding to display our application loader
                applicationLoader.visible = Qt.binding(function() {
                    return ((applicationLoader.status === Loader.Ready) && (IngeScapeMeasuringC.modelManager !== null));
                });

                /*subWindowsInstantiator.model = Qt.binding(function() {
                    return IngeScapeMeasuringC.openedWindows;
                });*/

                // Load our QML UI
                applicationLoader.source = "IngeScapeMeasuring.qml";
            }
        }


        // Overlay layer used to display popups above the content of our window
        I2Layer {
            id: overlayLayer

            objectName: "overlayLayer"

            anchors.fill: parent
        }

        // Overlay layer used to display draggable agent item above the content of our window and the popups
        /*I2Layer {
            id: overlayLayerDraggableItem
            objectName: "overlayLayerDraggableItem"

            anchors.fill: parent
        }*/

        // Overlay layer used by comboboxes
        I2Layer {
            id: overlayLayerComboBox

            objectName: "overlayLayerComboBox"

            anchors.fill: parent
        }


    }
}
