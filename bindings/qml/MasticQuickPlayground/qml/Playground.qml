import QtQuick 2.9
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4

import Qt.labs.settings 1.0

import MasticPlayground 1.0


Item {
    id: root

    //-------------------------------------------
    //
    // Properties
    //
    //-------------------------------------------

    readonly property int dropFeedbackWidth: 10
    readonly property color dropFeedbackErrorColor: "#99FF0000"
    readonly property color dropFeedbackOkColor: "#9900FF00"


    // Recent files
    property var recentFiles: MasticPlaygroundController.recentFiles

    // Auto-save
    property bool autoSave: MasticPlaygroundController.autoSave

    // Auto-restart Mastic
    property bool autoRestartMastic: MasticPlaygroundController.autoRestartMastic


    //-------------------------------------------
    //
    // Behavior
    //
    //-------------------------------------------


    onRecentFilesChanged: {
        MasticPlaygroundController.recentFiles = root.recentFiles;
    }


    onAutoSaveChanged: {
        MasticPlaygroundController.autoSave = root.autoSave;
    }

    onAutoRestartMasticChanged: {
        MasticPlaygroundController.autoRestartMastic = root.autoRestartMastic;
    }


    Component.onCompleted: {
        MasticPlaygroundController.init();
    }


    //-------------------------------------------
    //
    // Content
    //
    //-------------------------------------------


    //
    // Drop area to capture external files dropped above our application
    //
    DropArea {
        anchors.fill: parent

        keys: ["text/uri-list"]

        onEntered: {
            var isValid = (drag.hasText && drag.text.toString().match(".qml$"));
            dropFeedback.border.color = (isValid) ? root.dropFeedbackOkColor : root.dropFeedbackErrorColor
            dropFeedback.border.width = (isValid) ? root.dropFeedbackWidth : 0;
        }

        onExited: {
             dropFeedback.border.width = 0;
        }

        onDropped: {
            dropFeedback.border.width = 0;

            if (drop.hasText && drop.text.toString().match(".qml$"))
            {
                if ((drop.proposedAction == Qt.MoveAction) || (drop.proposedAction == Qt.CopyAction))
                {
                    // Open file
                    MasticPlaygroundController.openFile(Qt.resolvedUrl(drop.text));

                    drop.acceptProposedAction()
                }
            }
        }



        //
        // Editor and viewer
        //
        EditorAndViewer {
            id: editorAndViewer

            anchors {
                top: toolbar.bottom
                bottom: parent.bottom

                left: parent.left
                right: parent.right
            }

            onLiveViewLoaded: {
                toolbar.autoRestartMasticIfNeeded();
            }
        }



        //
        // Toolbar
        //
        PlaygroundToolbar {
            id: toolbar

            anchors {
                top: parent.top
                left: parent.left
                right: parent.right
            }
        }



        //
        // Drop feedback - used when we drag-n-drop an external file above our application
        //
        Rectangle {
            id: dropFeedback

            anchors.fill: parent

            color: "transparent"

            border {
                color: root.dropFeedbackOkColor
                width: 0
            }

            Behavior on border.width {
                NumberAnimation {}
            }
        }
    }






    //-------------------------------------------
    //
    // Settings
    //
    //-------------------------------------------

    Settings {
        category: "Playground"

        property alias recentFiles: root.recentFiles

        property alias autoSave: root.autoSave

        property alias autoRestartMastic: root.autoRestartMastic
    }
}
