/*
 *	IngeScape Assessments
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
//import QtQuick.Controls 2.5

import I2Quick 1.0

import INGESCAPE 1.0

//import "theme" as Theme
import "../popup" as Popup
import "../subject" as Subject
import "../task" as Task
import "../export" as Export


Item {
    id: rootItem

    //--------------------------------------------------------
    //
    //
    // Properties
    //
    //
    //--------------------------------------------------------

    property ExperimentationController controller: null;

    property ExperimentationM experimentation: controller ? controller.currentExperimentation : null;

    property bool isEditingName: false

    property int rightMarginToAvoidToBeHiddenByNetworkConnectionInfo: 220

    property int subScreensMargin: 35

    // Flag indicating if we are currently selecting the sessions to export
    property bool _isSelectingSessionsToExport: false

    // Duration of the animation about appearance of the check box to select sessions
    property int _appearanceAnimationDuration: 250


    //--------------------------------
    //
    //
    // Signals
    //
    //
    //--------------------------------

    // Go back to "Home"
    signal goBackToHome();

    // Emitted when "All protocol" is selected or unselected
    signal clickAllProtocol();

    // Emitted when "All subject" is selected or unselected
    signal clickAllSubject();


    //--------------------------------------------------------
    //
    //
    // Content
    //
    //
    //--------------------------------------------------------

    Rectangle {
        id: background

        anchors.fill: parent

        color: IngeScapeTheme.veryLightGreyColor
    }

    Item {
        id: headerItem

        anchors {
            top: parent.top
            left: parent.left
            right: parent.right
        }


        Rectangle {
            id: headerBackground

            anchors.fill: parent

            color: IngeScapeTheme.whiteColor
        }

        height: 108

        Button {
            id: btnGoBackToHome

            anchors {
                left: parent.left
                leftMargin: 21
                top: parent.top
                topMargin: 20
            }

            height: 50
            width: 50

            style: IngeScapeAssessmentsSvgButtonStyle {
                releasedID: "home-button"
            }

            onClicked: {
                console.log("QML: Go back to 'Home'");

                // Emit the signal the "Go Back To Home"
                rootItem.goBackToHome();
            }
        }

        MouseArea {
            id: expeNameMouseArea
            anchors {
                fill: expeNameEditBackground
            }

            hoverEnabled: true
        }

        Rectangle {
            id: expeNameEditBackground

            anchors {
                left: expeName.left
                leftMargin: -10
                verticalCenter: expeName.verticalCenter
            }

            radius: 5

            // Background rectangle won't be smaller than this
            property real minWidth: 450

            // Max available width based on the size of the parent. Won't be bigger than this
            property real maxAvailableWidth: (parent.width
                                              - 92 // expeName's leftMargin
                                              + 10 // this left margin from expeName
                                              - rightMarginToAvoidToBeHiddenByNetworkConnectionInfo // Right margin left to avoid (reaching the edge of the window)
                                              // Right margin left to avoid to be hidden by the network connection info panel on foreground
                                              - 10  // Margins around the edit button (5 on both sides)
                                              )

            // Desired width to follow the size of expeName
            property real desiredWidth: expeName.width
                                        + 10                // leftMargin
                                        + 10                // rightMargin
                                        + editButton.width  // button size
                                        + 14                // margin between edit button and text

            width: Math.min(maxAvailableWidth, Math.max(desiredWidth, minWidth))
            height: 40

            color: IngeScapeTheme.veryLightGreyColor

            opacity: (expeNameMouseArea.containsMouse || editButton.containsMouse || rootItem.isEditingName) ? 1 : 0
            enabled: opacity > 0

            Button {
                id: editButton

                anchors {
                    right: parent.right
                    rightMargin: 5
                    verticalCenter: parent.verticalCenter
                }

                property bool containsMouse: __behavior.containsMouse

                width: 42
                height: 30

                style: IngeScapeAssessmentsSvgButtonStyle {
                    releasedID: "edit"
                    disabledID: releasedID
                }

                onClicked: {
                    rootItem.isEditingName = !rootItem.isEditingName
                    if (rootItem.isEditingName)
                    {
                        // Entering edition mode
                        expeNameEditionTextField.text = expeName.text;
                    }
                    else
                    {
                        // Exiting edition mode
                        if (rootItem.experimentation)
                        {
                            expeName.text = expeNameEditionTextField.text
                            rootItem.experimentation.name = expeName.text
                        }

                    }
                }
            }
        }

        Text {
            id: expeName

            anchors {
                top: parent.top
                topMargin: 30
                left: parent.left
                leftMargin: 92
            }

            property real maxAvailableWidth: expeNameEditBackground.maxAvailableWidth
                                             - 10                // background's left margin
                                             - 5                 // edit button's right margin
                                             - editButton.width  // edit button's width
                                             - 14                // margin between edit button and text

            width: Math.min(implicitWidth, maxAvailableWidth)

            text: rootItem.experimentation ? rootItem.experimentation.name : ""
            elide: Text.ElideRight

            verticalAlignment: Text.AlignVCenter
            horizontalAlignment: Text.AlignLeft

            visible: !rootItem.isEditingName

            color: IngeScapeAssessmentsTheme.blueButton
            font {
                family: IngeScapeTheme.labelFontFamily
                weight: Font.Black
                pixelSize: 24
            }
        }

        TextField {
            id: expeNameEditionTextField
            anchors.fill: expeName

            visible: rootItem.isEditingName
            enabled: visible

            style: I2TextFieldStyle {
                backgroundColor: IngeScapeTheme.whiteColor
                borderColor: IngeScapeTheme.lightGreyColor
                borderErrorColor: IngeScapeTheme.redColor
                radiusTextBox: 5
                borderWidth: 0
                borderWidthActive: 1
                textIdleColor: IngeScapeAssessmentsTheme.regularDarkBlueHeader
                textDisabledColor: IngeScapeTheme.veryLightGreyColor

                padding.left: 10
                padding.right: 5

                font {
                    pixelSize: 20
                    family: IngeScapeTheme.textFontFamily
                }
            }
        }

        Text {
            id: expeGroupNameText

            anchors {
                left: (expeNameEditBackground.opacity > 0) ? expeNameEditBackground.right : expeName.right
                leftMargin: 15
                right: parent.right
                rightMargin: rightMarginToAvoidToBeHiddenByNetworkConnectionInfo
                bottom: expeName.bottom
            }
            verticalAlignment: Text.AlignVCenter

            text: rootItem.experimentation ? "- " + rootItem.experimentation.groupName
                                           : ""
            elide: Text.ElideRight

            color: IngeScapeAssessmentsTheme.regularDarkBlueHeader
            font {
                family: IngeScapeTheme.textFontFamily
                pixelSize: 20
                bold: true
            }
        }
    }

    //
    // Main view
    //
    Item {
        id: mainView

        anchors {
            top: headerItem.bottom
            bottom: parent.bottom
            left: parent.left
            right: parent.right
        }

        property real selectionColumnWidth: 30
        property real subjectColumnWidth: 210
        property real protocolColumnWidth: 250
        property real creationDateTimeColumnWidth: 165
        property real buttonsColumnWidth: 135

        property real sessionColumnWidth: taskInstancesPanel.width - taskInstanceScrollView.scrollBarSize - taskInstanceScrollView.verticalScrollbarMargin
                                          - selectionColumnWidth
                                          - subjectColumnWidth
                                          - protocolColumnWidth
                                          - creationDateTimeColumnWidth
                                          - buttonsColumnWidth

        //
        // Configuration Panel
        //
        Item {
            id: configurationPanel

            anchors {
                top: parent.top
                topMargin: 24
                bottom: parent.bottom
                left: parent.left
                leftMargin: 26
            }

            width: 182

            Column {

                anchors {
                    left: parent.left
                    right: parent.right
                    top: parent.top
                }

                spacing: 18

                Button {
                    width: parent.width
                    height: 62

                    style: IngeScapeAssessmentsSvgAndTextButtonStyle {
                        text: qsTr("SUBJECTS")

                        releasedID: "subjects"
                        pressedID: releasedID
                        rolloverID: releasedID
                        disabledID: releasedID
                    }

                    onClicked: {
                        console.log("QML: Open the 'Subjects View' popup");

                        // Open the popup "Subjects View"
                        subjectsViewPopup.open()
                    }
                }

                Button {
                    width: parent.width
                    height: 62

                    style: IngeScapeAssessmentsSvgAndTextButtonStyle {
                        text: qsTr("PROTOCOLS")

                        releasedID: "tasks"
                        pressedID: releasedID
                        rolloverID: releasedID
                        disabledID: releasedID
                    }

                    onClicked: {
                        console.log("QML: Open the 'Protocols View' popup");

                        // Open the popup "Protocols View"
                        protocolsViewPopup.open();
                    }
                }

                Button {
                    width: parent.width
                    height: 62

                    enabled: false

                    style: IngeScapeAssessmentsSvgAndTextButtonStyle {
                        text: qsTr("CODING")

                        releasedID: "coding"
                        pressedID: releasedID
                        rolloverID: releasedID
                        disabledID: releasedID
                    }
                }

                Button {
                    width: parent.width
                    height: 62

                    enabled: false

                    style: IngeScapeAssessmentsSvgAndTextButtonStyle {
                        text: qsTr("CLEANING")

                        releasedID: "cleaning"
                        pressedID: releasedID
                        rolloverID: releasedID
                        disabledID: releasedID
                    }
                }

//                Button {
//                    width: parent.width
//                    height: 62

//                    enabled: rootItem.controller && rootItem.controller.isRecorderON

//                    style: IngeScapeAssessmentsSvgAndTextButtonStyle {
//                        text: qsTr("EXPORT")

//                        releasedID: "export"
//                        pressedID: releasedID
//                        rolloverID: releasedID
//                        disabledID: releasedID
//                    }

//                    onClicked: {
//                        //console.log("QML: Open the 'Export View' popup");

//                        // Open the popup "Export View"
//                        //exportViewPopup.open();

//                        rootItem.controller.showAllSessionsForProtocol();
//                        rootItem.controller.showAllSessionsForSubject();

//                        _isSelectingSessionsToExport = !_isSelectingSessionsToExport;
//                    }
//                }
            }
        }


        //
        // Task Instances Panel
        //
        Item {
            id: taskInstancesPanel

            anchors {
                top: parent.top
                topMargin: 34
                bottom: parent.bottom
                bottomMargin: 28
                left: configurationPanel.right
                leftMargin: 28
                right: parent.right
                rightMargin: 28
            }

            Text {
                id: titleSessions

                anchors {
                    verticalCenter: btnNewSession.verticalCenter
                    left: parent.left
                }

                text: qsTr("SESSIONS")

                height: parent.height
                verticalAlignment: Text.AlignVCenter

                color: IngeScapeAssessmentsTheme.regularDarkBlueHeader
                font {
                    family: IngeScapeTheme.labelFontFamily
                    weight: Font.Black
                    pixelSize: 24
                }
            }

            Button {
                id: btnNewSession

                anchors {
                    top: parent.top
                    right: parent.right
                }

                height: 39
                width: 182

                onClicked: {
                    // Open the popup
                    createSessionPopup.open();
                }

                style: IngeScapeAssessmentsButtonStyle {
                    text: qsTr("NEW SESSION")
                }
            }

            IngeScapeAssessmentsListHeader {
                id: listHeader
                anchors {
                    top: btnNewSession.bottom
                    topMargin: 12
                    left: parent.left
                    right: parent.right
                }

                Row {
                    anchors {
                        top: parent.top
                        left: parent.left
                        //leftMargin: 15
                        right: parent.right
                        bottom: parent.bottom
                    }
                    spacing: 0

                    I2CustomRectangle {
                    //Rectangle {
                        width: mainView.selectionColumnWidth
                        height: parent.height

                        color: IngeScapeAssessmentsTheme.blueButton_pressed
                        topLeftRadius: 5
                        clip: true

                        LabellessSvgButton {
                            id: btnExport

                            anchors {
                                horizontalCenter: parent.horizontalCenter
                                verticalCenter: parent.verticalCenter
                            }

                            enabled: rootItem.controller && rootItem.controller.isRecorderON
                            visible: enabled

                            fileCache: IngeScapeTheme.svgFileIngeScape

                            pressedID: releasedID + "-pressed"
                            releasedID: "export"
                            disabledID : releasedID + "-disabled"

                            onClicked: {
                                //console.log("QML: Open the 'Export View' popup");

                                // Open the popup "Export View"
                                //exportViewPopup.open();

                                rootItem.controller.showAllSessionsForProtocol();
                                rootItem.controller.showAllSessionsForSubject();

                                _isSelectingSessionsToExport = !_isSelectingSessionsToExport;
                            }
                        }
                    }
//                    I2CustomRectangle {
//                    //Rectangle {
//                        width: mainView.selectionColumnWidth
//                        height: parent.height

//                        color: IngeScapeAssessmentsTheme.blueButton_pressed
//                        topLeftRadius: 5
//                        clip: true

//                        Text {
//                            anchors {
//                                horizontalCenter: parent.horizontalCenter
//                                verticalCenter: parent.verticalCenter
//                            }

//                            text: qsTr("-->")
//                            opacity: 1

//                            color: IngeScapeTheme.whiteColor
//                            font {
//                                family: IngeScapeTheme.labelFontFamily
//                                pixelSize: 18
//                                weight: Font.Black
//                            }
//                        }
//                    }

                    Item {
                        width: mainView.subjectColumnWidth
                        height: parent.height

                        Text {
                            anchors {
                                left: parent.left
                                leftMargin: 15
                                verticalCenter: parent.verticalCenter
                            }
                            visible: !_isSelectingSessionsToExport
                            enabled: visible

                            text: qsTr("Subject")
                            color: IngeScapeTheme.whiteColor
                            font {
                                family: IngeScapeTheme.labelFontFamily
                                pixelSize: 18
                                weight: Font.Black
                            }
                        }
                        DropDownCheckboxes {
                            id: dropDownSubject

                            anchors {
                                left: parent.left
                                leftMargin: 15
                                verticalCenter: parent.verticalCenter
                            }

                            height : 25
                            width : mainView.subjectColumnWidth - 10
                            //enabled: rootItem.controller && rootItem.controller.allAgentNamesList.length > 0
                            visible: _isSelectingSessionsToExport && rootItem.controller
                            enabled: visible
                            model: rootItem.controller ? rootItem.controller.subjectIdList : 0

                            placeholderText: enabled ? "- Select a subject -" : ""

                            text:{
                                if(enabled && rootItem.controller && (rootItem.controller.selectedSubjectIdList.length > 0)){
                                    if(rootItem.controller.selectedSubjectIdList.length === 1){
                                        "- " + rootItem.controller.selectedSubjectIdList + " -"
                                    }else if(rootItem.controller.selectedSubjectIdList.length < rootItem.controller.subjectIdList.length){
                                        "- " + rootItem.controller.selectedSubjectIdList.length + " subjects selected -"
                                    }else{
                                        "- All subjects selected -"
                                    }
                                }else{
                                    ""
                                }
                            }

                            checkAllText: " All subjects"

                            onCheckAll: {
                                rootItem.controller.showAllSessionsForSubject()
                                clickAllSubject()
                            }

                            onUncheckAll: {
                                rootItem.controller.hideAllSessionsForSubject()
                                clickAllSubject()
                            }

                            onPopupOpen: {
                                // update "all subjects" checkbox state
                                // reset isPartiallyChecked and checkedState properties
                                isPartiallyChecked = false;
                                checkAllState = Qt.Unchecked;

                                if (rootItem.controller && (rootItem.controller.selectedSubjectIdList.length > 0))
                                {
                                    if (rootItem.controller.selectedSubjectIdList.length === rootItem.controller.subjectIdList.length) {
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

                                width: dropDownSubject.comboButton.width
                                height: dropDownSubject.comboButton.height

                                CheckBox {
                                    id : filterSubjectCB
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

                                                svgFileCache: IngeScapeTheme.svgFileIngeScape;
                                                svgElementId: "check";

                                            }
                                        }

                                    }

                                    onClicked : {
                                        if (rootItem.controller) {
                                            if (checked) {
                                                rootItem.controller.showSessionForSubject(modelData)
                                            }
                                            else {
                                                rootItem.controller.hideSessionForSubject(modelData)
                                            }

                                            // update "all subjects" checkbox state
                                            // reset isPartiallyChecked and checkedState properties
                                            dropDownSubject.isPartiallyChecked = false;
                                            dropDownSubject.checkAllState = Qt.Unchecked;

                                            if (rootItem.controller && (rootItem.controller.selectedSubjectIdList.length > 0))
                                            {
                                                if (rootItem.controller.selectedSubjectIdList.length === rootItem.controller.subjectIdList.length) {
                                                    dropDownSubject.checkAllState = Qt.Checked;
                                                }
                                                else {
                                                    dropDownSubject.isPartiallyChecked = true;
                                                }
                                            }
                                        }
                                    }

                                    Connections {
                                        target : dropDownSubject.popup
                                        onOpened : {
                                            // update subjects checkboxes states when the pop up is opening
                                            if (controller) {
                                                filterSubjectCB.checked = controller.areShownSessionsForSubject(modelData);
                                            }
                                        }
                                    }

                                    Connections {
                                        target : rootItem
                                        onClickAllSubject : {
                                            // update subjects checkboxes states when the "pop up is opening   "All subjects" check box is selected or unselected
                                            if (controller) {
                                                filterSubjectCB.checked = controller.areShownSessionsForSubject(modelData);
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }

                    Item {
                        width: mainView.protocolColumnWidth
                        height: parent.height

                        Text {
                            anchors {
                                left: parent.left
                                leftMargin: 15
                                verticalCenter: parent.verticalCenter
                            }

                            visible: !_isSelectingSessionsToExport
                            enabled: visible

                            text: qsTr("Protocol")
                            color: IngeScapeTheme.whiteColor
                            font {
                                family: IngeScapeTheme.labelFontFamily
                                pixelSize: 18
                                weight: Font.Black
                            }
                        }

                        DropDownCheckboxes {
                            id: dropDownProtocol

                            anchors {
                                left: parent.left
                                leftMargin: 15
                                verticalCenter: parent.verticalCenter
                            }

                            height : 25
                            width : mainView.protocolColumnWidth - 10
                            //enabled: rootItem.controller && rootItem.controller.allAgentNamesList.length > 0
                            visible: _isSelectingSessionsToExport && rootItem.controller
                            enabled: visible
                            model: rootItem.controller ? rootItem.controller.protocolNameList : 0

                            placeholderText: enabled ? "- Select a protocol -" : ""

                            text:{
                                if(enabled && rootItem.controller && (rootItem.controller.selectedProtocolNameList.length > 0)){
                                    if(rootItem.controller.selectedProtocolNameList.length === 1){
                                        "- " + rootItem.controller.selectedProtocolNameList + " -"
                                    }else if(rootItem.controller.selectedProtocolNameList.length < rootItem.controller.protocolNameList.length){
                                        "- " + rootItem.controller.selectedProtocolNameList.length + " protocols selected -"
                                    }else{
                                        "- All protocols selected -"
                                    }
                                }else{
                                    ""
                                }
                            }
                            checkAllText: " All protocol"

                            onCheckAll: {
                                rootItem.controller.showAllSessionsForProtocol()
                                rootItem.clickAllProtocol();
                            }

                            onUncheckAll: {
                                rootItem.controller.hideAllSessionsForProtocol()
                                rootItem.clickAllProtocol();
                            }

                            onPopupOpen: {
                                // update "all agents" checkbox state
                                // reset isPartiallyChecked and checkedState properties
                                isPartiallyChecked = false;
                                checkAllState = Qt.Unchecked;

                                if (rootItem.controller && (rootItem.controller.selectedProtocolNameList.length > 0))
                                {
                                    if (rootItem.controller.selectedProtocolNameList.length === rootItem.controller.protocolNameList.length) {
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

                                width: dropDownProtocol.comboButton.width
                                height: dropDownProtocol.comboButton.height

                                CheckBox {
                                    id : filterProtocolCB
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

                                                svgFileCache: IngeScapeTheme.svgFileIngeScape;
                                                svgElementId: "check";

                                            }
                                        }

                                    }

                                    onClicked : {
                                        if (rootItem.controller) {
                                            if (checked) {
                                                rootItem.controller.showSessionForProtocol(modelData)
                                            }
                                            else {
                                                rootItem.controller.hideSessionForProtocol(modelData)
                                            }

                                            // update "all agents" checkbox state
                                            // reset isPartiallyChecked and checkedState properties
                                            dropDownProtocol.isPartiallyChecked = false;
                                            dropDownProtocol.checkAllState = Qt.Unchecked;

                                            if (rootItem.controller && (rootItem.controller.selectedProtocolNameList.length > 0))
                                            {
                                                if (rootItem.controller.selectedProtocolNameList.length === rootItem.controller.protocolNameList.length) {
                                                    dropDownProtocol.checkAllState = Qt.Checked;
                                                }
                                                else {
                                                    dropDownProtocol.isPartiallyChecked = true;
                                                }
                                            }
                                        }
                                    }

                                    Connections {
                                        target : dropDownProtocol.popup
                                        onOpened : {
                                            // update agents checkboxes states when the pop up is opening
                                            if (controller) {
                                                filterProtocolCB.checked = controller.areShownSessionsForProtocol(modelData);
                                            }
                                        }
                                    }

                                    Connections {
                                        target : rootItem
                                        onClickAllProtocol : {
                                            // update agents checkboxes states when the "pop up is opening   "All Agents" check box is selected or unselected
                                            if (controller) {
                                                filterProtocolCB.checked = controller.areShownSessionsForProtocol(modelData);
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }

                    Item {
                        width: mainView.creationDateTimeColumnWidth
                        height: parent.height

                        Text {
                            anchors {
                                left: parent.left
                                leftMargin: 15
                                verticalCenter: parent.verticalCenter
                            }

                            text: qsTr("Creation date")
                            color: IngeScapeTheme.whiteColor
                            font {
                                family: IngeScapeTheme.labelFontFamily
                                pixelSize: 18
                                weight: Font.Black
                            }
                        }
                    }

                    Item {
                        width: mainView.sessionColumnWidth
                        height: parent.height

                        Text {
                            anchors {
                                left: parent.left
                                leftMargin: 15
                                verticalCenter: parent.verticalCenter
                            }

                            text: qsTr("Description")
                            color: IngeScapeTheme.whiteColor
                            font {
                                family: IngeScapeTheme.labelFontFamily
                                pixelSize: 18
                                weight: Font.Black
                            }
                        }
                    }
                }
            }

            Rectangle {
                id: rectExportRow
                anchors {
                    top: listHeader.bottom
                    left: parent.left
                    right: parent.right
                }
                height: rootItem._isSelectingSessionsToExport && rootItem.controller.isRecorderON ? 40 : 0
                clip: true

                color: IngeScapeAssessmentsTheme.blueButton_pressed
                /*border {
                    color: IngeScapeTheme.veryLightGreyColor
                    width: 1
                }*/

                Behavior on height {
                    NumberAnimation {
                        duration: rootItem._appearanceAnimationDuration
                    }
                }

                Text {
                    anchors {
                        left: parent.left
                        leftMargin: 5
                        verticalCenter: parent.verticalCenter
                    }

                    text: qsTr("Select sessions to export")

                    color: IngeScapeTheme.whiteColor
                    font {
                        family: IngeScapeTheme.labelFontFamily
                        pixelSize: 18
                        weight: Font.Black
                    }
                }

                Button {
                    id: cancelButton

                    property var boundingBox: IngeScapeTheme.svgFileIngeScape.boundsOnElement("button");

                    anchors {
                        right: exportToFileButton.left
                        rightMargin: 10
                        verticalCenter: parent.verticalCenter
                    }
                    height: boundingBox.height
                    width: boundingBox.width

                    activeFocusOnPress: true

                    style: ButtonStyle {
                        background: Rectangle {
                            anchors.fill: parent
                            radius: 5
                            color: control.pressed ? IngeScapeTheme.lightGreyColor : (control.hovered ? IngeScapeTheme.veryLightGreyColor : "transparent")
                        }

                        label: Text {
                            text: "Cancel"
                            verticalAlignment: Text.AlignVCenter
                            horizontalAlignment: Text.AlignHCenter
                            color: IngeScapeAssessmentsTheme.regularDarkBlueHeader

                            font {
                                family: IngeScapeTheme.textFontFamily
                                weight: Font.Medium
                                pixelSize: 16
                            }
                        }
                    }
                    /*style: IngeScapeAssessmentsButtonStyle {
                            text: "Cancel"
                        }*/

                    onClicked: {
                        _isSelectingSessionsToExport = false;
                    }
                }

                Button {
                    id: exportToFileButton

                    property var boundingBox: IngeScapeTheme.svgFileIngeScape.boundsOnElement("button");

                    anchors {
                        right: parent.right
                        rightMargin: 10
                        verticalCenter: parent.verticalCenter
                    }
                    height: boundingBox.height
                    width: 150 // boundingBox.width

                    activeFocusOnPress: true
                    enabled: rootItem.controller && (rootItem.controller.selectedSessions.count > 0)

                    style: IngeScapeAssessmentsButtonStyle {
                        text: "Export to file"
                    }

                    onClicked: {
                        if (rootItem.controller)
                        {
                            //console.log("QML: Export " + rootItem.controller.selectedSessions.count + " selected sessions (to file)...");

                            // Export selected sessions
                            rootItem.controller.exportSelectedSessions();
                        }
                    }
                }

            }

            Rectangle {
                anchors {
                    top: rectExportRow.bottom
                    bottom: parent.bottom
                    left: parent.left
                    right: parent.right
                }

                color: IngeScapeTheme.whiteColor

                ScrollView {
                    id: taskInstanceScrollView
                    anchors {
                        fill: parent
                        rightMargin: -taskInstanceScrollView.scrollBarSize -taskInstanceScrollView.verticalScrollbarMargin
                    }

                    property int scrollBarSize: 11
                    property int verticalScrollbarMargin: 3

                    style: IngeScapeAssessmentsScrollViewStyle {
                        scrollBarSize: taskInstanceScrollView.scrollBarSize
                        verticalScrollbarMargin: taskInstanceScrollView.verticalScrollbarMargin
                    }

                    // Prevent drag overshoot on Windows
                    flickableItem.boundsBehavior: Flickable.OvershootBounds

                    Column {
                        id: taskInstanceColumn
                        width: taskInstanceScrollView.width - (taskInstanceScrollView.scrollBarSize + taskInstanceScrollView.verticalScrollbarMargin)
                        height: childrenRect.height
                        spacing: 0

                        Repeater {
                            model: rootItem.experimentation ? rootItem.controller.sessionFilteredList : null

                            delegate: TaskInstanceInList {
                                isSelectingSessionsToExport: rootItem._isSelectingSessionsToExport && rootItem.controller.isRecorderON
                                appearanceAnimationDuration: rootItem._appearanceAnimationDuration

                                selectionColumnWidth: mainView.selectionColumnWidth
                                subjectColumnWidth: mainView.subjectColumnWidth
                                protocolColumnWidth: mainView.protocolColumnWidth
                                creationDateTimeColumnWidth: mainView.creationDateTimeColumnWidth
                                sessionColumnWidth: mainView.sessionColumnWidth
                                //buttonsColumnWidth: mainView.buttonsColumnWidth

                                anchors {
                                    left: parent.left
                                    right: parent.right
                                }

                                modelM: model.QtObject

                                //
                                // Slots
                                //
                                onOpenTaskInstance: {
                                    if (rootItem.controller && modelM) {
                                        rootItem.controller.openTaskInstance(modelM);
                                    }
                                }

                                onDeleteTaskInstance: {
                                    deleteTaskInstancePopup.taskInstance = modelM
                                    deleteTaskInstancePopup.open()
                                }

                                onIsSelectedSessionChanged: {
                                    if (rootItem.controller)
                                    {
                                        if (isSelectedSession) {
                                            //console.log("session " + modelM.name + " selected");
                                            rootItem.controller.selectedSessions.insert(rootItem.controller.selectedSessions.count, modelM);
                                        }
                                        else {
                                            //console.log("session " + modelM.name + " UN-selected");
                                            rootItem.controller.selectedSessions.remove(modelM);
                                        }
                                    }
                                }
                            }
                        }
                    }
                }

                Rectangle {
                    id: bottomShadow
                    anchors {
                        left: parent.left
                        right: parent.right
                        bottom: parent.bottom
                    }
                    height: 4
                    gradient: Gradient {
                        GradientStop { position: 0.0; color: IngeScapeTheme.whiteColor; }
                        GradientStop { position: 1.0; color: IngeScapeTheme.darkGreyColor; }
                    }
                }
            }
        }


        //
        // Create Experimentation Popup
        //
        Popup.CreateTaskInstancePopup {
            id: createSessionPopup

            experimentationController: rootItem.controller
            experimentation: rootItem.experimentation
        }
    }

    Rectangle {
        id: headerBottomShadow

        anchors {
            top: headerItem.bottom
            left: headerItem.left
            right: headerItem.right
        }

        height: 8

        gradient: Gradient {
            GradientStop { position: 0.0; color: IngeScapeTheme.darkGreyColor; }
            GradientStop { position: 1.0; color: "transparent"; }
        }

    }


    //
    // Subjects View (popup)
    //
    Subject.SubjectsView {
        id: subjectsViewPopup

        anchors.centerIn: parent

        width: parent.width - subScreensMargin
        height: parent.height - subScreensMargin

        subjectController: IngeScapeAssessmentsC.subjectsC
    }


    //
    // Protocols View (popup)
    //
    Task.TasksView {
        id: protocolsViewPopup

        anchors.centerIn: parent

        width: parent.width - subScreensMargin
        height: parent.height - subScreensMargin

        taskController: IngeScapeAssessmentsC.tasksC
    }


    //
    // Export View (popup)
    //
    /*Export.ExportView {
        id: exportViewPopup

        anchors.centerIn: parent

        width: parent.width - subScreensMargin
        height: parent.height - subScreensMargin

        controller: IngeScapeAssessmentsC.exportC
    }*/


    Popup.DeleteConfirmationPopup {
        id: deleteTaskInstancePopup

        property var taskInstance: null

        showPopupTitle: false
        anchors.centerIn: parent

        text: qsTr("Are you sure you want to delete the task instance %1 ?").arg(taskInstance ? taskInstance.name : "")

        height: 160
        width: 470

        onValidated: {
            if (rootItem.controller && taskInstance) {
                rootItem.controller.deleteTaskInstance(taskInstance);
            }
            close()
        }

        onCanceled: {
            close()
        }

    }

}
