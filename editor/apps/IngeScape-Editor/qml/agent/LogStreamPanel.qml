/*
 *	IngeScape Editor
 *
 *  Copyright © 2017 Ingenuity i/o. All rights reserved.
 *
 *	See license terms for the rights and conditions
 *	defined by copyright holders.
 *
 *
 *	Contributors:
 *      Vincent Peyruqueou <peyruqueou@ingenuity.io>
 *
 */

import QtQuick 2.8
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4
import QtQuick.Window 2.3
import I2Quick 1.0

import INGESCAPE 1.0

import "../theme" as Theme;


Window {
    id: rootItem

    height: minimumHeight
    width: minimumWidth

    minimumWidth: 640
    minimumHeight: 545


    flags: Qt.Dialog

    //    isModal: false
    //    dismissOnOutsideTap : false;
    //    keepRelativePositionToInitialParent : false;


    //--------------------------------
    //
    // Properties
    //
    //--------------------------------

    property var controller: null;


    // List of widths
    property var widthsOfColumns: [
        140,
        60,
        400
    ]


    //--------------------------------
    //
    // Signals
    //
    //--------------------------------

    // Emitted when user pressed our popup
    // signal bringToFront();


    // Emitted when "All Agents" is selected or unselected
    //signal clickAllAgents();


    //--------------------------------
    //
    // Content
    //
    //--------------------------------

    Rectangle {

        anchors {
            fill: parent
        }
        radius: 5
        border {
            width: 2
            color: IngeScapeTheme.editorsBackgroundBorderColor
        }
        color: IngeScapeTheme.editorsBackgroundColor


        Button {
            id: btnClose

            anchors {
                verticalCenter: titleItem.verticalCenter
                right : parent.right
                rightMargin: 20
            }

            activeFocusOnPress: true
            style: Theme.LabellessSvgButtonStyle {
                fileCache: IngeScapeTheme.svgFileINGESCAPE

                pressedID: releasedID + "-pressed"
                releasedID: "closeEditor"
                disabledID : releasedID
            }

            onClicked: {
                // Close our popup
                rootItem.close();
            }
        }


        // Title
        Item {
            id : titleItem

            anchors {
                top : parent.top
                topMargin: 15
                left : parent.left
                leftMargin: 18
                right : parent.right
                rightMargin: 18
            }

            height : titleTxt.height


            Text {
                id : titleTxt

                anchors {
                    left : parent.left
                }

                text: (controller ? qsTr("Log Stream of %1").arg(controller.agentName) : qsTr("Log Stream"))

                elide: Text.ElideRight
                color: IngeScapeTheme.whiteColor
                font {
                    family: IngeScapeTheme.textFontFamily
                    pixelSize: 23
                    weight: Font.Medium
                }
            }
        }


        // Logs List
        Item {
            id : tableLogs

            anchors {
                top: parent.top
                topMargin: 100
                left: parent.left
                leftMargin: 20
                right: parent.right
                rightMargin: 20
                bottom: parent.bottom
                bottomMargin: 55
            }

            /// ****** Headers of columns ***** ////
            Row {
                id: tableauHeaderRow

                anchors {
                    left: parent.left
                    right: parent.right
                    top: parent.top
                    topMargin: 15
                }
                height : 33

                Repeater {
                    model: [
                        qsTr("Date"),
                        qsTr("Type"),
                        qsTr("Log"),
                    ]

                    Item {
                        height : 33
                        width : rootItem.widthsOfColumns[index]

                        Text {
                            anchors {
                                left: parent.left
                                leftMargin: 2
                                verticalCenter: parent.verticalCenter
                            }

                            text : modelData

                            color : IngeScapeTheme.lightGreyColor
                            font {
                                family: IngeScapeTheme.textFontFamily
                                pixelSize : 16
                            }
                        }
                    }
                }
            }


            // separator
            Rectangle {
                anchors {
                    left : parent.left
                    right : parent.right
                    top : tableauHeaderRow.bottom
                }
                height : 1

                color : IngeScapeTheme.blackColor
            }

            /// ****** List ***** ////
            ScrollView {
                id : scrollView

                anchors {
                    top: tableauHeaderRow.bottom
                    left : parent.left
                    right : parent.right
                    bottom : parent.bottom
                }

                style: IngeScapeScrollViewStyle {
                }

                // Prevent drag overshoot on Windows
                flickableItem.boundsBehavior: Flickable.OvershootBounds

                // Content of our scrollview
                ListView {
                    model: if (controller) {
                               controller.logs
                           }
                           else {
                               0
                           }

                    delegate: Item {
                        anchors {
                            left : parent.left
                            right : parent.right
                        }
                        height : 30

                        Row {
                            id: listLine

                            anchors {
                                fill: parent
                                leftMargin: 2
                            }

                            // Date and time
                            Text {
                                text: model.logDateTime.toLocaleString(Qt.locale(),"dd/MM hh:mm:ss.zzz")

                                anchors {
                                    verticalCenter: parent.verticalCenter
                                }
                                verticalAlignment: Text.AlignVCenter
                                width : rootItem.widthsOfColumns[0]
                                height: parent.height

                                //elide: Text.ElideRight
                                color: IngeScapeTheme.whiteColor
                                font {
                                    family: IngeScapeTheme.textFontFamily
                                    pixelSize: 14
                                }
                            }

                            // Type
                            Text {
                                text: model.logType

                                anchors {
                                    verticalCenter: parent.verticalCenter
                                }
                                verticalAlignment: Text.AlignVCenter

                                width : rootItem.widthsOfColumns[1]
                                height: parent.height

                                //elide: Text.ElideRight
                                color: IngeScapeTheme.whiteColor
                                font {
                                    family: IngeScapeTheme.textFontFamily
                                    pixelSize: 14
                                }
                            }

                            // Content
                            Text {
                                text: model.logContent

                                anchors {
                                    verticalCenter: parent.verticalCenter
                                }
                                verticalAlignment: Text.AlignVCenter

                                width : rootItem.widthsOfColumns[2]
                                height: parent.height

                                elide: Text.ElideRight
                                color: IngeScapeTheme.whiteColor
                                font {
                                    family: IngeScapeTheme.textFontFamily
                                    pixelSize: 14
                                }
                            }

                        }

                        // separator
                        Rectangle {
                            anchors {
                                left : parent.left
                                right : parent.right
                                bottom : parent.bottom
                            }
                            height : 1

                            color : IngeScapeTheme.blackColor
                        }

                    }
                }
            }
        }



        Button {
            id: okButton

            property var boundingBox: IngeScapeTheme.svgFileINGESCAPE.boundsOnElement("button");
            height : boundingBox.height
            width :  boundingBox.width

            activeFocusOnPress: true
            text : "OK"

            anchors {
                right : parent.right
                rightMargin: 16
                bottom : parent.bottom
                bottomMargin: 16
            }

            style: I2SvgButtonStyle {
                fileCache: IngeScapeTheme.svgFileINGESCAPE

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
                labelColorDisabled: IngeScapeTheme.whiteColor
            }

            onClicked: {
                // Close our popup
                rootItem.close();
            }
        }
    }
}
