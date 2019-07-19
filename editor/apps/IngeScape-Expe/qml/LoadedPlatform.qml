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

        Item {
            anchors {
                left: parent.left
                right: parent.right
            }
            height: 30

            Text {
                id: txtPlatform

                text: currentLoadedPlatform ? "Loaded platform: " + currentLoadedPlatform.name
                                            : "No loaded platform"

                height: 30

                verticalAlignment: Text.AlignVCenter

                color: IngeScapeTheme.whiteColor
                font {
                    family: IngeScapeTheme.textFontFamily
                    weight: Font.Medium
                    pixelSize: 16
                }
            }

            TextField {
                id: txtLoadedPlatformPath

                anchors {
                    top: parent.top
                    bottom: parent.bottom
                    left: txtPlatform.right
                    leftMargin: 10
                    right: parent.right
                }

                text: currentLoadedPlatform ? currentLoadedPlatform.filePath : ""

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
        }

        Text {
            text: IngeScapeExpeC.modelManager ? IngeScapeExpeC.modelManager.allAgentsGroupsByName.count + " agents"
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

        Column {
            id: columnAgents

            Repeater {
                model: IngeScapeExpeC.modelManager ? IngeScapeExpeC.modelManager.allAgentsGroupsByName : null

                delegate: Rectangle {
                    id: agentItem

                    property var isONandAlone: model && model.isON && (model.numberOfAgentsON === 1)

                    width: scrollView.width
                    height: 36

                    //color: (model && model.isON) ? IngeScapeTheme.orangeColor : "transparent"
                    color: (model && model.isON) ? "transparent" : "darkgray"

                    border {
                        //color: "darkgray"
                        color: agentItem.isONandAlone ? "darkgray" : IngeScapeTheme.redColor
                        width: agentItem.isONandAlone ? 1 : 2
                    }

                    Label {

                        anchors {
                            left: parent.left
                            leftMargin: 5
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

                        //visible: model ? (model.numberOfAgentsON > 1) : false

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
    }

}
