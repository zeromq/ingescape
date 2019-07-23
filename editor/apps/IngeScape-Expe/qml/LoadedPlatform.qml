/*
 *	IngeScape Expe
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
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4
import QtQuick.Dialogs 1.2

import I2Quick 1.0

import INGESCAPE 1.0

Item {

    id: rootItem


    //--------------------------------------------------------
    //
    //
    // Properties
    //
    //
    //--------------------------------------------------------


    property PlatformM currentLoadedPlatform: IngeScapeExpeC.modelManager ? IngeScapeExpeC.modelManager.currentLoadedPlatform : null;


    //--------------------------------------------------------
    //
    //
    // Content
    //
    //
    //--------------------------------------------------------

    Rectangle {
        id: bg
        anchors.fill: parent
        color: "transparent"
        radius: 5

        border {
            color: "darkgray"
            width: 2
        }
    }

    Column {
        id: column

        anchors {
            left: parent.left
            right: parent.right
            top: parent.top
            margins: 10
        }

        spacing: 10

        Text {
            id: titlePlatform

            anchors.horizontalCenter: parent.horizontalCenter
            height: 30

            text: "Current Platform"

            color: IngeScapeTheme.whiteColor
            font {
                family: IngeScapeTheme.textFontFamily
                weight: Font.Bold
                pixelSize: 18
            }
        }

        Row {
            height: 30

            spacing: 10

            Text {

                text: currentLoadedPlatform ? "Loaded platform:"
                                            : "No loaded platform"

                height: 30

                verticalAlignment: Text.AlignVCenter

                color: IngeScapeTheme.whiteColor
                font {
                    family: IngeScapeTheme.textFontFamily
                    //weight: Font.Medium
                    pixelSize: 16
                }
            }

            Text {

                text: currentLoadedPlatform ? currentLoadedPlatform.name : ""

                height: 30

                verticalAlignment: Text.AlignVCenter

                color: IngeScapeTheme.whiteColor
                font {
                    family: IngeScapeTheme.textFontFamily
                    weight: Font.Medium
                    pixelSize: 18
                }
            }
        }

        TextField {
            id: txtLoadedPlatformPath

            text: currentLoadedPlatform ? currentLoadedPlatform.filePath : ""

            anchors {
                left: parent.left
                right: parent.right
            }
            height: 30

            style: I2TextFieldStyle {
                backgroundColor: IngeScapeTheme.darkBlueGreyColor
                borderColor: IngeScapeTheme.whiteColor
                borderErrorColor: IngeScapeTheme.redColor
                radiusTextBox: 1
                borderWidth: 0;
                borderWidthActive: 1
                textIdleColor: IngeScapeTheme.whiteColor;
                textDisabledColor: IngeScapeTheme.darkGreyColor

                padding.left: 3
                padding.right: 3

                font {
                    pixelSize:15
                    family: IngeScapeTheme.textFontFamily
                }
            }
        }

        Text {
            text: IngeScapeExpeC.modelManager ? IngeScapeExpeC.modelManager.filteredPlatformAgents.count + " agents"
                                              : ""

            height: 30

            verticalAlignment: Text.AlignVCenter

            color: IngeScapeTheme.whiteColor
            font {
                family: IngeScapeTheme.textFontFamily
                weight: Font.Medium
                pixelSize: 16
            }
        }
    }


    ScrollView {
        id: scrollView

        anchors {
            top: column.bottom
            bottom: parent.bottom
            left: parent.left
            right: parent.right
            margins: 10
        }

        /*style: IngeScapeScrollViewStyle {
        }*/

        // Prevent drag overshoot on Windows
        flickableItem.boundsBehavior: Flickable.OvershootBounds


        ListView {
            id: agentsListView

            model: IngeScapeExpeC.modelManager ? IngeScapeExpeC.modelManager.filteredPlatformAgents : null

            delegate: componentAgent


            //
            // Transition animations
            //
            add: Transition {
                NumberAnimation { property: "opacity"; from: 0.0; to: 1.0 }
                NumberAnimation { property: "scale"; from: 0.0; to: 1.0 }
            }

            displaced: Transition {
                NumberAnimation { properties: "x,y"; easing.type: Easing.OutBounce }

                // ensure opacity and scale values return to 1.0
                NumberAnimation { property: "opacity"; to: 1.0 }
                NumberAnimation { property: "scale"; to: 1.0 }
            }

            move: Transition {
                NumberAnimation { properties: "x,y"; easing.type: Easing.OutBounce }

                // ensure opacity and scale values return to 1.0
                NumberAnimation { property: "opacity"; to: 1.0 }
                NumberAnimation { property: "scale"; to: 1.0 }
            }

            remove: Transition {
                // ensure opacity and scale values return to 0.0
                NumberAnimation { property: "opacity"; to: 0.0 }
                NumberAnimation { property: "scale"; to: 0.0 }
            }

        }
    }


    //
    // Component Agent
    //
    Component {
        id: componentAgent

        Rectangle {
            id: agentBackground

            property var isONandAlone: model && model.isON && (model.numberOfAgentsON === 1)

            width: scrollView.width
            height: 36

            color: (model && model.isON) ? "transparent" : IngeScapeTheme.darkGreyColor

            border {
                color: agentBackground.isONandAlone ? "darkgray" : IngeScapeTheme.redColor
                width: agentBackground.isONandAlone ? 1 : 3
            }

            Label {

                anchors {
                    left: parent.left
                    leftMargin: 10
                    verticalCenter: parent.verticalCenter
                }

                text: model ? model.name : ""

                color: IngeScapeTheme.whiteColor
                font {
                    family: IngeScapeTheme.textFontFamily
                    //weight: Font.Medium
                    pixelSize: 16
                }
            }

            Rectangle {
                id: warningManyAgentsWithSameName

                anchors {
                    right: parent.right
                    rightMargin: 5
                    verticalCenter: parent.verticalCenter
                }
                height: 20
                width: height
                radius: height / 2

                color: model && (model.numberOfAgentsON === 1) ? "transparent" : IngeScapeTheme.redColor

                Text {
                    anchors.centerIn: parent

                    text: model ? model.numberOfAgentsON : ""

                    color: IngeScapeTheme.whiteColor

                    font {
                        family: IngeScapeTheme.labelFontFamily
                        weight: Font.Black
                        pixelSize: 13
                    }
                }
            }

        }
    }
}
