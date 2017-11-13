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
 *      Justine Limoges <limoges@ingenuity.io>
 */

import QtQuick 2.8
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4

import I2Quick 1.0

import MASTIC 1.0

import "../theme" as Theme;


I2PopupBase {
    id: rootItem

    width: 600
    height: 545

    isModal: false
    dismissOnOutsideTap : false;
    keepRelativePositionToInitialParent : false;


    //--------------------------------
    //
    // Properties
    //
    //--------------------------------

    property var controller : MasticEditorC.valuesHistoryC

    // Columns with a fixed size
    // - Agent
    property int widthColumnAgent: 125
    // - Type
    property int widthColumnType: 110
    // - Definition Value
    property int widthColumnValue: 140
    // - Mapping Value
    property int widthColumnDate: 100

    // Resizable columns
    // - Name
    property int widthColumnName: (tableHistory.width - widthColumnType - widthColumnAgent - widthColumnValue - widthColumnDate)

    // List of widths
    property var widthsOfColumns: [
        widthColumnName,
        widthColumnAgent,
        widthColumnType,
        widthColumnValue,
        widthColumnDate
    ]

    //--------------------------------
    //
    // Signals
    //
    //--------------------------------

    // Emitted when user pressed our popup
    signal bringToFront();


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
            color: MasticTheme.editorsBackgroundBorderColor
        }
        color: MasticTheme.editorsBackgroundColor


        MouseArea {
            id : dragMouseArea
            hoverEnabled: true
            anchors.fill: parent
            drag.target: rootItem

            /*drag.minimumX : - faisceauEditor.width/2
            drag.maximumX : PGIMTheme.applicationWidth - faisceauEditor.width/2
            drag.minimumY : 0
            drag.maximumY : PGIMTheme.applicationHeight -  (dragButton.height + 30)*/

            onPressed: {
                // Emit signal "bring to front"
                rootItem.bringToFront();
            }
        }

        Button {
            id: btnClose

            anchors {
                verticalCenter: titleItem.verticalCenter
                right : parent.right
                rightMargin: 20
            }

            activeFocusOnPress: true
            style: Theme.LabellessSvgButtonStyle {
                fileCache: MasticTheme.svgFileMASTIC

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

                text : "Outputs history"
                elide: Text.ElideRight
                color: MasticTheme.definitionEditorsLabelColor
                font {
                    family: MasticTheme.textFontFamily
                    pixelSize: 23
                    weight: Font.Medium
                }
            }
        }


        Item {
            id : tableHistory

            anchors {
                top: parent.top
                topMargin: 90
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
                        qsTr("Name"),
                        qsTr("Agent"),
                        qsTr("Type"),
                        qsTr("Value"),
                        qsTr("Date")
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

                            color : MasticTheme.definitionEditorsAgentDescriptionColor
                            font {
                                family: MasticTheme.textFontFamily
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

                color : MasticTheme.blackColor
            }

            /// ****** List ***** ////
            ListView {
                anchors {
                    top: tableauHeaderRow.bottom
                    left : parent.left
                    right : parent.right
                    bottom : parent.bottom
                }

                model: if (controller) {
                           controller.filteredValues
                       }
                       else {
                           0
                       }

                delegate:   Item {
                    anchors {
                        left : parent.left
                        right : parent.right
                    }
                    height : 30

                    Row {
                        id: listLine

                        anchors {
                            fill : parent
                            leftMargin: 2
                        }

                        // IOP Name
                        Text {
                            text: model.iopName

                            anchors {
                                verticalCenter: parent.verticalCenter
                            }
                            verticalAlignment: Text.AlignVCenter
                            width : rootItem.widthsOfColumns[0]
                            elide: Text.ElideRight
                            height: parent.height
                            color: MasticTheme.whiteColor
                            font {
                                family: MasticTheme.textFontFamily
                                pixelSize : 16
                            }
                        }

                        // Agent Name
                        Text {
                            text: model.agentName

                            anchors {
                                verticalCenter: parent.verticalCenter
                            }
                            verticalAlignment: Text.AlignVCenter
                            width : rootItem.widthsOfColumns[1]
                            elide: Text.ElideRight
                            height: parent.height
                            color: MasticTheme.lightGreyColor
                            font {
                                family: MasticTheme.textFontFamily
                                pixelSize : 15
                            }
                        }


                        // Type
                        Item {
                            anchors {
                                verticalCenter: parent.verticalCenter
                            }
                            height: parent.height
                            width : rootItem.widthsOfColumns[2]

                            Rectangle {
                                id : circle
                                anchors {
                                    left : parent.left
                                    verticalCenter: parent.verticalCenter
                                }

                                width : 16
                                height : width
                                radius : width/2

                                color : switch (model.iopValueTypeGroup)
                                        {
                                        case AgentIOPValueTypeGroups.NUMBER:
                                            MasticTheme.orangeColor2
                                            break;
                                        case AgentIOPValueTypeGroups.STRING:
                                            MasticTheme.redColor2
                                            break;
                                        case AgentIOPValueTypeGroups.IMPULSION:
                                            MasticTheme.purpleColor
                                            break;
                                        case AgentIOPValueTypeGroups.DATA:
                                            MasticTheme.greenColor
                                            break;
                                        case AgentIOPValueTypeGroups.MIXED:
                                            MasticTheme.whiteColor
                                            break;
                                        case AgentIOPValueTypeGroups.UNKNOWN:
                                            "#000000"
                                            break;
                                        default:
                                            MasticTheme.whiteColor;
                                            break;
                                        }
                            }

                            Text {
                                text: AgentIOPValueTypes.enumToString(model.iopValueType)

                                anchors {
                                    verticalCenter: circle.verticalCenter
                                    verticalCenterOffset: 1
                                    left : circle.right
                                    leftMargin: 5
                                    right : parent.right
                                }
                                verticalAlignment: Text.AlignVCenter
                                elide: Text.ElideRight
                                height: parent.height
                                color: MasticTheme.whiteColor
                                font {
                                    family: MasticTheme.textFontFamily
                                    pixelSize : 16
                                }
                            }
                        }


                        // Value
                        Text {
                            text: model.displayableValue

                            anchors {
                                verticalCenter: parent.verticalCenter
                            }
                            verticalAlignment: Text.AlignVCenter
                            width : rootItem.widthsOfColumns[3]
                            height: parent.height
                            elide: Text.ElideRight
                            color: MasticTheme.whiteColor
                            font {
                                family: MasticTheme.textFontFamily
                                pixelSize : 16
                            }
                        }


                        // Date
                        Text {
                            text: model.time.toLocaleString(Qt.locale(),"hh:mm:ss.zzz")

                            anchors {
                                verticalCenter: parent.verticalCenter
                            }
                            verticalAlignment: Text.AlignVCenter
                            width : rootItem.widthsOfColumns[4]
                            height: parent.height
                            color: MasticTheme.whiteColor
                            elide: Text.ElideRight

                            font {
                                family: MasticTheme.textFontFamily
                                pixelSize : 16
                            }
                        }

                    }

                    //separator
                    Rectangle {
                        anchors {
                            left : parent.left
                            right : parent.right
                            bottom : parent.bottom
                        }
                        height : 1

                        color : MasticTheme.blackColor
                    }

                }
            }
        }
    }


    Button {
        id: okButton

        property var boundingBox: MasticTheme.svgFileMASTIC.boundsOnElement("button");
        height : boundingBox.height
        width :  boundingBox.width

        enabled : visible
        activeFocusOnPress: true
        text : "OK"

        anchors {
            right : parent.right
            rightMargin: 16
            bottom : parent.bottom
            bottomMargin: 16
        }

        style: I2SvgButtonStyle {
            fileCache: MasticTheme.svgFileMASTIC

            pressedID: releasedID + "-pressed"
            releasedID: "button"
            disabledID : releasedID

            font {
                family: MasticTheme.textFontFamily
                weight : Font.Medium
                pixelSize : 16
            }
            labelColorPressed: MasticTheme.blackColor
            labelColorReleased: MasticTheme.whiteColor
            labelColorDisabled: MasticTheme.whiteColor
        }

        onClicked: {
            // Close our popup
            rootItem.close();
        }
    }

}

