/*
 *	IngeScape Assessments
 *
 *  Copyright © 2019 Ingenuity i/o. All rights reserved.
 *
 *	See license terms for the rights and conditions
 *	defined by copyright holders.
 *
 *
 *	Contributors:
 *      Vincent Peyruqueou <peyruqueou@ingenuity.io>
 *      Alexandre Lemort   <lemort@ingenuity.io>
 *
 */

import QtQuick 2.8
import QtQuick.Window 2.2
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.3
import QtQml 2.2

import I2Quick 1.0

import INGESCAPE 1.0

// popup sub-directory
//import "popup" as Popups


ApplicationWindow {
    id: mainWindow

    //----------------------------------
    //
    // Properties
    //
    //----------------------------------

    title: qsTr("IngeScape Assessments (for ergonomics and human factors) - v%1").arg(Qt.application.version)


    // Default size
    width: IngeScapeAssessmentsTheme.appDefaultWidth
    height: IngeScapeAssessmentsTheme.appDefaultHeight

    // Minimum size
    minimumWidth: IngeScapeAssessmentsTheme.appMinWidth
    minimumHeight: IngeScapeAssessmentsTheme.appMinHeight


    visible: true


    //color: IngeScapeTheme.windowBackgroundColor
    color: IngeScapeTheme.veryDarkGreyColor


    // NB: we must override the style of our ApplicationWindow if we want a white background!
    // Because someone at Qt thinks that white means no background color and thus default system color
    // (i.e. light gray)
    // property QtObject styleData: QtObject {
    //    readonly property bool hasColor: window.color != "#ffffff"
    // }
    //
    style: ApplicationWindowStyle {
        background: Item {
        }
    }


    //----------------------------------
    //
    // Menu
    //
    //----------------------------------

    menuBar: MenuBar {
        // File
        Menu {
            id: menuFile

            title: qsTr("&File")

            /*MenuItem {
                text: qsTr("&New platform")

                shortcut: StandardKey.New

                onTriggered: {
                    IngeScapeAssessmentsC.clearCurrentPlatform();
                }
            }

            MenuItem {
                text: qsTr("Open a platform...")

                shortcut: StandardKey.Open

                onTriggered: {
                    IngeScapeAssessmentsC.loadPlatformFromSelectedFile();
                }
            }

            MenuSeparator {}

            MenuItem {
                text: qsTr("Save")

                shortcut: StandardKey.Save

                onTriggered: {
                    if (mainWindow.isEditorLicenseValid)
                    {
                        IngeScapeAssessmentsC.savePlatformToCurrentlyLoadedFile();
                    }
                    else
                    {
                        applicationLoader.item.openLicensePopup();
                    }
                }
            }

            MenuItem {
                text: qsTr("Save As...")

                shortcut: StandardKey.SaveAs

                onTriggered: {
                    if (mainWindow.isEditorLicenseValid)
                    {
                        IngeScapeAssessmentsC.selectFileToSavePlatform();
                    }
                    else
                    {
                        applicationLoader.item.openLicensePopup();
                    }
                }
            }

            MenuSeparator {}*/

            MenuItem {
                text: qsTr("Preferences...")

                //FIXME: enable this MenuItem when PreferencesPopup is ready (designed and coded)
                enabled: false

                shortcut: StandardKey.Preferences

                onTriggered: {
                    if (applicationLoader.item) {
                        console.log("QML: Open preferences popup");
                        applicationLoader.item.openPreferences();
                    }
                }
            }

            MenuItem {
                text: qsTr("Licenses...")

                onTriggered: {
                    if (applicationLoader.item) {
                        console.log("QML: Open license configuration popup");
                        applicationLoader.item.openLicensePopup();
                    }
                }
            }

            MenuSeparator {}

            MenuItem {
                text: qsTr("Take a snapshot")

                shortcut: StandardKey.AddTab

                onTriggered: {
                    I2SnapshotHelper.saveWindowOfItem(content, Qt.size(0,0), "IngeScape-Assessments");
                }
            }

            MenuSeparator {}

            MenuItem {
                text: qsTr("E&xit")

                shortcut: StandardKey.Quit

                onTriggered: {
                    // Try to close our man window
                    mainWindow.close();
                }
            }
        }


        // Edit
        Menu {
            id: menuEdit

            title: qsTr("&Edit")

            MenuItem {
                id: menuEditItemUndo

                text: qsTr("&Undo")

                shortcut: StandardKey.Undo

                enabled: false

                onTriggered: {
                    // TODO: undo
                    console.log("TODO: undo");
                }
            }

            MenuItem {
                id: menuEditItemRedo

                text: qsTr("&Redo")

                shortcut: StandardKey.Redo

                enabled: false

                onTriggered: {
                    // TODO: redo
                    console.log("TODO: redo");
                }
            }

            MenuSeparator {}

            MenuItem {
                id: menuEditItemCut

                text: qsTr("Cu&t")

                shortcut: StandardKey.Cut

                enabled: false

                onTriggered: {
                    // TODO: cut
                    console.log("TODO: cut");
                }
            }

            MenuItem {
                id: menuEditItemCopy

                text: qsTr("Copy")

                shortcut: StandardKey.Copy

                enabled: false

                onTriggered: {
                    // TODO: copy
                    console.log("TODO: copy");
                }
            }

            MenuItem {
                id: menuEditItemPaste

                text: qsTr("Paste")

                shortcut: StandardKey.Paste

                enabled: false

                onTriggered: {
                    // TODO: paste
                    console.log("TODO: paste");
                }
            }
        }


        // Debug
        Menu {
            id: menuDebug

            title: qsTr("Debug")

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
                text: qsTr("Getting started")

                enabled: false

                onTriggered: {
                     //applicationLoader.item.openGettingStarted(true);
                }
            }

            MenuSeparator {}

            MenuItem {
                text: qsTr("Report bug...")

                onTriggered: {
                    //TODO: open a webpage (bug tracking software) or use a simple mailto ?
                    Qt.openUrlExternally(
                                         "mailto:contact@ingescape.com?subject=Bug report "
                                         + Qt.application.name + " v" + Qt.application.version
                                         + "&body=(write your bug report here)\n\n"
                                         + "----------------------------------\n"
                                         + "System information:\n"
                                         + DebugQuickInspector.systemInformation
                                         );
                }
            }

            MenuItem {
                text: qsTr("System information...")

                onTriggered: systemInformationPopup.open();
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
        if (SHOW_DEBUG_MENU)
        {
            DebugQuickInspector.currentWindow = mainWindow;
        }

        // Start our loader delay animation when our initial content is ready
        loaderDelayAnimation.start();
    }


    // When user clicks on window close button
    onClosing: {
        console.info("QML: Close Window");

        IngeScapeAssessmentsC.processBeforeClosing();
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
                IngeScapeAssessmentsC.forceCreation();

                // Set the directory used to store snasphots of our application
                I2SnapshotHelper.directory = IngeScapeAssessmentsC.snapshotDirectory;

                // Binding to display our application loader
                applicationLoader.visible = Qt.binding(function() {
                    return ((applicationLoader.status === Loader.Ready) && (IngeScapeAssessmentsC.modelManager !== null));
                });

                /*subWindowsInstantiator.model = Qt.binding(function() {
                    return IngeScapeAssessmentsC.openedWindows;
                });*/

                // Load our QML UI
                applicationLoader.source = "IngeScapeAssessments.qml";
            }
        }


        // Overlay layer used to display popups above the content of our window
        I2Layer {
            id: overlayLayer

            objectName: "overlayLayer"

            anchors.fill: parent
        }

        // Second overlay layer used to display popups above the content of another popup
        I2Layer {
            id: overlay2Layer

            objectName: "overlay2Layer"

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


        //----------------------------------
        //
        // Popups
        //
        //----------------------------------

        // System information
        SystemInformationPopup {
            id: systemInformationPopup
        }


        // Save before quit popup
        /*Popups.SaveBeforeQuitPopup {
            id: saveBeforeQuitPopup

            anchors.centerIn: parent

            // Save the changes to the currently opened platform and quit
            onSaveAs: {
                IngeScapeEditorC.selectFileToSavePlatform();

                mainWindow.forceClose = true;
                mainWindow.close();
            }

            // Discard all unsaved changes and quit
            onDiscard: {
                mainWindow.forceClose = true;
                mainWindow.close();
            }

            // Cancel the closing procedure and keep the editor open
            onCancel: {}
        }*/
    }
}
