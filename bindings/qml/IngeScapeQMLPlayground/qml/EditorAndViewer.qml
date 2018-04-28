import QtQuick 2.9
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4

import "editor" as Editor
import "viewer" as Viewer

import IngeScapePlayground 1.0



Item {
    id: root

    //-------------------------------------------
    //
    // Properties
    //
    //-------------------------------------------

    state: PlaygroundController.currentViewMode


    //-------------------------------------------
    //
    // Signals
    //
    //-------------------------------------------

    // Triggered when our live view has loaded its content
    signal liveViewLoaded();


    // Triggered when our live view has failed to load its content
    signal liveViewError();


    //-------------------------------------------
    //
    // Content
    //
    //-------------------------------------------


    //
    // Live view i.e. rendering of our demo
    //
    Viewer.LiveView {
        id: demoLiveView

        anchors {
            top: parent.top
            bottom: parent.bottom
            right: parent.right
        }

        width: root.width/2

        visible: ((opacity !== 0) && (width !== 0))

        clip: (visible && demoCodeView.visible)

        onContentLoaded: {
            root.liveViewLoaded();
        }

        onContentError: {
            root.liveViewError();
        }

        Connections {
            target: PlaygroundController

            onClearLiveView: {
                // Just in case, our user opens a file with the same content
                // => it will force "sourceCodeChanged" when we set sourceCode in onReloadLiveView
                demoLiveView.sourceCode = "";
            }

            onReloadLiveView: {
                console.log("Reload live view")
                demoLiveView.sourceCodeFilePath = PlaygroundController.currentSourceFile;
                demoLiveView.sourceCode = PlaygroundController.editedSourceContent;
            }
        }
    }



    //
    // Editor view i.e. source code of our demo
    //
    Editor.CodeView {
        id: demoCodeView

        anchors {
            top: parent.top
            bottom: parent.bottom
            left: parent.left
        }

        width: root.width/2

        visible: ((opacity !== 0) && (width !== 0))

        sourceCode: PlaygroundController.editedSourceContent

        onSourceCodeChanged: {
            PlaygroundController.editedSourceContent = sourceCode;
         }

        Connections {
            target: PlaygroundController

            onReloadLiveView: {
                demoCodeView.sourceCode = Qt.binding(function() {return  PlaygroundController.editedSourceContent; });
            }
        }
    }


    //-------------------------------------------
    //
    // States
    //
    //-------------------------------------------

    states: [
        State {
            name: PlaygroundController.viewModeBoth

            PropertyChanges {
                target: demoCodeView
                width: root.width / 2
            }

            PropertyChanges {
                target: demoLiveView
                width: root.width / 2
            }
        },

        State {
            name: PlaygroundController.viewModeCodeOnly

            PropertyChanges {
                target: demoCodeView
                width: root.width
            }

            PropertyChanges {
                target: demoLiveView
                width: 0
            }
        },

        State {
            name: PlaygroundController.viewModeViewerOnly

            PropertyChanges {
                target: demoCodeView
                width: 0
            }

            PropertyChanges {
                target: demoLiveView
                width: root.width
            }
        }
    ]

    transitions: [
        Transition {
            to: "*"

            NumberAnimation {
                target: demoCodeView
                properties: "width"
                duration: 300
                easing.type: Easing.InOutQuad
            }

            NumberAnimation {
                target: demoLiveView
                properties: "width"
                duration: 300
                easing.type: Easing.InOutQuad
            }

        }
    ]
}
