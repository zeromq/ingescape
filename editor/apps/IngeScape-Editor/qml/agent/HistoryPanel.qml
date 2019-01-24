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
 *      Justine Limoges <limoges@ingenuity.io>
 */

import QtQuick 2.8
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4
import QtQuick.Window 2.3
import I2Quick 1.0

import INGESCAPE 1.0
import "../theme" as Theme;


WindowBlockTouches {
    id: rootItem

    title: qsTr("Outputs history")

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

    property var controller : IngeScapeEditorC.valuesHistoryC

    // Columns with a fixed size

    // - Type
    property int widthColumnType: 110
    // - Date
    property int widthColumnDate: 100

    // Minimum widht of resizable columns
    // - Name
    property int minWidthColumnName: 140
    // - Agent
    property int minWidthColumnAgent: 110
    // - Value
    property int minWidthColumnValue: 140

    // Maximum widht of resizable columns
    // - Name
    property int maxWidthColumnName: 230
    // - Agent
    property int maxWidthColumnAgent: 200


    // Resizable columns
    // - Available extra width that can be splitted between our 3 resizable columns
    property real splittableAvailableWidth: (tableHistory.width - widthColumnType - widthColumnDate - minWidthColumnName - minWidthColumnAgent - minWidthColumnValue)
    property int extraWidthForResizableColumns: Math.max(0, Math.round(splittableAvailableWidth/3))
    property int extraWidthForValueOnly : if (widthColumnName === maxWidthColumnName && widthColumnAgent === maxWidthColumnAgent) {
                                              Math.max(0, (tableHistory.width - widthColumnType - widthColumnDate - maxWidthColumnName - maxWidthColumnAgent - minWidthColumnValue));
                                          }
                                          else {
                                              0;
                                          }

    // - Width Resizable Columns
    property int widthColumnName: ((minWidthColumnName + extraWidthForResizableColumns) > maxWidthColumnName) ? maxWidthColumnName : (minWidthColumnName + extraWidthForResizableColumns);
    property int widthColumnAgent: ((minWidthColumnAgent + extraWidthForResizableColumns) > maxWidthColumnAgent) ? maxWidthColumnAgent : (minWidthColumnAgent + extraWidthForResizableColumns);
    property int widthColumnValue: if (widthColumnName === maxWidthColumnName && widthColumnAgent === maxWidthColumnAgent) {
                                       minWidthColumnValue + extraWidthForValueOnly;
                                   }
                                   else {
                                       minWidthColumnValue + extraWidthForResizableColumns;
                                   }


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

    // Emitted when "All Agents" is selected or unselected
    signal clickAllAgents();


    //--------------------------------
    //
    // Content
    //
    //--------------------------------

    Rectangle {

        anchors.fill: parent

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

            height: titleTxt.height


            Text {
                id: titleTxt

                anchors {
                    left : parent.left
                }

                text: title
                elide: Text.ElideRight
                color: IngeScapeTheme.whiteColor
                font {
                    family: IngeScapeTheme.textFontFamily
                    pixelSize: 23
                    weight: Font.Medium
                }
            }
        }

        // Agents Filter
        DropDownCheckboxes {
            id: dropDown
            anchors {
                right : parent.right
                rightMargin: 20
                top : titleItem.bottom
                topMargin: 30
            }

            height : 25
            width : 197
            enabled: rootItem.controller && rootItem.controller.allAgentNamesList.length > 0

            model: rootItem.controller ? rootItem.controller.allAgentNamesList : 0

            placeholderText: enabled ? "- Select an agent -" : "- No agent -"
            text: (enabled && rootItem.controller && rootItem.controller.selectedAgentNamesList.length > 0) ?
                      (rootItem.controller.selectedAgentNamesList.length < rootItem.controller.allAgentNamesList.length) ?
                          (rootItem.controller.selectedAgentNamesList.length === 1 ? "- " + rootItem.controller.selectedAgentNamesList.length + " agent selected -" : "- " + rootItem.controller.selectedAgentNamesList.length + " agents selected -")
                        : "- All agents selected -"
            : "";
            checkAllText: " All agents"

            onCheckAll: {
                rootItem.controller.showValuesOfAllAgents()
                rootItem.clickAllAgents();
            }

            onUncheckAll: {
                rootItem.controller.hideValuesOfAllAgents()
                rootItem.clickAllAgents();
            }

            onPopupOpen: {
                // update "all agents" checkbox state
                // reset isPartiallyChecked and checkedState properties
                isPartiallyChecked = false;
                checkAllState = Qt.Unchecked;

                if (rootItem.controller && (rootItem.controller.selectedAgentNamesList.length > 0))
                {
                    if (rootItem.controller.selectedAgentNamesList.length === rootItem.controller.allAgentNamesList.length) {
                        checkAllState = Qt.Checked;
                    }
                    else {
                        isPartiallyChecked = true;
                    }
                }
            }

            delegate: Item {
                anchors {
                    left: parent.left
                    right: parent.right
                }

                width: dropDown.comboButton.width
                height: dropDown.comboButton.height

                CheckBox {
                    id : filterAgentCB
                    anchors {
                        verticalCenter: parent.verticalCenter
                        left: parent.left
                        leftMargin :10
                        right: parent.right
                        rightMargin : 10
                    }

                    checked: false;
                    activeFocusOnPress: true;

                    style: CheckBoxStyle {
                        label: Text {
                            anchors {
                                verticalCenter: parent.verticalCenter
                                verticalCenterOffset: 1
                            }

                            color: IngeScapeTheme.lightGreyColor

                            text: " " + modelData
                            elide: Text.ElideRight

                            font {
                                family: IngeScapeTheme.textFontFamily
                                pixelSize: 16
                            }

                        }

                        indicator: Rectangle {
                            implicitWidth: 14
                            implicitHeight: 14
                            border.width: 0
                            color: IngeScapeTheme.veryDarkGreyColor

                            I2SvgItem {
                                visible: (control.checkedState === Qt.Checked)
                                anchors.centerIn: parent

                                svgFileCache: IngeScapeTheme.svgFileINGESCAPE;
                                svgElementId: "check";

                            }
                        }

                    }

                    onClicked : {
                        if (rootItem.controller) {
                            if (checked) {
                                rootItem.controller.showValuesOfAgent(modelData)
                            }
                            else {
                                rootItem.controller.hideValuesOfAgent(modelData)
                            }

                            // update "all agents" checkbox state
                            // reset isPartiallyChecked and checkedState properties
                            dropDown.isPartiallyChecked = false;
                            dropDown.checkAllState = Qt.Unchecked;

                            if (rootItem.controller && (rootItem.controller.selectedAgentNamesList.length > 0))
                            {
                                if (rootItem.controller.selectedAgentNamesList.length === rootItem.controller.allAgentNamesList.length) {
                                    dropDown.checkAllState = Qt.Checked;
                                }
                                else {
                                    dropDown.isPartiallyChecked = true;
                                }
                            }
                        }
                    }

                    Connections {
                        target : dropDown.popup
                        onOpened : {
                            // update agents checkboxes states when the pop up is opening
                            if (controller) {
                                filterAgentCB.checked = controller.areShownValuesOfAgent(modelData);
                            }
                        }
                    }

                    Connections {
                        target : rootItem
                        onClickAllAgents : {
                            // update agents checkboxes states when the "pop up is opening   "All Agents" check box is selected or unselected
                            if (controller) {
                                filterAgentCB.checked = controller.areShownValuesOfAgent(modelData);
                            }
                        }
                    }
                }
            }
        }

        // History List
        Item {
            id : tableHistory

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
                               controller.filteredValues
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
                            //spacing: 5

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
                                color: IngeScapeTheme.whiteColor
                                font {
                                    family: IngeScapeTheme.textFontFamily
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
                                color: IngeScapeTheme.lightGreyColor
                                font {
                                    family: IngeScapeTheme.textFontFamily
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

                                    color : IngeScapeTheme.colorOfIOPTypeWithConditions(model.iopValueTypeGroup, true);
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
                                    color: IngeScapeTheme.whiteColor
                                    font {
                                        family: IngeScapeTheme.textFontFamily
                                        pixelSize : 16
                                    }
                                }
                            }


                            // Value
                            /*Text {
                                text: model.displayableValue

                                anchors {
                                    verticalCenter: parent.verticalCenter
                                }
                                verticalAlignment: Text.AlignVCenter
                                width : rootItem.widthsOfColumns[3]
                                height: parent.height
                                elide: Text.ElideRight
                                color: IngeScapeTheme.whiteColor
                                font {
                                    family: IngeScapeTheme.textFontFamily
                                    pixelSize : 16
                                }
                            }*/
                            TextField {
                                anchors {
                                    verticalCenter: parent.verticalCenter
                                }
                                //verticalAlignment: TextInput.AlignVCenter
                                //horizontalAlignment: TextInput.AlignLeft
                                width : rootItem.widthsOfColumns[3]
                                height: parent.height

                                text: model.displayableValue
                                readOnly: true
                                //elide: Text.ElideRight

                                textColor: IngeScapeTheme.whiteColor

                                font {
                                    family: IngeScapeTheme.textFontFamily
                                    pixelSize : 16
                                }

                                style: TextFieldStyle {
                                        //textColor: "black"

                                        background: Rectangle {
                                            color: "transparent"
                                        }
                                    }
                            }


                            // Date
                            Text {
                                text: model.time.toLocaleString(Qt.locale(), "HH:mm:ss.zzz")

                                anchors {
                                    verticalCenter: parent.verticalCenter
                                }
                                verticalAlignment: Text.AlignVCenter
                                width : rootItem.widthsOfColumns[4]
                                height: parent.height
                                color: IngeScapeTheme.whiteColor
                                elide: Text.ElideRight

                                font {
                                    family: IngeScapeTheme.textFontFamily
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

                            color : IngeScapeTheme.blackColor
                        }

                    }
                }
            }
        }


        Button {
            id: okButton

            property var boundingBox: IngeScapeTheme.svgFileINGESCAPE.boundsOnElement("button");
            height: boundingBox.height
            width: boundingBox.width

            activeFocusOnPress: true
            text: "OK"

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


    I2Layer {
        id: overlayLayerComboBox
        objectName: "overlayLayerComboBox"

        anchors.fill: parent
    }

}

