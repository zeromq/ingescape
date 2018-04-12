import QtQuick 2.9
import QtQuick.Window 2.2
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4
import QtQuick.Dialogs 1.2

import MasticPlayground 1.0


ApplicationWindow {
    id: mainWindow

    //----------------------------------
    //
    // Properties
    //
    //----------------------------------

    property url editedFile


    visible: true

    // Minimum size
    minimumWidth: 720
    minimumHeight: 480

    // Default width
    width: 1024
    height: 768


    title: qsTr("MASTIC Playground - %1").arg(mainWindow.editedFile)


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
        // File menu
        Menu {
            id: fileMenu

            title: qsTr("&File")

            MenuItem {
                text: qsTr("&New")

                shortcut: StandardKey.New

                onTriggered: {
                    MasticPlaygroundController.newFile();
                }
            }

            MenuItem {
                text: qsTr("&Open")

                shortcut: StandardKey.Open

                onTriggered: {
                    openFileDialog.open();
                }
            }

            MenuSeparator {
            }

            MenuItem {
                text: qsTr("&Save")

                shortcut: StandardKey.Save

                enabled: !autoSaveMenuItem.checked

                onTriggered: {
                    MasticPlaygroundController.saveCurrentFile();
                }
            }

            MenuItem {
                text: qsTr("Save as")

                shortcut: StandardKey.SaveAs

                onTriggered: {
                    saveAsFileDialog.open();
                }
            }

            MenuItem {
                id: autoSaveMenuItem

                text: qsTr("Automatically save changes")

                checkable: true

                onTriggered: {
                    MasticPlaygroundController.autoSave = checked;
                }
            }

            MenuSeparator {
            }

            MenuSeparator {
                 visible: (recentFilesMenuRepeater.count > 0)
            }

            MenuItem {
                text: qsTr("E&xit")

                shortcut: StandardKey.Quit

                onTriggered: Qt.quit();
            }
        }

        // Examples meu
        Menu {
            id: examplesMenu

            title: qsTr("Examples")
        }


        // View menu
        Menu {
            id: viewMenu

            title: qsTr("&View")

            ExclusiveGroup {
                id: viewModesGroup
            }
        }
    }


    // Repeater used to add recent files
    Repeater {
        id: recentFilesMenuRepeater

        model: null

        delegate: Item {
            property var menuItem: menuItem

            MenuItem {
                id: menuItem

                text: modelData

                onTriggered: {
                    MasticPlaygroundController.openFile(modelData);
                }
            }
        }

        onItemAdded: {
            fileMenu.insertItem(7 + index, item.menuItem)
        }

        onItemRemoved: fileMenu.removeItem(item.menuItem)
    }


    // Repeater used to add examples
    Repeater {
        id: examplesMenuRepeater

        model: null

        delegate: Item {
            property var menuItem: exampleMenuItem

            MenuItem {
                id: exampleMenuItem

                text: model.description

                onTriggered: {
                    MasticPlaygroundController.openExample(model.file);
                }
            }
        }

        onItemAdded: {
            examplesMenu.insertItem(index, item.menuItem)
        }

        onItemRemoved: examplesMenu.removeItem(item.menuItem)
    }


    // Repeater used to add view modes
    Repeater {
        id: viewModesMenuRepeater

        model: null

        delegate: Item {
            property var menuItem: viewMenuItem

            MenuItem {
                id: viewMenuItem

                exclusiveGroup: viewModesGroup

                checkable: true
                checked: (modelData === MasticPlaygroundController.currentViewMode)

                text: modelData

                onTriggered: {
                    MasticPlaygroundController.currentViewMode = modelData;
                }
            }
        }

        onItemAdded: {
            viewMenu.insertItem(index, item.menuItem)
        }

        onItemRemoved: viewMenu.removeItem(item.menuItem)
    }



    //----------------------------------
    //
    // Behavior
    //
    //----------------------------------

    Component.onCompleted: {
        // Start our loader delay animation when our initial content is ready
        loaderDelayAnimation.start();

    }


    Connections {
        id: connectionToPlaygroundSignals

        ignoreUnknownSignals: true

        onUserMustDefineSaveFile: {
            saveAsFileDialog.openViaSignalUserMustDefineSaveFile();
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

            visible: (applicationLoader.status === Loader.Ready)
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

            duration: 50

            onStopped: {
                // Init controller
                MasticPlaygroundController.forceCreation();

                // Update connections
                connectionToPlaygroundSignals.target = MasticPlaygroundController;

                // Update edited file
                mainWindow.editedFile = Qt.binding(function() { return MasticPlaygroundController.currentSourceFile; });

                // Update our menus
                recentFilesMenuRepeater.model = Qt.binding(function() { return MasticPlaygroundController.recentFiles; });
                examplesMenuRepeater.model = MasticPlaygroundController.examples;
                viewModesMenuRepeater.model = Qt.binding(function() { return MasticPlaygroundController.viewModes; });
                autoSaveMenuItem.checked = Qt.binding(function() { return MasticPlaygroundController.autoSave; });

                // Load our QML UI
                applicationLoader.source = "Playground.qml";
            }
        }

    }


    //----------------------------------
    //
    // Popups
    //
    //----------------------------------


    //
    // Popup to open a new QML file
    //
    FileDialog {
        id: openFileDialog

        title: qsTr("Please select a QML file")

        selectMultiple: false
        selectFolder: false
        selectExisting: true

        nameFilters: [ "QML files (*.qml)" ]
        selectedNameFilter: "QML files (*.qml)"

        onAccepted: {
            if (fileUrls.length > 0)
            {
                MasticPlaygroundController.openFile(fileUrls[0]);
            }
        }

        onRejected: {
        }
    }


    //
    // Popup to save our current QML file into another file
    //
    FileDialog {
        id: saveAsFileDialog

        // Flag indicating if our menu has opened by a userMustDefineSaveFile signal
        property bool _openedViaSignalUserMustDefineSaveFile: false;

        function openViaSignalUserMustDefineSaveFile()
        {
            _openedViaSignalUserMustDefineSaveFile = true;
            open();
        }


        title: qsTr("Save QML file")

        selectMultiple: false
        selectFolder: false
        selectExisting: false

        nameFilters: [ "QML files (*.qml)" ]
        selectedNameFilter: "QML files (*.qml)"

        onAccepted: {
            if (fileUrls.length > 0)
            {
                if ( _openedViaSignalUserMustDefineSaveFile)
                {
                    MasticPlaygroundController.setRequiredFileToSave(fileUrls[0]);
                }
                else
                {
                    MasticPlaygroundController.saveCurrentFileAs(fileUrls[0]);
                }
            }

            _openedViaSignalUserMustDefineSaveFile = false;
        }

        onRejected: {
            _openedViaSignalUserMustDefineSaveFile = false;
        }
    }




}
