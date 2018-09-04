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

    title: (controller ? qsTr("Log stream for %1").arg(controller.agentName) : qsTr("Log stream"))

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
        130,
        70,
        400
    ]


    //--------------------------------
    //
    // Signals
    //
    //--------------------------------

    // Emitted when "All Log Types" is selected or unselected
    signal clickAllLogTypes();


    //--------------------------------
    //
    // Behavior
    //
    //--------------------------------

    Connections {
        target: controller

        //ignoreUnknownSignals: true

        onBringToFront: {
            //console.log("QML of Log Stream Panel: onBringToFront");

            // Raises the window in the windowing system.
            rootItem.raise();
        }
    }


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


        // Title (Agent name and Host name)
        Item {
            id: titleItem

            anchors {
                top : parent.top
                topMargin: 15
                left : parent.left
                leftMargin: 18
                right : parent.right
                rightMargin: 18
            }

            height : txtAgentName.height

            TextMetrics {
                id: textMetrics

                elideWidth: (titleItem.width - txtHostname.width)
                elide: Text.ElideRight

                text: (controller ? qsTr("Log stream for %1").arg(controller.agentName) : qsTr("Log stream"))
            }

            Text {
                id : txtAgentName

                anchors {
                    left : parent.left
                }

                text : textMetrics.elidedText
                color: IngeScapeTheme.whiteColor
                font {
                    family: IngeScapeTheme.textFontFamily
                    pixelSize: 23
                    weight: Font.Medium
                }

            }

            Text {
                id : txtHostname
                anchors {
                    bottom: txtAgentName.bottom
                    bottomMargin : 2
                    left : txtAgentName.right
                    leftMargin: 5
                }

                text: (controller ? "(" + controller.agentHostname + ")" : "")

                color: txtAgentName.color
                font {
                    family: IngeScapeTheme.textFontFamily
                    pixelSize : 16
                    italic : true
                }
            }
        }



        // Log Types Filter
        Item {
            id : _combobox

            anchors {
                right : parent.right
                rightMargin: 20
                top : titleItem.bottom
                topMargin: 30
            }

            height : 25
            width : 197

            onVisibleChanged: {
                if (!visible) {
                    close();
                }
            }

            /***
            * open function : open the combobox
            ***/
            function open() {
                _comboButton.checked = true;
                popup.open();
            }

            /***
            * close function : close the combobox
            ***/
            function close() {
                _comboButton.checked = false;
                _combobox.forceActiveFocus();
                popup.close();
            }

            Rectangle {
                id: _comboButton
                property bool checked : false;

                width: parent.width
                height: parent.height
                radius: 1

                border.width: _mouseAreaCombo.containsPress ? 1 : 0
                border.color: IngeScapeTheme.darkBlueGreyColor
                color: _mouseAreaCombo.containsPress ? IngeScapeTheme.darkGreyColor2 : IngeScapeTheme.darkBlueGreyColor

                Text {
                    id: _comboPlaceholder

                    visible: (_comboText.text === "");
                    text: "- Select a type -"
                    anchors {
                        verticalCenter: parent.verticalCenter;
                        left: parent.left;
                        leftMargin: 10
                        right: _imageCombo.left;
                        rightMargin: 10
                    }


                    font {
                        pixelSize: 15
                        family: IngeScapeTheme.textFontFamily;
                        italic : true;
                    }

                    color : IngeScapeTheme.greyColor
                    verticalAlignment: Text.AlignVCenter;
                    elide : Text.ElideRight;
                }


                Text {
                    id:_comboText

                    anchors {
                        verticalCenter: parent.verticalCenter;
                        left: parent.left;
                        leftMargin: 10
                        right: _imageCombo.left;
                        rightMargin: 10
                    }

                    font {
                        pixelSize: 15
                        family: IngeScapeTheme.textFontFamily;
                    }

                    color : IngeScapeTheme.lightGreyColor
                    verticalAlignment: Text.AlignVCenter;
                    elide : Text.ElideRight;
                    text: (rootItem.controller && rootItem.controller.selectedLogTypes.count > 0) ? ((rootItem.controller.selectedLogTypes.count < rootItem.controller.allLogTypes.count) ? ("- " + rootItem.controller.selectedLogTypes.count + ((rootItem.controller.selectedLogTypes.count === 1) ? " type selected -" : " types selected -"))
                                                                                                                                                                                             : "- All types selected -")
                                                                                                   : "";
                }


                Image {
                    id:_imageCombo;
                    anchors.verticalCenter: parent.verticalCenter;
                    anchors.right: parent.right;
                    anchors.rightMargin: 10
                    rotation : (_comboButton.checked ? 180 : 0);
                    source : "image://I2svg/resources/SVG/ingescape-pictos.svg#iconCombo";

                    Behavior on rotation {
                        NumberAnimation {}
                    }
                }


                MouseArea {
                    id: _mouseAreaCombo;
                    anchors.fill: parent;
                    activeFocusOnTab: true;
                    onClicked: {
                        _mouseAreaCombo.forceActiveFocus();

                        if (_comboButton.checked) {
                            _combobox.close();
                        }
                        else {
                            _combobox.open();
                        }
                    }
                }

                onVisibleChanged: {
                    if (!visible) {
                        _combobox.close();
                    }
                }
            }

            I2PopupBase {
                id : popup
                anchors.top: _comboButton.bottom;

                width: _comboButton.width;
                height: ((_combolist.count < 8) ? (_combolist.count + 1) * (_comboButton.height + 1) : 9 * (_comboButton.height + 1) );


                isModal: true;
                layerColor: "transparent"
                layerObjectName: "overlayLayerComboBox";
                dismissOnOutsideTap : true;

                keepRelativePositionToInitialParent : true;

                onClosed: {
                    _combobox.close();
                }

                onOpened: {

                }

                Rectangle {
                    id : popUpBackground
                    anchors.fill : parent
                    color: IngeScapeTheme.darkBlueGreyColor
                }

                ScrollView {
                    id : _scrollView

                    visible: _comboButton.checked;

                    anchors {
                        top: parent.top;
                        bottom: parent.bottom;
                    }

                    width: _comboButton.width;
                    height: ((_combolist.count < 8) ? (_combolist.count + 1) * (_comboButton.height + 1) : 9 * (_comboButton.height + 1) );

                    // Prevent drag overshoot on Windows
                    flickableItem.boundsBehavior: Flickable.OvershootBounds

                    style: IngeScapeScrollViewStyle {
                    }

                    contentItem: Item {
                        width: _scrollView.width
                        height: ( (_combolist.count < 8) ? (_combolist.count + 1) * (_comboButton.height + 1) : 9 * (_comboButton.height + 1) )

                        CheckBox {
                            id : filterAllTypesCB
                            anchors {
                                left: parent.left
                                leftMargin :10
                                right : parent.right
                                rightMargin : 10
                                top : parent.top
                                topMargin: 4
                            }

                            property bool isPartiallyChecked : false

                            checked: false;
                            partiallyCheckedEnabled: false;
                            activeFocusOnPress: true;

                            style: CheckBoxStyle {
                                label: Text {
                                    anchors {
                                        verticalCenter: parent.verticalCenter
                                        verticalCenterOffset: 1
                                    }

                                    color: IngeScapeTheme.lightGreyColor

                                    text: " All types"
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

                                    Text {
                                        visible : filterAllTypesCB.isPartiallyChecked
                                        anchors {
                                            centerIn: parent
                                        }

                                        color: IngeScapeTheme.lightGreyColor

                                        text: "-"
                                        elide: Text.ElideRight

                                        font {
                                            family: IngeScapeTheme.textFontFamily
                                            pixelSize: 16
                                        }
                                    }
                                }

                            }

                            onClicked : {
                                // reset isPartiallyChecked property
                                filterAllTypesCB.isPartiallyChecked =  false;

                                // show / hide all log types
                                if (rootItem.controller) {
                                    if (filterAllTypesCB.checked) {
                                        rootItem.controller.showAllLogs()
                                    }
                                    else {
                                        rootItem.controller.hideAllLogs()
                                    }
                                }

                                // signal the change to all log type check-boxes
                                rootItem.clickAllLogTypes();
                            }

                            Connections {
                                target : popup

                                // update "all types" checkbox state
                                onOpened : {
                                    // reset isPartiallyChecked and checkedState properties
                                    filterAllTypesCB.isPartiallyChecked = false;
                                    filterAllTypesCB.checkedState = Qt.Unchecked;

                                    if (rootItem.controller && (rootItem.controller.selectedLogTypes.count > 0))
                                    {
                                        if (rootItem.controller.selectedLogTypes.count === rootItem.controller.allLogTypes.count) {
                                            filterAllTypesCB.checkedState = Qt.Checked;
                                        }
                                        else {
                                            filterAllTypesCB.isPartiallyChecked = true;
                                        }
                                    }
                                }
                            }
                        }

                        ListView {
                            id: _combolist

                            boundsBehavior: Flickable.StopAtBounds

                            anchors {
                                top : filterAllTypesCB.bottom
                                topMargin: 4
                            }

                            width: parent.width;
                            height: ( (_combolist.count < 8) ? _combolist.count*(_comboButton.height + 1) : 8 * (_comboButton.height + 1) );

                            visible: parent.visible;

                            model: (rootItem.controller ? rootItem.controller.allLogTypes : 0)

                            delegate: Item {

                                anchors {
                                    left: parent.left
                                    right: parent.right
                                }

                                width: _comboButton.width
                                height: _comboButton.height

                                CheckBox {
                                    id : filterLogTypeCB

                                    anchors {
                                        verticalCenter: parent.verticalCenter
                                        left: parent.left
                                        leftMargin :10
                                        right : parent.right
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

                                            //color: IngeScapeTheme.lightGreyColor
                                            color: IngeScapeTheme.colorOfLogType(model.value)

                                            text: " " + model.name
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
                                        if (rootItem.controller)
                                        {
                                            if (filterLogTypeCB.checked) {
                                                rootItem.controller.showLogsOfType(model.value)
                                            }
                                            else {
                                                rootItem.controller.hideLogsOfType(model.value)
                                            }

                                            // update "all agents" checkbox state
                                            // reset isPartiallyChecked and checkedState properties
                                            filterAllTypesCB.isPartiallyChecked = false;
                                            filterAllTypesCB.checkedState = Qt.Unchecked;

                                            if (rootItem.controller.selectedLogTypes.count > 0)
                                            {
                                                if (rootItem.controller.selectedLogTypes.count === rootItem.controller.allLogTypes.count) {
                                                    filterAllTypesCB.checkedState = Qt.Checked;
                                                }
                                                else {
                                                    filterAllTypesCB.isPartiallyChecked = true;
                                                }
                                            }
                                        }
                                    }

                                    Connections {
                                        target : popup

                                        onOpened : {
                                            // update checkbox state when the pop up is opening
                                            if (rootItem.controller) {
                                                filterLogTypeCB.checked = rootItem.controller.isSelectedLogType(model.value);
                                            }
                                        }
                                    }

                                    Connections {
                                        target : rootItem

                                        onClickAllLogTypes : {
                                            // update checkbox state when the special check box "All Log Types" is selected or unselected
                                            if (rootItem.controller) {
                                                filterLogTypeCB.checked = rootItem.controller.isSelectedLogType(model.value);
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
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
                               controller.filteredLogs
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
                                text: LogTypes.enumToString(model.logType)

                                anchors {
                                    verticalCenter: parent.verticalCenter
                                }
                                verticalAlignment: Text.AlignVCenter

                                width : rootItem.widthsOfColumns[1]
                                height: parent.height

                                //elide: Text.ElideRight

                                //color: IngeScapeTheme.whiteColor
                                color: IngeScapeTheme.colorOfLogType(model.logType)

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
            height: boundingBox.height
            width: boundingBox.width

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


    I2Layer {
        id: overlayLayerComboBox
        objectName: "overlayLayerComboBox"

        anchors.fill: parent
    }
}
