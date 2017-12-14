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
import QtQuick.Window 2.3
import I2Quick 1.0

import MASTIC 1.0
import "../theme" as Theme;

Window {
    id: rootItem

    height: minimumHeight
    width: minimumWidth

    minimumWidth: 475
    minimumHeight: MasticTheme.appMinHeight

    maximumWidth: 475
    maximumHeight: MasticTheme.appMinHeight

    flags: Qt.Tool

//    automaticallyOpenWhenCompleted: true
//    isModal: false
//    dismissOnOutsideTap : false;
//    keepRelativePositionToInitialParent : false;


    //--------------------------------
    //
    // Properties
    //
    //--------------------------------

    // action model
    property var actionM: panelController?  panelController.editedAction : null;
    // action view model
    property var actionVM: panelController? panelController.editedViewModel : null;

    // our scenario controller
    property var controller: null;
    // our panel controller
    property var panelController: null;


    //--------------------------------
    //
    // Signals
    //
    //--------------------------------

    // Emitted when user pressed our popup
    //signal bringToFront();

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
            width: 1
            color: MasticTheme.editorsBackgroundBorderColor
        }
        color: MasticTheme.editorsBackgroundColor
//        clip : true

//        // catch events
//        MultiPointTouchArea {
//            anchors.fill: parent
//        }

        // drag Area
//        I2CustomRectangle {
//            id : dragRect
//            anchors {
//                top : parent.top
//                left : parent.left
//                right : parent.right
//                margins : 2
//            }
//            height : 50
//            topLeftRadius : 5
//            topRightRadius : 5

//            color :  /dragMouseArea.pressed? MasticTheme.editorsBackgroundColor : MasticTheme.darkBlueGreyColor

//            MouseArea {
//                id : dragMouseArea
//                hoverEnabled: true
//                anchors.fill: parent
//                drag.target: rootItem

//                drag.minimumX : - rootItem.width/2
//                drag.maximumX : rootItem.parent.width - rootItem.width/2
//                drag.minimumY :  0
//                drag.maximumY :  rootItem.parent.height - rootItem.height/2

//                onPressed: {
//                    // Emit signal "bring to front"
//                    rootItem.bringToFront();
//                }
//            }
//        }

        // separator
        Rectangle {
            anchors {
                top : parent.top
                topMargin: 50
                left : parent.left
                right : parent.right
            }
            height : 1
            color : MasticTheme.editorsBackgroundBorderColor
        }

        Item {
            anchors {
                fill : parent
                margins : 20
            }

            Text {
                id : titleTxt

                anchors {
                    left : parent.left
                    top : parent.top
                    right : parent.right
                }

                text : "Action"
                elide: Text.ElideRight
                color: MasticTheme.definitionEditorsLabelColor
                font {
                    family: MasticTheme.textFontFamily
                    pixelSize: 23
                    weight: Font.Medium
                }
            }

//            Button {
//                id: btnCloseEditor

//                anchors {
//                    top: parent.top
//                    right : parent.right
//                }

//                activeFocusOnPress: true
//                style: Theme.LabellessSvgButtonStyle {
//                    fileCache: MasticTheme.svgFileMASTIC

//                    pressedID: releasedID + "-pressed"
//                    releasedID: "closeEditor"
//                    disabledID : releasedID
//                }

//                onClicked: {
//                    // Close our popup
//                    rootItem.close();
//                }
//            }

            //
            // Name
            //
            Item {
                id : nameItem
                anchors {
                    left : parent.left
                    right : parent.right
                    top : titleTxt.bottom
                    topMargin : 25
                }

                height: textFieldName.height

                TextField {
                    id: textFieldName

                    anchors {
                        left : parent.left
                        verticalCenter : parent.verticalCenter
                    }

                    height: 25
                    width: 185
                    verticalAlignment: TextInput.AlignVCenter
                    text: actionM ? actionM.name : ""

                    style: I2TextFieldStyle {
                        backgroundColor: MasticTheme.darkBlueGreyColor
                        borderColor: MasticTheme.whiteColor;
                        borderErrorColor: MasticTheme.redColor
                        radiusTextBox: 1
                        borderWidth: 0;
                        borderWidthActive: 1
                        textIdleColor: MasticTheme.whiteColor;
                        textDisabledColor: MasticTheme.darkGreyColor;

                        padding.left: 3
                        padding.right: 3

                        font {
                            pixelSize:15
                            family: MasticTheme.textFontFamily
                        }

                    }

                    onTextChanged: {
                        if (activeFocus &&  actionM ) {
                            actionM.name = text;
                        }
                    }

                    onActiveFocusChanged: {
                        if (!activeFocus) {
                            // Move cursor to our first character when we lose focus
                            // (to always display the beginning or our text instead of
                            // an arbitrary part if our text is too long)
                            cursorPosition = 0;
                        } else {
                            textFieldName.selectAll();
                        }
                    }


                    Binding {
                        target : textFieldName
                        property :  "text"
                        value : if (actionM ) {
                                    actionM.name
                                }
                                else {
                                    "";
                                }
                    }
                }
            }


            //
            // Start Time
            //
            Item {
                id : startTimeItem
                anchors {
                    left : parent.left
                    right : parent.right
                    top : nameItem.bottom
                    topMargin : 15
                }
                height : textFieldStartTime.height

                visible : actionVM !== null

                Text {
                    id : textstartTime
                    anchors {
                        left : parent.left
                        verticalCenter : parent.verticalCenter
                    }

                    text : "Start Time:"

                    color: MasticTheme.lightGreyColor
                    font {
                        family: MasticTheme.textFontFamily
                        pixelSize: 16
                    }
                }


                TextField {
                    id: textFieldStartTime

                    anchors {
                        left : textstartTime.right
                        leftMargin: 10
                        verticalCenter : parent.verticalCenter
                    }

                    height: 25
                    width: 105
                    verticalAlignment: TextInput.AlignVCenter
                    text: actionVM ? actionVM.startTimeString : "00:00:00.000"
                    inputMask: "00:00:00.000"
                    inputMethodHints: Qt.ImhTime
                    validator: RegExpValidator { regExp: /^(?:(?:([01]?\d|2[0-3]):)?([0-5]?\d):)?([0-5]?\d).([0-9\s][0-9\s][0-9\s])$/ }

                    style: I2TextFieldStyle {
                        backgroundColor: MasticTheme.darkBlueGreyColor
                        borderColor: MasticTheme.whiteColor;
                        borderErrorColor: MasticTheme.redColor
                        radiusTextBox: 1
                        borderWidth: 0;
                        borderWidthActive: 1
                        textIdleColor: MasticTheme.whiteColor;
                        textDisabledColor: MasticTheme.darkGreyColor;

                        padding.left: 3
                        padding.right: 3

                        font {
                            pixelSize:15
                            family: MasticTheme.textFontFamily
                        }

                    }

                    onActiveFocusChanged: {
                        if (!activeFocus) {
                            // Move cursor to our first character when we lose focus
                            // (to always display the beginning or our text instead of
                            // an arbitrary part if our text is too long)
                            cursorPosition = 0;
                        } else {
                            textFieldStartTime.selectAll();
                        }
                    }


                    onTextChanged: {
                        if (activeFocus &&  actionVM ) {
                            actionVM.startTimeString = text;
                        }
                    }

                    Binding {
                        target : textFieldStartTime
                        property :  "text"
                        value : if (actionVM) {
                                    actionVM.startTimeString
                                }
                                else {
                                    "";
                                }
                    }
                }

            }


            //
            // Conditions
            //
            Item {
                id : conditionsItem
                anchors {
                    left : parent.left
                    right : parent.right
                    top : (startTimeItem.visible) ? startTimeItem.bottom : nameItem.bottom
                    topMargin: 14
                }
                height  : isOpened ? 230 : 37

                Behavior on height {
                    NumberAnimation {}
                }

                property bool isOpened : false

                Connections {
                    target : rootItem
                    Component.onCompleted : {
                        // make the conditions list visible if there are conditions
                        if (actionM && actionM.conditionsList.count > 0) {
                            conditionsItem.isOpened = true;
                        }
                    }
                }

                //Title
                MouseArea {
                    id : titleMouseArea
                    anchors {
                        left : parent.left
                        right : parent.right
                    }
                    height : 29

                    onClicked: {
                        conditionsItem.isOpened = !conditionsItem.isOpened;
                    }

                    I2SvgItem {
                        id : arrow
                        anchors {
                            left : parent.left
                            verticalCenter: titleCdt.verticalCenter
                        }

                        svgFileCache : MasticTheme.svgFileMASTIC;
                        svgElementId: "arrowWhite"

                        rotation: conditionsItem.isOpened? 0 : 270
                    }

                    Text {
                        id : titleCdt
                        anchors {
                            left : arrow.right
                            leftMargin: 6
                            right : parent.right
                            verticalCenter: parent.verticalCenter
                        }

                        text : "Conditions"

                        color: titleMouseArea.containsPress? MasticTheme.lightGreyColor : MasticTheme.whiteColor
                        font {
                            family: MasticTheme.textFontFamily
                            pixelSize: 19
                        }
                    }

                }

                // separator
                Rectangle {
                    id : separatorCdt
                    anchors {
                        left : parent.left
                        right : parent.right
                        top : titleMouseArea.bottom
                    }
                    height : 1
                    color : MasticTheme.whiteColor
                }

                /// Validity duration
                Item {
                    id : validityDuration
                    anchors {
                        left : parent.left
                        right : parent.right
                        top : separatorCdt.bottom
                        topMargin: 8
                    }

                    height: validityDurationCombo.height

                    visible : conditionsItem.isOpened
                    enabled : visible

                    Text {
                        id : textValidity
                        anchors {
                            left : parent.left
                            verticalCenter : parent.verticalCenter
                        }

                        text : "Conditions shall be checked"

                        color: MasticTheme.whiteColor
                        font {
                            family: MasticTheme.textFontFamily
                            pixelSize: 16
                        }
                    }

                    Row {
                        anchors {
                            verticalCenter : parent.verticalCenter
                            left : textValidity.right
                            leftMargin: 10
                        }
                        spacing : 6

                        MasticComboBox {
                            id : validityDurationCombo

                            anchors {
                                verticalCenter : parent.verticalCenter
                            }

                            height : 25
                            width : 115

                            model : controller ? controller.validationDurationsTypesList : 0
                            function modelToString(model)
                            {
                                return model.name;
                            }


                            Binding {
                                target : validityDurationCombo
                                property : "selectedItem"
                                value : if (actionM && controller)
                                        {
                                            controller.validationDurationsTypesList.getItemWithValue(actionM.validityDurationType);
                                        }
                                        else {
                                            null;
                                        }
                            }


                            onSelectedItemChanged:
                            {
                                if (actionM)
                                {
                                    actionM.validityDurationType = validityDurationCombo.selectedItem.value;
                                }
                            }

                        }

                        TextField {
                            id: textFieldValidity

                            anchors {
                                verticalCenter : parent.verticalCenter
                            }

                            visible :  actionM && actionM.validityDurationType === ValidationDurationType.CUSTOM
                            enabled: visible
                            height: 25
                            width: 57
                            horizontalAlignment: TextInput.AlignLeft
                            verticalAlignment: TextInput.AlignVCenter

                            text : actionM ? actionM.validityDurationString : "0.0"
                            inputMethodHints: Qt.ImhFormattedNumbersOnly
                            validator: RegExpValidator { regExp: /(\d{1,5})([.]\d{3})?$/ }

                            style: I2TextFieldStyle {
                                backgroundColor: MasticTheme.darkBlueGreyColor
                                borderColor: MasticTheme.whiteColor;
                                borderErrorColor: MasticTheme.redColor
                                radiusTextBox: 1
                                borderWidth: 0;
                                borderWidthActive: 1
                                textIdleColor: MasticTheme.whiteColor;
                                textDisabledColor: MasticTheme.darkGreyColor;

                                padding.left: 3
                                padding.right: 3

                                font {
                                    pixelSize:15
                                    family: MasticTheme.textFontFamily
                                }

                            }

                            onActiveFocusChanged: {
                                if (!activeFocus) {
                                    // Move cursor to our first character when we lose focus
                                    // (to always display the beginning or our text instead of
                                    // an arbitrary part if our text is too long)
                                    cursorPosition = 0;
                                } else {
                                    textFieldValidity.selectAll();
                                }
                            }

                            onTextChanged: {
                                if (activeFocus &&  actionM ) {
                                    actionM.validityDurationString = text;
                                }
                            }

                            Binding {
                                target : textFieldValidity
                                property :  "text"
                                value : if (actionM) {
                                            actionM.validityDurationString
                                        }
                                        else {
                                            "";
                                        }
                            }
                        }

                        Text {
                            anchors {
                                verticalCenter : parent.verticalCenter
                            }

                            visible : textFieldValidity.visible
                            text : "seconds"

                            color: MasticTheme.whiteColor
                            font {
                                family: MasticTheme.textFontFamily
                                pixelSize: 15
                            }
                        }

                    }
                }

                //
                // Conditions List
                //
                ScrollView {
                    id : scrollViewCondition

                    anchors {
                        top : validityDuration.bottom
                        topMargin: 8
                        right : parent.right
                        left : parent.left
                        bottom : parent.bottom
                    }

                    visible : conditionsItem.isOpened
                    enabled : visible

                    style: MasticScrollViewStyle {
                    }

                    // Prevent drag overshoot on Windows
                    flickableItem.boundsBehavior: Flickable.OvershootBounds


                    //
                    // Conditions List
                    //
                    contentItem: Column {
                        id : conditionsListColumn
                        spacing : 6
                        height : childrenRect.height
                        width : scrollViewCondition.width - 9 // scrollbar size + 1

                        Repeater {
                            model : actionM ? actionM.conditionsList : 0

                            Rectangle {
                                height : 62
                                anchors {
                                    right : parent.right
                                    left : parent.left
                                }

                                color : "transparent"
                                radius: 5
                                border {
                                    width : 1
                                    color : MasticTheme.blackColor
                                }

                                // my condition
                                property var myCondition: model.QtObject

                                // Condition Type
                                Row {
                                    id : rowConditionsTypes
                                    anchors {
                                        right : parent.right
                                        left : parent.left
                                        leftMargin: 10
                                        top : parent.top
                                        topMargin: 6
                                    }
                                    height : 14
                                    spacing : 15

                                    ExclusiveGroup {
                                        id : cdtTypesExclusifGroup
                                    }

                                    Repeater {
                                        model : controller? controller.conditionsTypesList : 0

                                        CheckBox {
                                            id : conditionsTypeCB
                                            anchors {
                                                verticalCenter: parent.verticalCenter;
                                            }

                                            checked : myCondition && myCondition.conditionType === model.value;
                                            exclusiveGroup: cdtTypesExclusifGroup
                                            activeFocusOnPress: true;

                                            style: CheckBoxStyle {
                                                label: Text {
                                                    anchors {
                                                        verticalCenter: parent.verticalCenter
                                                        verticalCenterOffset: 2
                                                    }
                                                    color: control.checked? MasticTheme.whiteColor : MasticTheme.lightGreyColor

                                                    text: model.name
                                                    elide: Text.ElideRight

                                                    font {
                                                        family: MasticTheme.textFontFamily
                                                        pixelSize: 15
                                                    }
                                                }

                                                indicator: Rectangle {
                                                    implicitWidth: 14
                                                    implicitHeight: 14
                                                    radius : height / 2
                                                    border.width: 0;
                                                    color : MasticTheme.darkBlueGreyColor

                                                    Rectangle {
                                                        anchors.centerIn: parent
                                                        visible : control.checked
                                                        width: 8
                                                        height: 8
                                                        radius : height / 2

                                                        border.width: 0;
                                                        color : MasticTheme.whiteColor
                                                    }
                                                }

                                            }

                                            onCheckedChanged : {
                                                if (myCondition && checked) {
                                                    myCondition.conditionType = model.value
                                                }
                                            }


                                            Binding {
                                                target : conditionsTypeCB
                                                property : "checked"
                                                value : (myCondition && myCondition.conditionType === model.value)
                                            }
                                        }
                                    }

                                }

                                //
                                // Conditions Details
                                //
                                Row {
                                    anchors {
                                        right : parent.right
                                        rightMargin: 10
                                        left : rowConditionsTypes.left
                                        bottom : parent.bottom
                                        bottomMargin: 6
                                    }
                                    height : agentCombo.height
                                    spacing : 6

                                    // Agent
                                    MasticComboBox {
                                        id : agentCombo

                                        anchors {
                                            verticalCenter : parent.verticalCenter
                                        }

                                        height : 25
                                        width : 148

                                        model : controller ? controller.agentsInMappingList : 0
                                        function modelToString(model)
                                        {
                                            return model.name;
                                        }


                                        Binding {
                                            target : agentCombo
                                            property : "selectedItem"
                                            value : if (myCondition && myCondition.modelM)
                                                    {
                                                        myCondition.modelM.agent;
                                                    }
                                                    else {
                                                        null;
                                                    }
                                        }


                                        onSelectedItemChanged:
                                        {
                                            if (myCondition && myCondition.modelM && agentCombo.selectedItem)
                                            {
                                                myCondition.modelM.agent = agentCombo.selectedItem;
                                            }
                                        }

                                    }

                                    // Agent Inputs/Outputs
                                    MasticComboBoxAgentsIOP {
                                        id : ioCombo

                                        visible : myCondition && myCondition.conditionType === ActionConditionType.VALUE
                                        enabled : visible
                                        anchors {
                                            verticalCenter : parent.verticalCenter
                                        }

                                        height : 25
                                        width : 148

                                        model: (myCondition && myCondition.modelM && myCondition.modelM.agentIopList) ? myCondition.modelM.agentIopList : 0
                                        inputsNumber: (myCondition && myCondition.modelM && myCondition.modelM.agent)? myCondition.modelM.agent.inputsList.count : 0;

                                        Binding {
                                            target : ioCombo
                                            property : "selectedItem"
                                            value : if (myCondition && myCondition.modelM)
                                                    {
                                                        myCondition.modelM.agentIOP;
                                                    }
                                                    else {
                                                        null;
                                                    }
                                        }


                                        onSelectedItemChanged:
                                        {
                                            if (myCondition && myCondition.modelM)
                                            {
                                                myCondition.modelM.agentIOP = ioCombo.selectedItem;
                                            }
                                        }

                                    }

                                    // Comparison Type
                                    MasticComboBox {
                                        id : comparisonCombo
                                        enabled : visible
                                        anchors {
                                            verticalCenter : parent.verticalCenter
                                        }

                                        height : 25
                                        width : (myCondition && myCondition.conditionType === ActionConditionType.VALUE) ? 44 : 78

                                        model :
                                        {
                                            if(controller)
                                            {
                                                (myCondition && myCondition.conditionType === ActionConditionType.VALUE) ? controller.comparisonsValuesTypesList : controller.comparisonsAgentsTypesList
                                            }
                                            else {
                                                0
                                            }

                                        }

                                        function modelToString(model)
                                        {
                                            return model.name;
                                        }


                                        Binding {
                                            target : comparisonCombo
                                            property : "selectedItem"
                                            value : if (myCondition && myCondition.modelM && controller)
                                                    {
                                                        (myCondition && myCondition.conditionType === ActionConditionType.VALUE) ?
                                                                    controller.comparisonsValuesTypesList.getItemWithValue(myCondition.modelM.comparison)
                                                                  :  controller.comparisonsAgentsTypesList.getItemWithValue(myCondition.modelM.comparison);
                                                    }
                                                    else {
                                                        null;
                                                    }
                                        }


                                        onSelectedItemChanged:
                                        {
                                            if (myCondition && myCondition.modelM && comparisonCombo.selectedItem)
                                            {
                                                myCondition.modelM.comparison = comparisonCombo.selectedItem.value;
                                            }
                                        }

                                    }

                                    // Comparison Value
                                    TextField {
                                        id: textFieldComparisonValue

                                        anchors {
                                            verticalCenter : parent.verticalCenter
                                        }

                                        visible : myCondition && myCondition.conditionType === ActionConditionType.VALUE

                                        enabled : visible
                                        height: 25
                                        width: 49

                                        horizontalAlignment: TextInput.AlignLeft
                                        verticalAlignment: TextInput.AlignVCenter

                                        text : myCondition && myCondition.modelM ? myCondition.modelM.value : ""

                                        style: I2TextFieldStyle {
                                            backgroundColor: MasticTheme.darkBlueGreyColor
                                            borderColor: MasticTheme.whiteColor;
                                            borderErrorColor: MasticTheme.redColor
                                            radiusTextBox: 1
                                            borderWidth: 0;
                                            borderWidthActive: 1
                                            textIdleColor: MasticTheme.whiteColor;
                                            textDisabledColor: MasticTheme.darkGreyColor;

                                            padding.left: 3
                                            padding.right: 3

                                            font {
                                                pixelSize:15
                                                family: MasticTheme.textFontFamily
                                            }

                                        }

                                        onActiveFocusChanged: {
                                            if (!activeFocus) {
                                                // Move cursor to our first character when we lose focus
                                                // (to always display the beginning or our text instead of
                                                // an arbitrary part if our text is too long)
                                                cursorPosition = 0;
                                            } else {
                                                textFieldComparisonValue.selectAll();
                                            }
                                        }

                                        onTextChanged: {
                                            if (activeFocus && (myCondition && myCondition.modelM)) {
                                                myCondition.modelM.value = text;
                                            }
                                        }

                                        Binding {
                                            target : textFieldComparisonValue
                                            property :  "text"
                                            value : if  (myCondition && myCondition.modelM) {
                                                        myCondition.modelM.value
                                                    }
                                                    else {
                                                        "";
                                                    }
                                        }
                                    }

                                }


                                // Delete Condition
                                Button {
                                    id: btnDeleteCondition

                                    height : 10
                                    width : 10
                                    anchors {
                                        top: parent.top
                                        right : parent.right
                                        margins: 5
                                    }

                                    activeFocusOnPress: true
                                    style: Theme.LabellessSvgButtonStyle {
                                        fileCache: MasticTheme.svgFileMASTIC

                                        pressedID: releasedID + "-pressed"
                                        releasedID: "closeEditor"
                                        disabledID : releasedID
                                    }

                                    onClicked: {
                                        if (panelController && myCondition)
                                        {
                                            panelController.removeCondition(myCondition);
                                        }
                                    }
                                }
                            }
                        }

                        // add conditions
                        Button {
                            id: addCondition

                            activeFocusOnPress: true

                            anchors {
                                left: parent.left
                            }

                            style: Theme.LabellessSvgButtonStyle {
                                fileCache: MasticTheme.svgFileMASTIC

                                pressedID: releasedID + "-pressed"
                                releasedID: "createButton"
                                disabledID : releasedID
                            }

                            onClicked: {
                                if (panelController)
                                {
                                    panelController.createNewCondition();
                                }
                            }
                        }

                    }
                }

            }


            //
            // Advanced modes
            //
            Item {
                id : advModesItem
                anchors {
                    left : parent.left
                    right : parent.right
                    top : conditionsItem.bottom
                    topMargin: 10
                }

                clip : true
                height : isOpened?
                             (titleadvModeMouseArea.height + revertActionitem.height + revertActionitem.anchors.topMargin + rearmActionitem.height + rearmActionitem.anchors.topMargin + 5)
                           : 35

                Behavior on height {
                    NumberAnimation {}
                }

                property bool isOpened: false

                Connections {
                    target : rootItem
                    Component.onCompleted : {
                        // make the advanced modes visible if there are some modes checked
                        if (actionM && (actionM.shallRevert || actionM.shallRearm)) {
                            advModesItem.isOpened = true;
                        }
                    }
                }

                //Title
                MouseArea {
                    id : titleadvModeMouseArea
                    anchors {
                        left : parent.left
                        right : parent.right
                    }
                    height : 29

                    onClicked: {
                        advModesItem.isOpened = !advModesItem.isOpened;
                    }

                    I2SvgItem {
                        id : arrowadvModes
                        anchors {
                            left : parent.left
                            verticalCenter: titleAdvMode.verticalCenter
                        }

                        svgFileCache : MasticTheme.svgFileMASTIC;
                        svgElementId: "arrowWhite"

                        rotation: advModesItem.isOpened? 0 : 270
                    }

                    Text {
                        id : titleAdvMode
                        anchors {
                            left : arrowadvModes.right
                            leftMargin: 6
                            right : parent.right
                            verticalCenter: parent.verticalCenter
                        }

                        text : "Advanced options"

                        color: titleadvModeMouseArea.containsPress? MasticTheme.lightGreyColor : MasticTheme.whiteColor
                        font {
                            family: MasticTheme.textFontFamily
                            pixelSize: 19
                        }
                    }
                }

                // separator
                Rectangle {
                    id : separatorAdvMode
                    anchors {
                        left : parent.left
                        right : parent.right
                        top : titleadvModeMouseArea.bottom
                    }
                    height : 1
                    color : MasticTheme.whiteColor
                }

                // Revert Action
                Item {
                    id : revertActionitem
                    anchors {
                        left: parent.left;
                        right: parent.right;
                        top : separatorAdvMode.bottom
                        topMargin: 8
                    }
                    height : revertActionCB.checked ? revertActionTime.height : revertActionCB.height
                    visible : advModesItem.isOpened
                    enabled : visible

                    Behavior on height {
                        NumberAnimation {}
                    }

                    CheckBox {
                        id : revertActionCB
                        anchors {
                            left: parent.left;
                            top : parent.top
                        }

                        checked : actionM && actionM.shallRevert;
                        activeFocusOnPress: true;

                        style: CheckBoxStyle {
                            label:  Text {
                                anchors {
                                    verticalCenter: parent.verticalCenter
                                    verticalCenterOffset: 2
                                }

                                color: control.checked? MasticTheme.whiteColor : MasticTheme.lightGreyColor

                                text: "Revert action"
                                elide: Text.ElideRight

                                font {
                                    family: MasticTheme.textFontFamily
                                    pixelSize: 16
                                }

                            }

                            indicator: Rectangle {
                                implicitWidth: 14
                                implicitHeight: 14
                                border.width: 0;
                                color : MasticTheme.darkBlueGreyColor

                                I2SvgItem {
                                    visible : control.checked
                                    anchors.centerIn: parent

                                    svgFileCache : MasticTheme.svgFileMASTIC;
                                    svgElementId:  "check";

                                }
                            }

                        }

                        onCheckedChanged : {
                            if (actionM) {
                                actionM.shallRevert = checked
                            }
                        }


                        Binding {
                            target : revertActionCB
                            property : "checked"
                            value : (actionM && actionM.shallRevert)
                        }
                    }

                    Column {
                        id : revertActionTime

                        anchors {
                            left : revertActionCB.right
                            leftMargin: 14
                            right : parent.right
                            top: revertActionCB.top
                        }

                        height : childrenRect.height
                        spacing: 6

                        enabled: revertActionCB.checked
                        visible : enabled

                        ExclusiveGroup {
                            id : revertActionOpt
                        }

                        CheckBox {
                            id : revertActionTimeCB
                            anchors {
                                left: parent.left;
                            }
                            checked : actionM && actionM.shallRevertWhenValidityIsOver;
                            exclusiveGroup: revertActionOpt
                            activeFocusOnPress: true;

                            style: CheckBoxStyle {
                                label: Text {
                                    anchors {
                                        verticalCenter: parent.verticalCenter
                                        verticalCenterOffset: 2
                                    }
                                    color: control.enabled ? (control.checked? MasticTheme.whiteColor : MasticTheme.lightGreyColor) : "#3C3C3B"

                                    text: "when conditions check terminates"
                                    elide: Text.ElideRight

                                    font {
                                        family: MasticTheme.textFontFamily
                                        pixelSize: 16
                                    }
                                }

                                indicator: Rectangle {
                                    implicitWidth: 14
                                    implicitHeight: 14
                                    radius : height / 2
                                    border.width: 0;
                                    color : control.enabled ?  MasticTheme.darkBlueGreyColor : "#3C3C3B"

                                    Rectangle {
                                        anchors.centerIn: parent
                                        visible : control.checked
                                        width: 8
                                        height: 8
                                        radius : height / 2

                                        border.width: 0;
                                        color : MasticTheme.whiteColor
                                    }
                                }

                            }

                            onCheckedChanged : {
                                if (actionM) {
                                    actionM.shallRevertWhenValidityIsOver = checked
                                }
                            }


                            Binding {
                                target : revertActionTimeCB
                                property : "checked"
                                value : (actionM && actionM.shallRevertWhenValidityIsOver)
                            }
                        }

                        Row {
                            anchors {
                                left: parent.left;
                                right :parent.right
                            }
                            height : 25

                            CheckBox {
                                id : revertActionAfterCB
                                anchors {
                                    verticalCenter: parent.verticalCenter
                                }

                                checked : actionM && actionM.shallRevertAfterTime;
                                exclusiveGroup: revertActionOpt
                                activeFocusOnPress: true;

                                style: CheckBoxStyle {
                                    label: Text {
                                        anchors {
                                            verticalCenter: parent.verticalCenter
                                            verticalCenterOffset: 2
                                        }
                                        color: control.enabled ? (control.checked? MasticTheme.whiteColor : MasticTheme.lightGreyColor) : "#3C3C3B"

                                        text: "after " // space allowing to keep selection possible for the whole row

                                        elide: Text.ElideRight

                                        font {
                                            family: MasticTheme.textFontFamily
                                            pixelSize: 16
                                        }
                                    }

                                    indicator: Rectangle {
                                        implicitWidth: 14
                                        implicitHeight: 14
                                        radius : height / 2
                                        border.width: 0;
                                        color : control.enabled ?  MasticTheme.darkBlueGreyColor : "#3C3C3B"

                                        Rectangle {
                                            anchors.centerIn: parent
                                            visible : control.checked
                                            width: 8
                                            height: 8
                                            radius : height / 2

                                            border.width: 0;
                                            color : MasticTheme.whiteColor
                                        }
                                    }

                                }

                                onCheckedChanged : {
                                    if (actionM) {
                                        actionM.shallRevertAfterTime = checked
                                    }
                                }

                                Binding {
                                    target : revertActionAfterCB
                                    property : "checked"
                                    value : (actionM && actionM.shallRevertAfterTime)
                                }
                            }


                            TextField {
                                id: textFieldDuration

                                anchors {
                                    verticalCenter: parent.verticalCenter
                                    verticalCenterOffset: 2
                                }

                                height: 25
                                width: 57
                                enabled: revertActionAfterCB.enabled
                                horizontalAlignment: TextInput.AlignLeft
                                verticalAlignment: TextInput.AlignVCenter

                                text : actionM ? actionM.revertAfterTimeString : "0.0"
                                inputMethodHints: Qt.ImhFormattedNumbersOnly
                                validator: RegExpValidator { regExp: /(\d{1,5})([.]\d{3})?$/ }

                                style: I2TextFieldStyle {
                                    backgroundColor: MasticTheme.darkBlueGreyColor
                                    backgroundDisabledColor: "#3C3C3B"
                                    borderColor: MasticTheme.whiteColor;
                                    borderErrorColor: MasticTheme.redColor
                                    radiusTextBox: 1
                                    borderWidth: 0;
                                    borderWidthActive: 1
                                    textIdleColor: MasticTheme.whiteColor;
                                    textDisabledColor: MasticTheme.darkGreyColor;

                                    padding.left: 3
                                    padding.right: 3

                                    font {
                                        pixelSize:14
                                        family: MasticTheme.textFontFamily
                                    }

                                }

                                onActiveFocusChanged: {
                                    if (!activeFocus) {
                                        // Move cursor to our first character when we lose focus
                                        // (to always display the beginning or our text instead of
                                        // an arbitrary part if our text is too long)
                                        cursorPosition = 0;
                                    } else {
                                        textFieldDuration.selectAll();
                                    }
                                }

                                onTextChanged: {
                                    if (activeFocus &&  actionM ) {
                                        actionM.revertAfterTimeString = text;
                                    }
                                }

                                Binding {
                                    target : textFieldDuration
                                    property :  "text"
                                    value : if (actionM) {
                                                actionM.revertAfterTimeString
                                            }
                                            else {
                                                "";
                                            }
                                }

                                onFocusChanged: {
                                    if (focus) {
                                        revertActionAfterCB.checked = true;
                                    }
                                }
                            }


                            Text {
                                anchors {
                                    verticalCenter: parent.verticalCenter
                                    verticalCenterOffset: 2
                                }
                                color: revertActionAfterCB.enabled ? (revertActionAfterCB.checked? MasticTheme.whiteColor : MasticTheme.lightGreyColor) : "#3C3C3B"

                                text: " seconds"
                                elide: Text.ElideRight

                                font {
                                    family: MasticTheme.textFontFamily
                                    pixelSize: 16
                                }

                                MouseArea {
                                    anchors.fill: parent

                                    onPressed: {
                                        revertActionAfterCB.checked = true;
                                    }
                                }
                            }

                        }

                    }
                }

                // Rearm Action
                Item {
                    id : rearmActionitem
                    anchors {
                        left: parent.left;
                        right: parent.right;
                        top : revertActionitem.bottom
                        topMargin: 17
                    }
                    height : 50
                    visible : advModesItem.isOpened
                    enabled : visible

                    Column {
                        anchors {
                            left: parent.left;
                            right: parent.right;
                            top : parent.top
                        }


                        CheckBox {
                            id : rearmActionCB
                            anchors {
                                left: parent.left;
                            }

                            checked : actionM && actionM.shallRearm;
                            activeFocusOnPress: true;
                            height: 25

                            style: CheckBoxStyle {
                                label: Text {
                                    anchors {
                                        verticalCenter: parent.verticalCenter
                                        verticalCenterOffset: 2
                                    }
                                    color: control.checked? MasticTheme.whiteColor : MasticTheme.lightGreyColor

                                    text: "Allow multiple triggers as long as conditions are verified"
                                    elide: Text.ElideRight

                                    font {
                                        family: MasticTheme.textFontFamily
                                        pixelSize: 16
                                    }
                                }

                                indicator: Rectangle {
                                    implicitWidth: 14
                                    implicitHeight: 14
                                    border.width: 0;
                                    color : MasticTheme.darkBlueGreyColor

                                    I2SvgItem {
                                        visible : control.checked
                                        anchors.centerIn: parent

                                        svgFileCache : MasticTheme.svgFileMASTIC;
                                        svgElementId:  "check";

                                    }
                                }

                            }

                            onCheckedChanged : {
                                if (actionM) {
                                    actionM.shallRearm = checked
                                }
                            }


                            Binding {
                                target : rearmActionCB
                                property : "checked"
                                value : (actionM && actionM.shallRearm)
                            }
                        }

                        Row {

                            anchors {
                                left: parent.left;
                                leftMargin: 18
                            }

                            height: 25

                            Text {
                                anchors {
                                    verticalCenter: parent.verticalCenter
                                    verticalCenterOffset: 2

                                }

                                color: rearmActionCB.enabled ? (rearmActionCB.checked? MasticTheme.whiteColor : MasticTheme.lightGreyColor) : "#3C3C3B"

                                text: "with trigger every "
                                elide: Text.ElideRight

                                font {
                                    family: MasticTheme.textFontFamily
                                    pixelSize: 16
                                }

                                MouseArea {
                                    anchors.fill: parent

                                    onPressed: {
                                        rearmActionCB.checked = true;
                                    }
                                }
                            }

                            TextField {
                                id: textFieldTimeBeforeRearm

                                anchors {
                                    verticalCenter: parent.verticalCenter
                                    verticalCenterOffset: 2
                                }

                                height: 25
                                width: 57
                                enabled: rearmActionCB.enabled
                                horizontalAlignment: TextInput.AlignLeft
                                verticalAlignment: TextInput.AlignVCenter

                                text : actionM ? actionM.rearmAfterTimeString : "0.0"
                                inputMethodHints: Qt.ImhFormattedNumbersOnly
                                validator: RegExpValidator { regExp: /(\d{1,5})([.]\d{3})?$/ }

                                style: I2TextFieldStyle {
                                    backgroundColor: MasticTheme.darkBlueGreyColor
                                    backgroundDisabledColor: "#3C3C3B"
                                    borderColor: MasticTheme.whiteColor;
                                    borderErrorColor: MasticTheme.redColor
                                    radiusTextBox: 1
                                    borderWidth: 0;
                                    borderWidthActive: 1
                                    textIdleColor: MasticTheme.whiteColor;
                                    textDisabledColor: MasticTheme.darkGreyColor;

                                    padding.left: 3
                                    padding.right: 3

                                    font {
                                        pixelSize:14
                                        family: MasticTheme.textFontFamily
                                    }

                                }

                                onActiveFocusChanged: {
                                    if (!activeFocus) {
                                        // Move cursor to our first character when we lose focus
                                        // (to always display the beginning or our text instead of
                                        // an arbitrary part if our text is too long)
                                        cursorPosition = 0;
                                    } else {
                                        textFieldTimeBeforeRearm.selectAll();
                                    }
                                }

                                onTextChanged: {
                                    if (activeFocus &&  actionM ) {
                                        actionM.rearmAfterTimeString = text;
                                    }
                                }

                                Binding {
                                    target : textFieldTimeBeforeRearm
                                    property :  "text"
                                    value : if (actionM) {
                                                actionM.rearmAfterTimeString
                                            }
                                            else {
                                                "";
                                            }
                                }

                                onFocusChanged: {
                                    if (focus) {
                                        rearmActionCB.checked = true;
                                    }
                                }
                            }


                            Text {
                                anchors {
                                    verticalCenter: parent.verticalCenter
                                    verticalCenterOffset: 2
                                }

                                color: rearmActionCB.enabled ? (rearmActionCB.checked? MasticTheme.whiteColor : MasticTheme.lightGreyColor) : "#3C3C3B"

                                text: " seconds"
                                elide: Text.ElideRight

                                font {
                                    family: MasticTheme.textFontFamily
                                    pixelSize: 16
                                }

                                MouseArea {
                                    anchors.fill: parent

                                    onPressed: {
                                        rearmActionCB.checked = true;
                                    }
                                }
                            }

                        }



                    }

                }
            }


            //
            // Effects
            //
            Item {
                id : effectsListItem
                anchors {
                    left : parent.left
                    right : parent.right
                    top : advModesItem.bottom
                    topMargin: 20
                    bottom : cancelButton.top
                    bottomMargin: 10
                }

                Behavior on anchors.top {
                    NumberAnimation {}
                }

                //Title
                Text {
                    id : titleEffects
                    anchors {
                        left : parent.left
                        right : parent.right
                    }

                    text : "Effects"

                    color: MasticTheme.whiteColor
                    font {
                        family: MasticTheme.textFontFamily
                        pixelSize: 19
                    }
                }

                // separator
                Rectangle {
                    id : separatorEffect
                    anchors {
                        left : parent.left
                        right : parent.right
                        top : titleEffects.bottom
                        topMargin: 5
                    }
                    height : 1
                    color : MasticTheme.whiteColor
                }

                /// Effects List
                ScrollView {
                    id : scrollView

                    anchors {
                        top : separatorEffect.bottom
                        topMargin: 6
                        right : parent.right
                        left : parent.left
                        bottom : parent.bottom
                    }

                    style: MasticScrollViewStyle {
                    }

                    // Prevent drag overshoot on Windows
                    flickableItem.boundsBehavior: Flickable.OvershootBounds

                    /// Effects List
                    contentItem: Column {
                        id : effectsList
                        spacing : 6
                        height : childrenRect.height
                        width : effectsListItem.width - 9 // scrollbar size

                        Repeater {
                            model : actionM ? actionM.effectsList : 0

                            Rectangle {
                                height :  (myEffect && myEffect.effectType === ActionEffectTypes.MAPPING) ? 90 : 62
                                anchors {
                                    left : parent.left
                                    right :parent.right
                                }

                                color : "transparent"
                                radius: 5
                                border {
                                    width : 1
                                    color : MasticTheme.blackColor
                                }

                                // my effect
                                property var myEffect: model.QtObject

                                // Effect Type
                                Row {
                                    id : rowEffectsTypes
                                    anchors {
                                        right : parent.right
                                        left : parent.left
                                        leftMargin: 10
                                        top : parent.top
                                        topMargin: 6
                                    }
                                    height : 14
                                    spacing : 15

                                    ExclusiveGroup {
                                        id : effectTypesExclusifGroup
                                    }

                                    Repeater {
                                        model : controller? controller.effectsTypesList : 0

                                        CheckBox {
                                            id : effectTypeCB
                                            anchors {
                                                verticalCenter: parent.verticalCenter;
                                            }

                                            checked : myEffect && myEffect.effectType === model.value;
                                            exclusiveGroup: effectTypesExclusifGroup
                                            activeFocusOnPress: true;

                                            style: CheckBoxStyle {
                                                label: Text {
                                                    anchors {
                                                        verticalCenter: parent.verticalCenter
                                                        verticalCenterOffset: 2
                                                    }
                                                    color: control.checked? MasticTheme.whiteColor : MasticTheme.lightGreyColor

                                                    text: model.name
                                                    elide: Text.ElideRight

                                                    font {
                                                        family: MasticTheme.textFontFamily
                                                        pixelSize: 15
                                                    }
                                                }

                                                indicator: Rectangle {
                                                    implicitWidth: 14
                                                    implicitHeight: 14
                                                    radius : height / 2
                                                    border.width: 0;
                                                    color : MasticTheme.darkBlueGreyColor

                                                    Rectangle {
                                                        anchors.centerIn: parent
                                                        visible : control.checked
                                                        width: 8
                                                        height: 8
                                                        radius : height / 2

                                                        border.width: 0;
                                                        color : MasticTheme.whiteColor
                                                    }
                                                }

                                            }

                                            onCheckedChanged : {
                                                if (myEffect && checked) {
                                                    myEffect.effectType = model.value
                                                }
                                            }


                                            Binding {
                                                target : effectTypeCB
                                                property : "checked"
                                                value : (myEffect && myEffect.effectType === model.value)
                                            }
                                        }
                                    }
                                }

                                //
                                // Effect Details for Agent and Value
                                //
                                Row {
                                    anchors {
                                        right : parent.right
                                        rightMargin: 10
                                        left : rowEffectsTypes.left
                                        bottom : parent.bottom
                                        bottomMargin: 6
                                    }
                                    height : agentEffectCombo.height
                                    spacing : 6

                                    visible : myEffect && myEffect.effectType !== ActionEffectTypes.MAPPING

                                    // Agent
                                    MasticComboBox {
                                        id : agentEffectCombo

                                        anchors {
                                            verticalCenter : parent.verticalCenter
                                        }

                                        height : 25
                                        width : 148

                                        model : controller ? controller.agentsInMappingList : 0
                                        function modelToString(model)
                                        {
                                            return model.name;
                                        }


                                        Binding {
                                            target : agentEffectCombo
                                            property : "selectedItem"
                                            value : if (myEffect && myEffect.modelM)
                                                    {
                                                        myEffect.modelM.agent;
                                                    }
                                                    else {
                                                        null;
                                                    }
                                        }


                                        onSelectedItemChanged:
                                        {
                                            if (myEffect && myEffect.modelM)
                                            {
                                                myEffect.modelM.agent = agentEffectCombo.selectedItem;
                                            }
                                        }

                                    }

                                    // Agent Inputs/Outputs
                                    MasticComboBoxAgentsIOP {
                                        id : ioEffectsCombo

                                        visible : myEffect && myEffect.effectType === ActionEffectTypes.VALUE
                                        enabled : visible
                                        anchors {
                                            verticalCenter : parent.verticalCenter
                                        }

                                        height : 25
                                        width : 148

                                        model: (myEffect && myEffect.modelM && myEffect.modelM.iopMergedList) ? myEffect.modelM.iopMergedList : 0
                                        inputsNumber: (myEffect && myEffect.modelM && myEffect.modelM.agent) ? myEffect.modelM.agent.inputsList.count : 0;

                                        function modelToString(model)
                                        {
                                            return model.name;
                                        }


                                        Binding {
                                            target : ioEffectsCombo
                                            property : "selectedItem"
                                            value : if (myEffect && myEffect.modelM)
                                                    {
                                                        myEffect.modelM.agentIOP;
                                                    }
                                                    else {
                                                        null;
                                                    }
                                        }


                                        onSelectedItemChanged:
                                        {
                                            if (myEffect && myEffect.modelM)
                                            {
                                                myEffect.modelM.agentIOP = ioEffectsCombo.selectedItem;
                                            }
                                        }

                                    }

                                    // Comparison Type
                                    MasticComboBox {
                                        id : effectTypeCombo

                                        anchors {
                                            verticalCenter : parent.verticalCenter
                                        }

                                        height : 25
                                        width : 78

                                        visible : (myEffect && myEffect.effectType === ActionEffectTypes.AGENT)
                                        enabled : visible

                                        model :
                                        {
                                            if(controller) {
                                                controller.agentEffectValuesList
                                            }
                                            else {
                                                0
                                            }

                                        }

                                        function modelToString(model)
                                        {
                                            return model.name;
                                        }


                                        Binding {
                                            target : effectTypeCombo
                                            property : "selectedItem"
                                            value : if (myEffect && myEffect.modelM && controller)
                                                    {
                                                        controller.agentEffectValuesList.getItemWithValue(myEffect.modelM.agentEffectValue);
                                                    }
                                                    else {
                                                        null;
                                                    }
                                        }


                                        onSelectedItemChanged:
                                        {
                                            if (myEffect && myEffect.modelM && effectTypeCombo.selectedItem)
                                            {
                                                myEffect.modelM.agentEffectValue = effectTypeCombo.selectedItem.value;
                                            }
                                        }

                                    }

                                    // Target Value
                                    TextField {
                                        id: textFieldTargetValue

                                        anchors {
                                            verticalCenter : parent.verticalCenter
                                        }

                                        visible : myEffect && myEffect.effectType === ActionEffectTypes.VALUE
                                        enabled : visible

                                        height: 25
                                        width: 49

                                        horizontalAlignment: TextInput.AlignLeft
                                        verticalAlignment: TextInput.AlignVCenter

                                        text : myEffect && myEffect.modelM  ? myEffect.modelM.value : ""

                                        style: I2TextFieldStyle {
                                            backgroundColor: MasticTheme.darkBlueGreyColor
                                            borderColor: MasticTheme.whiteColor;
                                            borderErrorColor: MasticTheme.redColor
                                            radiusTextBox: 1
                                            borderWidth: 0;
                                            borderWidthActive: 1
                                            textIdleColor: MasticTheme.whiteColor;
                                            textDisabledColor: MasticTheme.darkGreyColor;

                                            padding.left: 3
                                            padding.right: 3

                                            font {
                                                pixelSize:15
                                                family: MasticTheme.textFontFamily
                                            }

                                        }

                                        onActiveFocusChanged: {
                                            if (!activeFocus) {
                                                // Move cursor to our first character when we lose focus
                                                // (to always display the beginning or our text instead of
                                                // an arbitrary part if our text is too long)
                                                cursorPosition = 0;
                                            } else {
                                                textFieldTargetValue.selectAll();
                                            }
                                        }

                                        onTextChanged: {
                                            if (activeFocus && (myEffect && myEffect.modelM)) {
                                                myEffect.modelM.value = text;
                                            }
                                        }

                                        Binding {
                                            target : textFieldTargetValue
                                            property :  "text"
                                            value : if  (myEffect && myEffect.modelM) {
                                                        myEffect.modelM.value
                                                    }
                                                    else {
                                                        "";
                                                    }
                                        }
                                    }

                                }

                                //
                                // Effect Details for Mapping
                                //
                                Item {
                                    anchors {
                                        right : parent.right
                                        rightMargin: 10
                                        left : rowEffectsTypes.left
                                        bottom : parent.bottom
                                        bottomMargin: 6
                                    }
                                    visible: (myEffect && myEffect.effectType === ActionEffectTypes.MAPPING)

                                    // Output Agent
                                    MasticComboBox {
                                        id : comboEffectOnMapping_OutputAgent

                                        anchors {
                                            left : parent.left
                                            bottom : comboEffectOnMapping_Output.top
                                            bottomMargin: 6
                                        }

                                        height : 25
                                        width : 148

                                        model : controller ? controller.agentsInMappingList : 0

                                        function modelToString(model) {
                                            return model.name;
                                        }

                                        Binding {
                                            target : comboEffectOnMapping_OutputAgent
                                            property : "selectedItem"
                                            value : if (myEffect && myEffect.modelM) {
                                                        myEffect.modelM.outputAgent;
                                                    }
                                                    else {
                                                        null;
                                                    }
                                        }

                                        onSelectedItemChanged: {
                                            if (myEffect && myEffect.modelM)
                                            {
                                                myEffect.modelM.outputAgent = comboEffectOnMapping_OutputAgent.selectedItem;
                                            }
                                        }
                                    }

                                    // Outputs (of output agent)
                                    MasticComboBoxAgentsIOP {
                                        id : comboEffectOnMapping_Output

                                        enabled : visible
                                        anchors {
                                            left : parent.left
                                            bottom : parent.bottom
                                        }

                                        height : 25
                                        width : 148

                                        model : (myEffect && myEffect.modelM) ? myEffect.modelM.outputsList : 0

                                        function modelToString(model) {
                                            return model.name;
                                        }

                                        Binding {
                                            target : comboEffectOnMapping_Output
                                            property : "selectedItem"
                                            value : if (myEffect && myEffect.modelM) {
                                                        myEffect.modelM.output;
                                                    }
                                                    else {
                                                        null;
                                                    }
                                        }

                                        onSelectedItemChanged: {
                                            if (myEffect && myEffect.modelM)
                                            {
                                                myEffect.modelM.output = comboEffectOnMapping_Output.selectedItem;
                                            }
                                        }

                                    }

                                    Item {
                                        id : disableMappingItem
                                        anchors {
                                            left : comboEffectOnMapping_OutputAgent.right
                                            right : comboEffectOnMapping_InputAgent.left
                                            top : comboEffectOnMapping_OutputAgent.top
                                            bottom : parent.bottom
                                        }
                                        clip : true

                                        Rectangle {
                                            anchors {
                                                verticalCenter: parent.verticalCenter
                                                right : rectRight.right
                                                left : rectLeft.left
                                            }
                                            color : MasticTheme.blackColor
                                            height : 1
                                        }

                                        Rectangle {
                                            id : rectLeft
                                            anchors {
                                                horizontalCenter: parent.left
                                                top : parent.top
                                                bottom : parent.bottom
                                                topMargin: comboEffectOnMapping_OutputAgent.height/2
                                                bottomMargin: comboEffectOnMapping_OutputAgent.height/2
                                            }
                                            width : 12
                                            color : MasticTheme.veryDarkGreyColor
                                            border {
                                                width: 1
                                                color : MasticTheme.blackColor
                                            }
                                        }

                                        Rectangle {
                                            id : rectRight
                                            anchors {
                                                horizontalCenter: parent.right
                                                top : parent.top
                                                bottom : parent.bottom
                                                topMargin: comboEffectOnMapping_OutputAgent.height/2
                                                bottomMargin: comboEffectOnMapping_OutputAgent.height/2
                                            }
                                            width : 12
                                            color : MasticTheme.veryDarkGreyColor
                                            border {
                                                width: 1
                                                color : MasticTheme.blackColor
                                            }
                                        }

                                        Button {
                                            id : enabledbutton
                                            anchors.centerIn: parent

                                            style: I2SvgToggleButtonStyle {
                                                fileCache: MasticTheme.svgFileMASTIC

                                                toggleCheckedReleasedID :  "enabledToggle-checked";
                                                toggleCheckedPressedID :  "enabledToggle-checked-pressed";
                                                toggleUncheckedReleasedID : "enabledToggle";
                                                toggleUncheckedPressedID : "enabledToggle-pressed";

                                                // No disabled states
                                                toggleCheckedDisabledID: ""
                                                toggleUncheckedDisabledID: ""

                                                labelMargin: 0;
                                            }

                                            onCheckedChanged: {
                                                if (myEffect && myEffect.modelM)
                                                {
                                                    if (checked) {
                                                        myEffect.modelM.mappingEffectValue = MappingEffectValues.MAPPED;
                                                    }
                                                    else {
                                                        myEffect.modelM.mappingEffectValue = MappingEffectValues.UNMAPPED;
                                                    }
                                                }
                                            }

                                            Binding {
                                                target : enabledbutton
                                                property : "checked"
                                                value : (myEffect && myEffect.modelM && myEffect.modelM.mappingEffectValue === MappingEffectValues.MAPPED) ? true : false;
                                            }
                                        }


                                        Text {
                                            anchors {
                                                horizontalCenter : enabledbutton.horizontalCenter
                                                top : enabledbutton.bottom
                                                topMargin: 3
                                            }

                                            text : (myEffect && myEffect.modelM && myEffect.modelM.mappingEffectValue === MappingEffectValues.MAPPED) ? "Mapped" : "Not mapped"
                                            color : MasticTheme.whiteColor
                                            font {
                                                family: MasticTheme.textFontFamily
                                                pixelSize: 12
                                            }
                                        }
                                    }

                                    // Input Agent
                                    MasticComboBox {
                                        id : comboEffectOnMapping_InputAgent

                                        anchors {
                                            right : parent.right
                                            bottom : comboEffectOnMapping_Input.top
                                            bottomMargin: 6
                                        }

                                        height : 25
                                        width : 148

                                        model : controller ? controller.agentsInMappingList : 0

                                        function modelToString(model) {
                                            return model.name;
                                        }


                                        Binding {
                                            target : comboEffectOnMapping_InputAgent
                                            property : "selectedItem"
                                            value : if (myEffect && myEffect.modelM) {
                                                        myEffect.modelM.agent;
                                                    }
                                                    else {
                                                        null;
                                                    }
                                        }

                                        onSelectedItemChanged: {
                                            if (myEffect && myEffect.modelM)
                                            {
                                                myEffect.modelM.agent = comboEffectOnMapping_InputAgent.selectedItem;
                                            }
                                        }
                                    }

                                    // Intputs (of input agent)
                                    MasticComboBoxAgentsIOP {
                                        id : comboEffectOnMapping_Input

                                        enabled : visible
                                        anchors {
                                            right : parent.right
                                            bottom : parent.bottom
                                        }

                                        height : 25
                                        width : 148

                                        model : (myEffect && myEffect.modelM) ? myEffect.modelM.inputsList : 0

                                        function modelToString(model) {
                                            return model.name;
                                        }

                                        Binding {
                                            target : comboEffectOnMapping_Input
                                            property : "selectedItem"
                                            value : if (myEffect && myEffect.modelM) {
                                                        myEffect.modelM.input;
                                                    }
                                                    else {
                                                        null;
                                                    }
                                        }

                                        onSelectedItemChanged: {
                                            if (myEffect && myEffect.modelM)
                                            {
                                                myEffect.modelM.input = comboEffectOnMapping_Input.selectedItem;
                                            }
                                        }
                                    }

                                }


                                // Delete Effect
                                Button {
                                    id: btnDeleteEffect

                                    height : 10
                                    width : 10
                                    anchors {
                                        top: parent.top
                                        right : parent.right
                                        margins: 5
                                    }

                                    activeFocusOnPress: true
                                    style: Theme.LabellessSvgButtonStyle {
                                        fileCache: MasticTheme.svgFileMASTIC

                                        pressedID: releasedID + "-pressed"
                                        releasedID: "closeEditor"
                                        disabledID : releasedID
                                    }

                                    onClicked: {
                                        if (panelController && myEffect)
                                        {
                                            panelController.removeEffect(myEffect);
                                        }
                                    }
                                }


                            }
                        }

                        // add effect
                        Button {
                            id: addEffects

                            activeFocusOnPress: true

                            anchors {
                                left: parent.left
                            }

                            style: Theme.LabellessSvgButtonStyle {
                                fileCache: MasticTheme.svgFileMASTIC

                                pressedID: releasedID + "-pressed"
                                releasedID: "createButton"
                                disabledID : releasedID
                            }

                            onClicked: {
                                if (panelController)
                                {
                                    panelController.createNewEffect();
                                }
                            }
                        }


                    }
                }
            }


            // Delete Action
            MouseArea {
                id : actionDeleteBtn
                enabled: visible
                visible: (panelController && panelController.originalAction !== null)
                anchors {
                    left : parent.left
                    leftMargin: 15
                    verticalCenter: cancelButton.verticalCenter
                    verticalCenterOffset: 2
                }

                height : actionDelete.height
                width : actionDelete.width

                hoverEnabled: true
                onClicked: {
                    if (controller && panelController && panelController.originalAction) {
                        controller.deleteAction(panelController.originalAction);
                    }
                    else {
                        // Close our popup
                        rootItem.close();
                    }
                }

                Text {
                    id: actionDelete

                    anchors {
                        left : parent.left
                    }
                    text : "Delete Action"
                    color: actionDeleteBtn.pressed ? MasticTheme.greyColor : MasticTheme.lightGreyColor
                    elide: Text.ElideRight

                    font {
                        family: MasticTheme.textFontFamily
                        pixelSize: 16
                    }
                }

                // underline
                Rectangle {
                    visible: actionDeleteBtn.containsMouse

                    anchors {
                        left : actionDeleteBtn.left
                        right : actionDelete.right
                        bottom : parent.bottom
                    }

                    height : 1

                    color : actionDelete.color
                }
            }


            Button {
                id: cancelButton
                activeFocusOnPress: true
                property var boundingBox: MasticTheme.svgFileMASTIC.boundsOnElement("button");
                height : boundingBox.height
                width :  boundingBox.width

                enabled : visible
                text : "Cancel"

                anchors {
                    verticalCenter: okButton.verticalCenter
                    right : okButton.left
                    rightMargin: 20
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

            Button {
                id: okButton

                property var boundingBox: MasticTheme.svgFileMASTIC.boundsOnElement("button");
                height : boundingBox.height
                width :  boundingBox.width

                enabled : visible
                activeFocusOnPress: true
                text : "OK"

                anchors {
                    bottom: parent.bottom
                    right : parent.right
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
                    if(controller)
                    {
                        controller.valideActionEditor(model.QtObject);
                    }
                    // Close our popup
                    rootItem.close();
                }
            }
        }

    }



    I2Layer {
        id: overlayLayerComboBox
        objectName: "overlayLayerComboBox"

        anchors.fill: parent
    }
}

