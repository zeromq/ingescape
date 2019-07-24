import QtQuick 2.8
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4

import I2Quick 1.0

import INGESCAPE 1.0


I2PopupBase {
    id: root


    //--------------------------------------------------------
    //
    // Properties
    //
    //--------------------------------------------------------

    anchors.centerIn: parent

    width: content.width
    height: content.height

    dismissOnOutsideTap: true


    property string _qtCompilationVersion: ""
    property string _qtRuntimeVersion: ""
    property string _qtQuickSceneGraphBackend: ""
    property string _qtQuickRenderLoop: ""

    property bool _useOpenGL: false
    property string _openGLVendor: ""
    property string _openGLRenderer: ""
    property string _openGLVersion: ""


    //--------------------------------------------------------
    //
    // Behavior
    //
    //--------------------------------------------------------

    onOpened: {
        // NB: We set values here instead of using bindings to avoid useless updates when our application is loading
        root._qtCompilationVersion = DebugQuickInspector.qtCompilationVersion;
        root._qtRuntimeVersion = DebugQuickInspector.qtRuntimeVersion;
        root._qtQuickSceneGraphBackend = DebugQuickInspector.sceneGraphBackend;
        root._qtQuickRenderLoop = DebugQuickInspector.renderLoop;

        root._useOpenGL = DebugQuickInspector.useOpenGL;
        root._openGLVendor = DebugQuickInspector.openGLVendor;
        root._openGLRenderer = DebugQuickInspector.openGLRenderer;
        root._openGLVersion = DebugQuickInspector.openGLVersion;
    }


    //--------------------------------------------------------
    //
    // Content
    //
    //--------------------------------------------------------

    Rectangle {
        id: content

        width: 500
        height: childrenRect.y * 2 + childrenRect.height

        radius: 5

        color: IngeScapeTheme.veryDarkGreyColor

        border {
            width: 2
            color: IngeScapeTheme.editorsBackgroundBorderColor
        }

        Column {
            anchors {
                top: parent.top
                topMargin: 20
                left: parent.left
                leftMargin: 20
                right: parent.right
                rightMargin: 20
            }

            spacing: 20

            Text {
                id: title

                anchors {
                    left: parent.left
                    right: parent.right
                }

                text: qsTr("Qt Quick infos")

                color: IngeScapeTheme.whiteColor

                font {
                    family: IngeScapeTheme.textFontFamily
                    weight: Font.Medium
                    pixelSize: 23
                }
            }

            Text {
                id: infos

                anchors {
                    left: parent.left
                    right: parent.right
                }

                wrapMode: Text.WordWrap

                color: IngeScapeTheme.whiteColor

                font {
                    family: IngeScapeTheme.textFontFamily
                    weight: Font.Medium
                    pixelSize: 16
                }

                text: qsTr("Built with Qt %1\n").arg(root._qtCompilationVersion)
                     + qsTr("Using Qt %1\n").arg(root._qtRuntimeVersion)
                     + "\n"
                     + qsTr("Render loop: %1\n").arg(root._qtQuickRenderLoop)
                     + "\n"
                     + qsTr("Scene graph backend: %1\n").arg(root._qtQuickSceneGraphBackend)
                     + (
                        root._useOpenGL
                        ?   qsTr("- Vendor: %1\n").arg(root._openGLVendor)
                          + qsTr("- Renderer: %1\n").arg(root._openGLRenderer)
                          + qsTr("- Version: %1\n").arg(root._openGLVersion)
                        : ""
                        )

            }


            Button {
                id: closeButton

                property var boundingBox: IngeScapeTheme.svgFileIngeScape.boundsOnElement("button");

                height: boundingBox.height
                width: boundingBox.width

                activeFocusOnPress: true
                text: qsTr("OK")

                anchors {
                    horizontalCenter: parent.horizontalCenter
                }

                style: I2SvgButtonStyle {
                    fileCache: IngeScapeTheme.svgFileIngeScape

                    pressedID: releasedID + "-pressed"
                    releasedID: "button"
                    disabledID: releasedID + "-disabled"

                    font {
                        family: IngeScapeTheme.textFontFamily
                        weight : Font.Medium
                        pixelSize : 16
                    }

                    labelColorPressed: IngeScapeTheme.blackColor
                    labelColorReleased: IngeScapeTheme.whiteColor
                    labelColorDisabled: IngeScapeTheme.greyColor
                }

                onClicked: {
                    root.close();
                }
            }
        }
    }

}
