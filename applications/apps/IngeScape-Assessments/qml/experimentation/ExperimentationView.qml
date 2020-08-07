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
 *      Chloé Roumieu      <roumieu@ingenuity.io>
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
import "../protocol" as Protocol
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

    property ExperimentationController experimentationC: null;
    property ExperimentationM experimentation: experimentationC ? experimentationC.currentExperimentation : null;

    property bool isEditingName: false

    property int rightMarginToAvoidToBeHiddenByNetworkConnectionInfo: 220

    property int subScreensMargin: 35

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

        property real sessionColumnWidth: sessionsPanel.width - sessionsScrollView.scrollBarSize - sessionsScrollView.verticalScrollbarMargin
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
                        text: qsTr("PROTOCOLS")

                        releasedID: "tasks"
                        pressedID: releasedID
                        rolloverID: releasedID
                        disabledID: releasedID
                    }

                    onClicked: {
                        console.log("QML: Open the 'Protocols View' popup");
                        if (rootItem.experimentationC)
                        {
                            rootItem.experimentationC.isSelectingSessions = false // Close mode "export session"
                        }
                        protocolsViewPopup.open();
                    }
                }

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
                        if (rootItem.experimentationC)
                        {
                            rootItem.experimentationC.isSelectingSessions = false // Close mode "export session"
                        }
                        subjectsViewPopup.open()
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
            }
        }


        //
        // Sessions panel
        //
        Item {
            id: sessionsPanel

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
                    rootItem.experimentationC.isSelectingSessions = false // Close mode "export session"
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

                            enabled: rootItem.experimentationC
                            visible: enabled

                            fileCache: IngeScapeTheme.svgFileIngeScape

                            pressedID: releasedID + "-pressed"
                            releasedID: "export"
                            disabledID : releasedID + "-disabled"

                            onClicked: {
                                //console.log("QML: Open the 'Export Sessions' mode ");
                                if (rootItem.experimentationC) {
                                    rootItem.experimentationC.isSelectingSessions = ! rootItem.experimentationC.isSelectingSessions
                                }
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
                            visible: rootItem.experimentationC ? !rootItem.experimentationC.isSelectingSessions : false
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
                            //enabled: rootItem.experimentationC && rootItem.experimentationC.allAgentNamesList.length > 0
                            visible: rootItem.experimentationC ? rootItem.experimentationC.isSelectingSessions : false
                            enabled: visible
                            model: rootItem.experimentation ? rootItem.experimentation.allSubjects : 0

                            placeholderText: enabled ? "- Select a subject -" : ""

                            text:{
                                if (enabled && rootItem.experimentation && rootItem.experimentationC && (rootItem.experimentationC.selectedSubjectIdListToFilter.length > 0))
                                {
                                    if(rootItem.experimentationC.selectedSubjectIdListToFilter.length === 1)
                                    {
                                        "- " + rootItem.experimentationC.selectedSubjectIdListToFilter + " -"
                                    }
                                    else if(rootItem.experimentationC.selectedSubjectIdListToFilter.length < rootItem.experimentation.allSubjects.length)
                                    {
                                        "- " + rootItem.experimentationC.selectedSubjectIdListToFilter.length + " subjects selected -"
                                    }
                                    else
                                    {
                                        "- All subjects selected -"
                                    }
                                }
                                else
                                {
                                    ""
                                }
                            }

                            checkAllText: " All subjects"

                            onCheckAll: {
                                rootItem.experimentationC.addAllSubjectsToFilterSessions();
                                clickAllSubject();
                            }

                            onUncheckAll: {
                                rootItem.experimentationC.removeAllSubjectsToFilterSessions();
                                clickAllSubject();
                            }

                            onPopupOpen: {
                                // update "all subjects" checkbox state
                                // reset isPartiallyChecked and checkedState properties
                                isPartiallyChecked = false;
                                checkAllState = Qt.Unchecked;

                                if (rootItem.experimentation && rootItem.experimentationC && (rootItem.experimentationC.selectedSubjectIdListToFilter.length > 0))
                                {
                                    if (rootItem.experimentationC.selectedSubjectIdListToFilter.length === rootItem.experimentation.allSubjects.count) {
                                        checkAllState = Qt.Checked;
                                    }
                                    else {
                                        isPartiallyChecked = true;
                                    }
                                }
                            }

                            delegate: Item {
                                id: delegateSubjectCombolist

                                anchors {
                                    left: parent.left
                                    right: parent.right
                                }

                                width: dropDownSubject.comboButton.width
                                height: dropDownSubject.comboButton.height

                                property SubjectM subjectM : rootItem.experimentation ? rootItem.experimentation.allSubjects.get(index) : null

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

                                            text: delegateSubjectCombolist.subjectM ? " " + delegateSubjectCombolist.subjectM.displayedId : ""
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
                                        if (rootItem.experimentationC && delegateSubjectCombolist.subjectM) {
                                            if (checked) {
                                                rootItem.experimentationC.addOneSubjectToFilterSessions(delegateSubjectCombolist.subjectM.displayedId)
                                            }
                                            else {
                                                rootItem.experimentationC.removeOneSubjectToFilterSessions(delegateSubjectCombolist.subjectM.displayedId)
                                            }

                                            // update "all subjects" checkbox state
                                            // reset isPartiallyChecked and checkedState properties
                                            dropDownSubject.isPartiallyChecked = false;
                                            dropDownSubject.checkAllState = Qt.Unchecked;

                                            if (rootItem.experimentationC && rootItem.experimentation && (rootItem.experimentationC.selectedSubjectIdListToFilter.length > 0))
                                            {
                                                if (rootItem.experimentationC.selectedSubjectIdListToFilter.length === rootItem.experimentation.allSubjects.count) {
                                                    dropDownSubject.checkAllState = Qt.Checked;
                                                }
                                                else {
                                                    dropDownSubject.isPartiallyChecked = true;
                                                }
                                            }
                                        }
                                    }

                                    Connections {
                                        target: dropDownSubject.popup

                                        function onOpened() {
                                            // update subjects checkboxes states when the pop up is opening
                                            if (experimentationC && delegateSubjectCombolist.subjectM) {
                                                filterSubjectCB.checked = experimentationC.isSubjectFilterSessions(delegateSubjectCombolist.subjectM.displayedId);
                                            }
                                        }
                                    }

                                    Connections {
                                        target: rootItem

                                        function onClickAllSubject() {
                                            // update subjects checkboxes states when the "pop up is opening   "All subjects" check box is selected or unselected
                                            if (rootItem.experimentationC) {
                                                filterSubjectCB.checked = rootItem.experimentationC.isSubjectFilterSessions(delegateSubjectCombolist.subjectM.displayedId);
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

                            visible: rootItem.experimentationC ? !rootItem.experimentationC.isSelectingSessions : false
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
                            //enabled: rootItem.experimentationC && rootItem.experimentationC.allAgentNamesList.length > 0
                            visible: rootItem.experimentationC ? rootItem.experimentationC.isSelectingSessions : false
                            enabled: visible
                            model: rootItem.experimentation ? rootItem.experimentation.allProtocols : 0

                            placeholderText: enabled ? "- Select a protocol -" : ""

                            text:{
                                if (enabled && rootItem.experimentationC && rootItem.experimentation && (rootItem.experimentationC.selectedProtocolNameListToFilter.length > 0))
                                {
                                    if (rootItem.experimentationC.selectedProtocolNameListToFilter.length === 1)
                                    {
                                        "- " + rootItem.experimentationC.selectedProtocolNameListToFilter + " -"
                                    }
                                    else if (rootItem.experimentationC.selectedProtocolNameListToFilter.length < rootItem.experimentation.allProtocols.count)
                                    {
                                        "- " + rootItem.experimentationC.selectedProtocolNameListToFilter.length + " protocols selected -"
                                    }
                                    else
                                    {
                                        "- All protocols selected -"
                                    }
                                }
                                else
                                {
                                    ""
                                }
                            }
                            checkAllText: " All protocol"

                            onCheckAll: {
                                rootItem.experimentationC.addAllProtocolsToFilterSessions();
                                rootItem.clickAllProtocol();
                            }

                            onUncheckAll: {
                                rootItem.experimentationC.removeAllProtocolsToFilterSessions();
                                rootItem.clickAllProtocol();
                            }

                            onPopupOpen: {
                                // update "all agents" checkbox state
                                // reset isPartiallyChecked and checkedState properties
                                isPartiallyChecked = false;
                                checkAllState = Qt.Unchecked;

                                if (rootItem.experimentationC && rootItem.experimentation && (rootItem.experimentationC.selectedProtocolNameListToFilter.length > 0))
                                {
                                    if (rootItem.experimentationC.selectedProtocolNameListToFilter.length === rootItem.experimentation.allProtocols.count) {
                                        checkAllState = Qt.Checked;
                                    }
                                    else {
                                        isPartiallyChecked = true;
                                    }
                                }
                            }

                            delegate: Item {
                                id: delegateProtocolComboList
                                anchors {
                                    left: parent.left
                                    right: parent.right
                                }

                                width: dropDownProtocol.comboButton.width
                                height: dropDownProtocol.comboButton.height

                                property ProtocolM protocolM : rootItem.experimentation ? rootItem.experimentation.allProtocols.get(index) : null

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

                                            text: delegateProtocolComboList.protocolM ? " " + delegateProtocolComboList.protocolM.name : ""
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
                                        if (rootItem.experimentationC && delegateProtocolComboList.protocolM)
                                        {
                                            if (checked)
                                            {
                                                rootItem.experimentationC.addOneProtocolToFilterSessions(delegateProtocolComboList.protocolM.name)
                                            }
                                            else
                                            {
                                                rootItem.experimentationC.removeOneProtocolToFilterSessions(delegateProtocolComboList.protocolM.name)
                                            }

                                            // update "all agents" checkbox state
                                            // reset isPartiallyChecked and checkedState properties
                                            dropDownProtocol.isPartiallyChecked = false;
                                            dropDownProtocol.checkAllState = Qt.Unchecked;

                                            if (rootItem.experimentationC && rootItem.experimentation && (rootItem.experimentationC.selectedProtocolNameListToFilter.length > 0))
                                            {
                                                if (rootItem.experimentationC.selectedProtocolNameListToFilter.length === rootItem.experimentation.allProtocols.count)
                                                {
                                                    dropDownProtocol.checkAllState = Qt.Checked;
                                                }
                                                else
                                                {
                                                    dropDownProtocol.isPartiallyChecked = true;
                                                }
                                            }
                                        }
                                    }

                                    Connections {
                                        target: dropDownProtocol.popup

                                        function onOpened() {
                                            // update agents checkboxes states when the pop up is opening
                                            if (rootItem.experimentationC && delegateProtocolComboList.protocolM)
                                            {
                                                filterProtocolCB.checked = rootItem.experimentationC.isProtocolFilterSessions(delegateProtocolComboList.protocolM.name);
                                            }
                                        }
                                    }

                                    Connections {
                                        target: rootItem

                                        function onClickAllProtocol() {
                                            // update agents checkboxes states when the "pop up is opening  "All Agents" check box is selected or unselected
                                            if (rootItem.experimentationC && delegateProtocolComboList.protocolM)
                                            {
                                                filterProtocolCB.checked = rootItem.experimentationC.isProtocolFilterSessions(delegateProtocolComboList.protocolM.name);
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
                height: (rootItem.experimentationC && rootItem.experimentationC.isSelectingSessions) ? 40 : 0
                clip: true

                color: rootItem.experimentationC.isRecorderON ? IngeScapeAssessmentsTheme.blueButton_pressed : IngeScapeTheme.veryLightGreyColor

                Behavior on height {
                    NumberAnimation {
                        duration: rootItem._appearanceAnimationDuration
                    }
                }

                Behavior on color {
                    enabled: !rootItem.experimentationC.isRecorderON
                    ColorAnimation {
                        duration: rootItem._appearanceAnimationDuration
                    }
                }

                Text {
                    anchors {
                        left: parent.left
                        leftMargin: 5
                        verticalCenter: parent.verticalCenter
                    }

                    visible: rootItem.experimentationC.isRecorderON

                    text: qsTr("Select sessions to export")

                    color: IngeScapeTheme.whiteColor
                    font {
                        family: IngeScapeTheme.labelFontFamily
                        pixelSize: 18
                        weight: Font.Black
                    }
                }

                Item {
                    anchors {
                        verticalCenter: parent.verticalCenter
                        left: parent.left
                        leftMargin: 5
                    }

                    height: childrenRect.height
                    width: childrenRect.width

                    visible: !rootItem.experimentationC.isRecorderON

                    I2SvgItem {
                        id: errorMessageIcon
                        svgFileCache: IngeScapeAssessmentsTheme.svgFileIngeScapeAssessments
                        svgElementId: "warning"
                    }

                    Text {
                        id: errorMessageText
                        anchors {
                            verticalCenter: errorMessageIcon.verticalCenter
                            left : errorMessageIcon.right
                            leftMargin: 10
                        }

                        wrapMode: Text.WordWrap
                        text: "No Recorder found on network device '" + IngeScapeAssessmentsC.networkDevice + "' and port " + IngeScapeAssessmentsC.port + ". Unable to export sessions."
                        color: IngeScapeTheme.orangeColor
                        font {
                            family: IngeScapeTheme.textFontFamily
                            pixelSize: 16
                            bold: true
                        }
                    }
                }

                Text {
                    id: exportSessionsMessage

                    anchors {
                        verticalCenter: parent.verticalCenter
                        //horizontalCenter: parent.horizontalCenter
                        right: parent.right
                        rightMargin: 10
                    }

                    text: rootItem.experimentationC.exportSessionsMessage
                    visible: (exportSessionsMessage.text.length > 0)

                    color: IngeScapeTheme.whiteColor
                    font {
                        family: IngeScapeTheme.textFontFamily
                        pixelSize: 18
                        bold: true
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

                    visible: !exportSessionsMessage.visible

                    activeFocusOnPress: true

                    style: IngeScapeAssessmentsButtonStyle {
                        text: "Cancel"
                    }

                    onClicked: {
                        if (rootItem.experimentationC) {
                            rootItem.experimentationC.isSelectingSessions = false;
                        }
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

                    visible: !exportSessionsMessage.visible

                    activeFocusOnPress: true
                    enabled: rootItem.experimentationC && (rootItem.experimentationC.selectedSessions.count > 0)

                    style: IngeScapeAssessmentsButtonStyle {
                        text: "Export to file"
                    }

                    onClicked: {
                        exportViewPopup.open();
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
                    id: sessionsScrollView
                    anchors {
                        fill: parent
                        rightMargin: -sessionsScrollView.scrollBarSize -sessionsScrollView.verticalScrollbarMargin
                    }

                    property int scrollBarSize: 11
                    property int verticalScrollbarMargin: 3

                    style: IngeScapeAssessmentsScrollViewStyle {
                        scrollBarSize: sessionsScrollView.scrollBarSize
                        verticalScrollbarMargin: sessionsScrollView.verticalScrollbarMargin
                    }

                    // Prevent drag overshoot on Windows
                    flickableItem.boundsBehavior: Flickable.OvershootBounds

                    Column {
                        id: sessionColumn

                        width: sessionsScrollView.width - (sessionsScrollView.scrollBarSize + sessionsScrollView.verticalScrollbarMargin)
                        height: childrenRect.height
                        spacing: 0

                        Repeater {
                            model: rootItem.experimentationC ? rootItem.experimentationC.sessionFilteredList
                                                             : null

                            delegate: SessionInList {
                                isSelectingSessionsToExport: rootItem.experimentationC && rootItem.experimentationC.isSelectingSessions && rootItem.experimentationC.isRecorderON
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
                                onOpenSessionAsked: {
                                    if (rootItem.experimentationC && modelM) {
                                        rootItem.experimentationC.openSession(modelM);
                                    }
                                }

                                onDeleteSessionAsked: {
                                    deleteSessionPopup.session = modelM
                                    deleteSessionPopup.open()
                                }

                                onIsSelectedSessionChanged: {
                                    if (rootItem.experimentationC)
                                    {
                                        if (isSelectedSession) {
                                            console.log("session " + modelM.name + " selected");
                                            rootItem.experimentationC.selectedSessions.insert(rootItem.experimentationC.selectedSessions.count, modelM);
                                        }
                                        else {
                                            console.log("session " + modelM.name + " UN-selected");
                                            rootItem.experimentationC.selectedSessions.remove(modelM);
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
        Popup.CreateSessionPopup {
            id: createSessionPopup

            experimentationController: rootItem.experimentationC
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
    Protocol.ProtocolsView {
        id: protocolsViewPopup

        anchors.centerIn: parent

        width: parent.width - subScreensMargin
        height: parent.height - subScreensMargin

        protocolsController: IngeScapeAssessmentsC.protocolsC
    }


    //
    // Export View (popup)
    //
    Export.ExportView {
        id: exportViewPopup

        anchors.centerIn: parent

        //width: parent.width - subScreensMargin
        //height: parent.height - subScreensMargin

        //controller: IngeScapeAssessmentsC.exportC

        onExportAllOutputs: {
            if (rootItem.experimentationC)
            {
                //console.log("QML: Export ALL outputs of " + rootItem.experimentationC.selectedSessions.count + " selected sessions (to file)...");
                rootItem.experimentationC.exportSelectedSessions(false);
            }
        }

        onExportOnlyDependentVariables: {
            if (rootItem.experimentationC)
            {
                //console.log("QML: Export ONLY Dependent Variables of " + rootItem.experimentationC.selectedSessions.count + " selected sessions (to file)...");
                rootItem.experimentationC.exportSelectedSessions(true);
            }
        }
    }


    Popup.DeleteConfirmationPopup {
        id: deleteSessionPopup

        property var session: null

        showPopupTitle: false
        anchors.centerIn: parent

        text: qsTr("Are you sure you want to delete the session %1 ?").arg(session ? session.name : "")

        height: 160
        width: 470

        onValidated: {
            if (rootItem.experimentationC && session) {
                rootItem.experimentationC.deleteSession(session);
            }
            close();
        }

        onCanceled: {
            close();
        }
    }
}
