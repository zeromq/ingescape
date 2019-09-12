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


    property string _systemInformation: ""


    //--------------------------------------------------------
    //
    // Behavior
    //
    //--------------------------------------------------------

    onOpened: {
        // NB: We set values here instead of using bindings to avoid useless updates when our application is loading
        root._systemInformation = Qt.binding(function() {
            return DebugQuickInspector.systemInformation;
        });
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

                text: qsTr("System information")

                color: IngeScapeTheme.whiteColor

                font {
                    family: IngeScapeTheme.textFontFamily
                    weight: Font.Medium
                    pixelSize: 23
                }
            }

            // Introduction
            Text {
                id: introduction

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

                text: qsTr("Use the following to provide more detailed information about your system to bug reports:")
            }


            // Infos
            TextArea {
                id: infos

                anchors {
                    left: parent.left
                    right: parent.right
                }

                height: 250

                readOnly: true

                text: root._systemInformation

                style: TextAreaStyle {
                    font {
                        family: IngeScapeTheme.textFontFamily
                        weight: Font.Medium
                        pixelSize: 16
                    }

                    textColor: IngeScapeTheme.whiteColor
                    selectedTextColor: IngeScapeTheme.blackColor
                    selectionColor: IngeScapeTheme.orangeColor

                    backgroundColor: content.color

                    padding {
                        top: 10
                        bottom: 10
                        left: 10
                        right: 14
                    }

                    frame: Rectangle {
                        color: "transparent"

                        radius: 5

                        border {
                            color: IngeScapeTheme.editorsBackgroundBorderColor
                            width: 2
                        }
                    }

                    handle: Rectangle {
                        implicitWidth: 10
                        implicitHeight: 10

                        radius: Math.min(width/2, height/2)

                        color: IngeScapeTheme.lightGreyColor

                        opacity: 0.8
                    }

                    scrollBarBackground: Item {
                        implicitWidth: 10
                        implicitHeight: 10
                    }

                    incrementControl: Item {
                        implicitWidth: 1
                        implicitHeight: 1
                    }

                    decrementControl: Item {
                        implicitWidth: 1
                        implicitHeight: 1
                    }
                }

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
