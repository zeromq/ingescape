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

    width: 666
    height: 420

    automaticallyOpenWhenCompleted: true
    isModal: false
    dismissOnOutsideTap : false;
    keepRelativePositionToInitialParent : false;


    //--------------------------------
    //
    // Properties
    //
    //--------------------------------

    // our model is a definition
    property var definition: model.QtObject;



    // Columns with a fixed size
    // - Type
    property int widthColumnType: 135
    // - Definition Value
    property int widthColumnDefaultValue: 140
    // - Mapping Value
    property int widthColumnCurrentValue: 140
    // - Mute
    property int widthColumnMute: 40

    // Resizable columns
    // - Name
    // 8 <=> scrollbar width
    property int widthColumnName: (tabs.width - 8 - widthColumnType - widthColumnDefaultValue - widthColumnCurrentValue - widthColumnMute)

    // List of widths
    property var widthsOfColumns: [
        widthColumnName,
        widthColumnType,
        widthColumnDefaultValue,
        widthColumnCurrentValue,
        widthColumnMute
    ]

    //--------------------------------
    //
    // Signals
    //
    //--------------------------------

    // Emitted when user pressed our popup
    signal bringToFront();


    // Emitted when user clicks on "istory" button
    signal openHistory();

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

        // catch events
        MultiPointTouchArea {
            anchors.fill: parent
        }

        // drag Area
        I2CustomRectangle {
            id : dragRect
            anchors {
                top : parent.top
                left : parent.left
                right : parent.right
                margins : 2
            }
            height : 50
            topLeftRadius : 5
            topRightRadius : 5

            color :dragMouseArea.pressed? MasticTheme.editorsBackgroundColor : MasticTheme.darkBlueGreyColor

            MouseArea {
                id : dragMouseArea
                hoverEnabled: true
                anchors.fill: parent
                drag.target: rootItem

                drag.minimumX : - rootItem.width/2
                drag.maximumX : rootItem.parent.width - rootItem.width/2
                drag.minimumY :  0
                drag.maximumY :  rootItem.parent.height - rootItem.height/2

                onPressed: {
                    // Emit signal "bring to front"
                    rootItem.bringToFront();
                }
            }
        }

        // separator
        Rectangle {
            anchors {
                top : dragRect.bottom
                left : dragRect.left
                right : dragRect.right
            }
            height : 1
            color : MasticTheme.editorsBackgroundBorderColor
        }


        Button {
            id: btnCloseEditor

            anchors {
                verticalCenter: definitionNameItem.verticalCenter
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


        // Definition name and version
        Item {
            id : definitionNameItem

            anchors {
                top : parent.top
                topMargin: 15
                left : parent.left
                leftMargin: 18
                right : parent.right
                rightMargin: 18
            }

            height : definitionNameTxt.height

            TextMetrics {
                id : definitionName

                elideWidth: (definitionNameItem.width - versionName.width)
                elide: Text.ElideRight

                text: definition ? definition.name : ""
            }

            Text {
                id : definitionNameTxt

                anchors {
                    left : parent.left
                }

                text : definitionName.elidedText
                color: MasticTheme.definitionEditorsLabelColor
                font {
                    family: MasticTheme.textFontFamily
                    pixelSize: 23
                    weight: Font.Medium
                }

            }

            Text {
                id : versionName
                anchors {
                    bottom: definitionNameTxt.bottom
                    bottomMargin : 2
                    left : definitionNameTxt.right
                    leftMargin: 5
                }

                text: definition ? "(v" + definition.version + ")" : ""
                color: definitionNameTxt.color

                font {
                    family: MasticTheme.textFontFamily
                    pixelSize : 16
                    italic : true
                }
            }
        }


        Text {
            id : descriptionTxt
            anchors {
                top : definitionNameItem.bottom
                topMargin: 28
                left : definitionNameItem.left
                right : parent.right
                rightMargin: 22
            }

            text: definition ? definition.description : ""

            width: parent.width
            wrapMode: Text.Wrap
            elide : Text.ElideRight
            maximumLineCount : 3

            color: MasticTheme.definitionEditorsAgentDescriptionColor
            font {
                family: MasticTheme.textFontFamily
                pixelSize : 16
            }
        }



        I2TabView {
            id : tabs

            anchors {
                top: parent.top
                topMargin: 130
                left: parent.left
                leftMargin: 20
                right: parent.right
                rightMargin: 20
                bottom: parent.bottom
                bottomMargin: 50
            }
            clip : true

            style: I2TabViewStyle {
                frameOverlap: 1
                tabsAlignment : Qt.AlignHCenter
                tab: Item {
                    implicitHeight: 30
                    implicitWidth: 104

                    I2SvgItem {
                        id : svgMenu
                        anchors.centerIn: parent

                        svgFileCache : MasticTheme.svgFileMASTIC;
                        svgElementId:  styleData.selected ? "tab-" + index + "-Selected" : "tab-" + index;

                        Text {
                            anchors.centerIn: parent

                            text : styleData.title

                            color : styleData.selected ? MasticTheme.veryDarkGreyColor : MasticTheme.whiteColor
                            font {
                                family: MasticTheme.textFontFamily
                                weight : Font.Medium
                                pixelSize : 16
                            }
                        }
                    }
                }

                frame: Rectangle {
                    color : "transparent"
                }
            }

            onCurrentIndexChanged: {
            }


            //---------------------------------------
            //
            // Content of our tabs
            //
            //---------------------------------------
            Repeater {
                model : ["Inputs", "Outputs", "Parameters"]

                Tab {
                    id : tab
                    title: modelData;
                    active : true

                    Item {
                        anchors.fill: parent

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
                                    qsTr("Type"),
                                    qsTr("Default value"),
                                    qsTr("Current value"),
                                    qsTr("Mute")
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

                                        text : (modelData !== "Mute" || tab.title === "Outputs") ? modelData : ""

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
                        ScrollView {
                            id : scrollView

                            anchors {
                                top: tableauHeaderRow.bottom
                                left : parent.left
                                right : parent.right
                                bottom : parent.bottom
                            }

                            // Prevent drag overshoot on Windows
                            flickableItem.boundsBehavior: Flickable.OvershootBounds

                            style: MasticScrollViewStyle {
                            }

                            // Content of our scrollview
                            ListView {
                                id : listView

                                width : scrollView.width

                                model: if (definition) {
                                           switch (tabs.currentIndex)
                                           {
                                           case 0:
                                               definition.inputsList
                                               break;
                                           case 1:
                                               definition.outputsList
                                               break;
                                           case 2:
                                               definition.parametersList
                                               break;
                                           }
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
                                        }

                                        // Name
                                        Text {
                                            text: model.name

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


                                        // Type
                                        Item {
                                            anchors {
                                                verticalCenter: parent.verticalCenter
                                            }
                                            height: parent.height
                                            width : rootItem.widthsOfColumns[1]

                                            Rectangle {
                                                id : circle
                                                anchors {
                                                    left : parent.left
                                                    verticalCenter: parent.verticalCenter
                                                }

                                                width : 16
                                                height : width
                                                radius : width/2

                                                color : switch (model.agentIOPValueTypeGroup)
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
                                                text: AgentIOPValueTypes.enumToString(model.agentIOPValueType)

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


                                        // Default Value
                                        Text {
                                            text: model.displayableDefaultValue

                                            anchors {
                                                verticalCenter: parent.verticalCenter
                                            }
                                            verticalAlignment: Text.AlignVCenter
                                            width : rootItem.widthsOfColumns[2]
                                            height: parent.height
                                            elide: Text.ElideRight
                                            color: MasticTheme.whiteColor
                                            font {
                                                family: MasticTheme.textFontFamily
                                                pixelSize : 16
                                            }
                                        }


                                        // Current Value
                                        Text {
                                            text: model.displayableCurrentValue

                                            anchors {
                                                verticalCenter: parent.verticalCenter
                                            }
                                            verticalAlignment: Text.AlignVCenter
                                            width : rootItem.widthsOfColumns[3]
                                            height: parent.height
                                            color: MasticTheme.whiteColor
                                            elide: Text.ElideRight

                                            font {
                                                family: MasticTheme.textFontFamily
                                                pixelSize : 16
                                            }
                                        }

                                        // Mute
                                        Item {
                                            anchors {
                                                verticalCenter: parent.verticalCenter
                                            }
                                            width : rootItem.widthsOfColumns[4]
                                            height: parent.height

                                            Button {
                                                id: btnMuteOutput
                                                visible: (model.agentIOPType === AgentIOPTypes.OUTPUT)
                                                enabled : visible
                                                activeFocusOnPress: true

                                                anchors {
                                                    verticalCenter: parent.verticalCenter
                                                    right : parent.right
                                                }

                                                style: Theme.LabellessSvgButtonStyle {
                                                    fileCache: MasticTheme.svgFileMASTIC

                                                    pressedID: releasedID + "-pressed"
                                                    releasedID: model.isMuted? "muteactif" : "muteinactif"
                                                    disabledID : releasedID

                                                }

                                                onClicked: {
                                                    model.QtObject.changeMuteOutput();
                                                }
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
                }

            }

        }

        // History
        MouseArea {
            id : historyBtn
            enabled: visible
            anchors {
                left : tabs.left
                top : tabs.bottom
                topMargin: 16
            }

            height : history.height
            width : history.width

            hoverEnabled: true
            onClicked: {
                if (definition) {
                    definition.openValuesHistoryOfAgent();
                }
                rootItem.openHistory();
            }

            Text {
                id: history

                anchors {
                    left : parent.left
                }
                text : "> History"
                color: historyBtn.pressed ? MasticTheme.lightGreyColor : MasticTheme.whiteColor
                elide: Text.ElideRight

                font {
                    family: MasticTheme.textFontFamily
                    pixelSize: 16
                }
            }

            // underline
            Rectangle {
                visible: historyBtn.containsMouse

                anchors {
                    left : historyBtn.left
                    right : history.right
                    bottom : parent.bottom
                }

                height : 1

                color : history.color
            }
        }


        Row {
            anchors {
                right : tabs.right
                top : tabs.bottom
                topMargin: 12
                bottom : parent.bottom
                bottomMargin: 16
            }
            spacing : 15


            //            Button {
            //                id: cancelButton
            //            activeFocusOnPress: true
            //                property var boundingBox: MasticTheme.svgFileMASTIC.boundsOnElement("button");
            //                height : boundingBox.height
            //                width :  boundingBox.width

            //                enabled : visible
            //                text : "Cancel"

            //                anchors {
            //                    verticalCenter: parent.verticalCenter
            //                }

            //                style: I2SvgButtonStyle {
            //                    fileCache: MasticTheme.svgFileMASTIC

            //                    pressedID: releasedID + "-pressed"
            //                    releasedID: "button"
            //                    disabledID : releasedID

            //                    font {
            //                        family: MasticTheme.textFontFamily
            //                        weight : Font.Medium
            //                        pixelSize : 16
            //                    }
            //                    labelColorPressed: MasticTheme.blackColor
            //                    labelColorReleased: MasticTheme.whiteColor
            //                    labelColorDisabled: MasticTheme.whiteColor

            //                }

            //                onClicked: {
            //                    // Close our popup
            //                    rootItem.close();
            //                }
            //            }

            //            Button {
            //                id: okButton

            //                property var boundingBox: MasticTheme.svgFileMASTIC.boundsOnElement("button");
            //                height : boundingBox.height
            //                width :  boundingBox.width

            //                enabled : visible
            //                activeFocusOnPress: true
            //                text : "OK"

            //                anchors {
            //                    verticalCenter: parent.verticalCenter
            //                }

            //                style: I2SvgButtonStyle {
            //                    fileCache: MasticTheme.svgFileMASTIC

            //                    pressedID: releasedID + "-pressed"
            //                    releasedID: "button"
            //                    disabledID : releasedID

            //                    font {
            //                        family: MasticTheme.textFontFamily
            //                        weight : Font.Medium
            //                        pixelSize : 16
            //                    }
            //                    labelColorPressed: MasticTheme.blackColor
            //                    labelColorReleased: MasticTheme.whiteColor
            //                    labelColorDisabled: MasticTheme.whiteColor

            //                }

            //                onClicked: {
            //                    // Close our popup
            //                    rootItem.close();
            //                }
            //            }

        }

    }


}
