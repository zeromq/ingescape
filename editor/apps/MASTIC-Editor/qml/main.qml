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
 *      Vincent Peyruqueou <peyruqueou@ingenuity.io>
 *      Alexandre Lemort   <lemort@ingenuity.io>
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

    color: MasticTheme.greyWindowBgColor



    //----------------------------------
    //
    // Menu
    //
    //----------------------------------

    menuBar: MenuBar {
        Menu {
            title: qsTr("Supervision")

            MenuItem {
                text: qsTr("TODO 1")
                onTriggered: {
                    console.log("Supervision TODO 1");
                }
            }
            MenuItem {
                text: qsTr("TODO 2")
                onTriggered: {
                    console.log("Supervision TODO 2");
                }
            }
        }

        Menu {
            title: qsTr("Mapping")

            MenuItem {
                text: qsTr("Créer un nouveau mapping")
                onTriggered: {
                    console.log("Créer un nouveau mapping");
                }
            }
            MenuItem {
                text: qsTr("Importer un mapping")
                onTriggered: {
                    console.log("Importer un mapping");
                }
            }
            MenuItem {
                text: qsTr("Exporter le mapping")
                onTriggered: {
                    console.log("Exporter le mapping");
                }
            }
            MenuItem {
                text: qsTr("Exécuter le mapping")
                onTriggered: {
                    console.log("Exécuter le mapping");
                }
            }

            MenuSeparator {
            }

            MenuItem {
                text: qsTr("Historique")
                onTriggered: {
                    console.log("Historique");
                }
            }
        }

        Menu {
            title: qsTr("Scénario")

            MenuItem {
                text: qsTr("TODO 1")
                onTriggered: {
                    console.log("Scénario TODO 1");
                }
            }
            MenuItem {
                text: qsTr("TODO 2")
                onTriggered: {
                    console.log("Scénario TODO 2");
                }
            }
        }

        Menu {
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
        }

        Menu {
            title: qsTr("Paramètres")

            MenuItem {
                text: qsTr("Prendre une capture d'écran")
                onTriggered: {
                    I2SnapshotHelper.saveWindowOfItem(content, Qt.size(0,0), "MASTIC");
                }
            }

            MenuSeparator {
            }

            MenuItem {
                text: qsTr("Fermer l'application")
                onTriggered: {
                    console.info("QML: close application");
                    Qt.quit();
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
    }
}
