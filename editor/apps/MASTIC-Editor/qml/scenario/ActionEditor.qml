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
 *
 */

import QtQuick 2.8
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4

import I2Quick 1.0

import MASTIC 1.0
import "../theme" as Theme;

I2PopupBase {
    id: rootItem

    width: 475
    height: 882

    automaticallyOpenWhenCompleted: true
    isModal: false
    dismissOnOutsideTap : false;
    keepRelativePositionToInitialParent : false;


    //--------------------------------
    //
    // Properties
    //
    //--------------------------------

    // our model is an action model
    property var actionM: model.editedAction;

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
            width: 1
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

                text : "Action Editor"
                elide: Text.ElideRight
                color: MasticTheme.definitionEditorsLabelColor
                font {
                    family: MasticTheme.textFontFamily
                    pixelSize: 23
                    weight: Font.Medium
                }
            }

            Button {
                id: btnCloseEditor

                anchors {
                    top: parent.top
                    right : parent.right
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
                        borderColor: MasticTheme.lightGreyColor;
                        borderErrorColor: MasticTheme.redColor
                        radiusTextBox: 1
                        borderWidth: 0;
                        borderWidthActive: 1
                        textIdleColor: MasticTheme.lightGreyColor;
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
            // Conditions
            //
            Item {
                id : conditionsItem
                anchors {
                    left : parent.left
                    right : parent.right
                    top : nameItem.bottom
                    topMargin: 14
                    bottom : advModesItem.top
                    bottomMargin: 10
                }

                //Title
                Text {
                    id : titleCdt
                    anchors {
                        left : parent.left
                        right : parent.right
                    }

                    text : "Conditions"

                    color: MasticTheme.whiteColor
                    font {
                        family: MasticTheme.textFontFamily
                        pixelSize: 19
                    }
                }

                // separator
                Rectangle {
                    id : separatorCdt
                    anchors {
                        left : parent.left
                        right : parent.right
                        top : titleCdt.bottom
                        topMargin: 5
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


                    Text {
                        id : textValidity
                        anchors {
                            left : parent.left
                            verticalCenter : parent.verticalCenter
                        }

                        text : "Conditions check duration:"

                        color: MasticTheme.lightGreyColor
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
                                        } else {
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
                            validator: RegExpValidator { regExp: /(\d{1,4})([.]\d{3})?$/ }

                            style: I2TextFieldStyle {
                                backgroundColor: MasticTheme.darkBlueGreyColor
                                borderColor: MasticTheme.lightGreyColor;
                                borderErrorColor: MasticTheme.redColor
                                radiusTextBox: 1
                                borderWidth: 0;
                                borderWidthActive: 1
                                textIdleColor: MasticTheme.lightGreyColor;
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

                            color: MasticTheme.lightGreyColor
                            font {
                                family: MasticTheme.textFontFamily
                                pixelSize: 15
                            }
                        }

                    }
                }



                // add conditions
                Button {
                    id: addCondition

                    activeFocusOnPress: true

                    anchors {
                        top : validityDuration.bottom
                        topMargin: 18
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


                //
                // Conditions List
                //
                ScrollView {
                    id : scrollViewCondition

                    anchors {
                        top : addCondition.bottom
                        topMargin: 8
                        right : parent.right
                        left : parent.left
                        bottom : parent.bottom
                    }

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
                        width : scrollViewCondition.width - 9 // scrollbar size

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
                                                    color: control.checked? MasticTheme.lightGreyColor : MasticTheme.greyColor

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
                                                        color : MasticTheme.lightGreyColor
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
                                            value : if (myCondition && myCondition.condition)
                                                    {
                                                        myCondition.condition.agentModel;
                                                    } else {
                                                        null;
                                                    }
                                        }


                                        onSelectedItemChanged:
                                        {
                                            if (myCondition && myCondition.condition && agentCombo.selectedItem)
                                            {
                                                myCondition.condition.agentModel = agentCombo.selectedItem;
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

                                        model: (myCondition && myCondition.condition && myCondition.condition.agentIopList) ? myCondition.condition.agentIopList : 0
                                        inputsNumber: (myCondition && myCondition.condition && myCondition.condition.agentModel)? myCondition.condition.agentModel.inputsList.count : 0;

                                        Binding {
                                            target : ioCombo
                                            property : "selectedItem"
                                            value : if (myCondition && myCondition.condition)
                                                    {
                                                        myCondition.condition.agentIOP;
                                                    } else {
                                                        null;
                                                    }
                                        }


                                        onSelectedItemChanged:
                                        {
                                            if (myCondition && myCondition.condition)
                                            {
                                                myCondition.condition.agentIOP = ioCombo.selectedItem;
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
                                            } else {
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
                                            value : if (myCondition && myCondition.condition && controller)
                                                    {
                                                        (myCondition && myCondition.conditionType === ActionConditionType.VALUE) ?
                                                                    controller.comparisonsValuesTypesList.getItemWithValue(myCondition.condition.comparison)
                                                                  :  controller.comparisonsAgentsTypesList.getItemWithValue(myCondition.condition.comparison);
                                                    } else {
                                                        null;
                                                    }
                                        }


                                        onSelectedItemChanged:
                                        {
                                            if (myCondition && myCondition.condition && comparisonCombo.selectedItem)
                                            {
                                                myCondition.condition.comparison = comparisonCombo.selectedItem.value;
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

                                        text : actionM  ? actionM.validityDuration : "0.0"
                                        //   inputMethodHints: Qt.ImhFormattedNumbersOnly
                                        //   validator: RegExpValidator { regExp: /(\d{1,4})([.]\d{3})?$/ }

                                        style: I2TextFieldStyle {
                                            backgroundColor: MasticTheme.darkBlueGreyColor
                                            borderColor: MasticTheme.lightGreyColor;
                                            borderErrorColor: MasticTheme.redColor
                                            radiusTextBox: 1
                                            borderWidth: 0;
                                            borderWidthActive: 1
                                            textIdleColor: MasticTheme.lightGreyColor;
                                            textDisabledColor: MasticTheme.darkGreyColor;

                                            padding.left: 3
                                            padding.right: 3

                                            font {
                                                pixelSize:15
                                                family: MasticTheme.textFontFamily
                                            }

                                        }

                                        onTextChanged: {
                                            if (activeFocus && (myCondition && myCondition.condition)) {
                                                myCondition.condition.value = text;
                                            }
                                        }

                                        Binding {
                                            target : textFieldComparisonValue
                                            property :  "text"
                                            value : if  (myCondition && myCondition.condition) {
                                                        myCondition.condition.value
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
                    }
                }

            }


            //
            // Advanced mode
            //
            Item {
                id : advModesItem
                anchors {
                    left : parent.left
                    right : parent.right
                    top : conditionsItem.top
                    topMargin: 320
                }
                height : childrenRect.height

                //Title
                Text {
                    id : titleAdvMode
                    anchors {
                        left : parent.left
                        right : parent.right
                    }

                    text : "Advanced modes"

                    color: MasticTheme.whiteColor
                    font {
                        family: MasticTheme.textFontFamily
                        pixelSize: 19
                    }
                }

                // separator
                Rectangle {
                    id : separatorAdvMode
                    anchors {
                        left : parent.left
                        right : parent.right
                        top : titleAdvMode.bottom
                        topMargin: 5
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
                    height : childrenRect.height

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

                                color: control.checked? MasticTheme.lightGreyColor : MasticTheme.greyColor

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
                        spacing: 6

                        enabled: revertActionCB.checked

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
                                    color: control.enabled ? (control.checked? MasticTheme.lightGreyColor : MasticTheme.greyColor) : "#3C3C3B"

                                    text: "At end of conditions check duration"
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
                                        color : MasticTheme.lightGreyColor
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
                                        color: control.enabled ? (control.checked? MasticTheme.lightGreyColor : MasticTheme.greyColor) : "#3C3C3B"

                                        text: "After " // space allowing to keep selection possible for the whole row

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
                                            color : MasticTheme.lightGreyColor
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
                                validator: RegExpValidator { regExp: /(\d{1,4})([.]\d{3})?$/ }

                                style: I2TextFieldStyle {
                                    backgroundColor: MasticTheme.darkBlueGreyColor
                                    backgroundDisabledColor: "#3C3C3B"
                                    borderColor: MasticTheme.lightGreyColor;
                                    borderErrorColor: MasticTheme.redColor
                                    radiusTextBox: 1
                                    borderWidth: 0;
                                    borderWidthActive: 1
                                    textIdleColor: MasticTheme.lightGreyColor;
                                    textDisabledColor: MasticTheme.darkGreyColor;

                                    padding.left: 3
                                    padding.right: 3

                                    font {
                                        pixelSize:14
                                        family: MasticTheme.textFontFamily
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
                                color: revertActionAfterCB.enabled ? (revertActionAfterCB.checked? MasticTheme.lightGreyColor : MasticTheme.greyColor) : "#3C3C3B"

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
                    height : childrenRect.height


                    CheckBox {
                        id : rearmActionCB
                        anchors {
                            left: parent.left;
                            top : parent.top
                        }

                        checked : actionM && actionM.v;
                        activeFocusOnPress: true;

                        style: CheckBoxStyle {
                            label: Text {
                                anchors {
                                    verticalCenter: parent.verticalCenter
                                    verticalCenterOffset: 2
                                }
                                color: control.checked? MasticTheme.lightGreyColor : MasticTheme.greyColor

                                text: "Rearm action after each execution"
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



                // add effect
                Button {
                    id: addEffects

                    activeFocusOnPress: true

                    anchors {
                        top: separatorEffect.bottom
                        topMargin: 15
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



                /// Effects List
                ScrollView {
                    id : scrollView

                    anchors {
                        top : addEffects.bottom
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
                                height :  (myEffect && myEffect.effectType === ActionEffectType.MAPPING) ? 90 : 62
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
                                                    color: control.checked? MasticTheme.lightGreyColor : MasticTheme.greyColor

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
                                                        color : MasticTheme.lightGreyColor
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

                                    visible : myEffect && myEffect.effectType !== ActionEffectType.MAPPING

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
                                            value : if (myEffect && myEffect.effect)
                                                    {
                                                        myEffect.effect.agentModel;
                                                    } else {
                                                        null;
                                                    }
                                        }


                                        onSelectedItemChanged:
                                        {
                                            if (myEffect && myEffect.effect)
                                            {
                                                myEffect.effect.agentModel = agentEffectCombo.selectedItem;
                                            }
                                        }

                                    }

                                    // Agent Inputs/Outputs
                                    MasticComboBoxAgentsIOP {
                                        id : ioEffectsCombo

                                        visible : myEffect && myEffect.effectType === ActionEffectType.VALUE
                                        enabled : visible
                                        anchors {
                                            verticalCenter : parent.verticalCenter
                                        }

                                        height : 25
                                        width : 148

                                        model : (myEffect && myEffect.effect && myEffect.effect.agentIopList) ? myEffect.effect.agentIopList : 0
                                        inputsNumber: (myEffect && myEffect.effect && myEffect.effect.agentModel)? myEffect.effect.agentModel.inputsList.count : 0;

                                        function modelToString(model)
                                        {
                                            return model.name;
                                        }


                                        Binding {
                                            target : ioEffectsCombo
                                            property : "selectedItem"
                                            value : if (myEffect && myEffect.effect)
                                                    {
                                                        myEffect.effect.agentIOP;
                                                    } else {
                                                        null;
                                                    }
                                        }


                                        onSelectedItemChanged:
                                        {
                                            if (myEffect && myEffect.effect)
                                            {
                                                myEffect.effect.agentIOP = ioEffectsCombo.selectedItem;
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

                                        visible : (myEffect && myEffect.effectType === ActionEffectType.AGENT)
                                        enabled : visible

                                        model :
                                        {
                                            if(controller)
                                            {
                                                controller.effectsAgentsTypesList
                                            } else {
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
                                            value : if (myEffect && myEffect.effect && controller)
                                                    {
                                                        controller.effectsAgentsTypesList.getItemWithValue(myEffect.effect.effect);
                                                    } else {
                                                        null;
                                                    }
                                        }


                                        onSelectedItemChanged:
                                        {
                                            if (myEffect && myEffect.effect && effectTypeCombo.selectedItem)
                                            {
                                                myEffect.effect.effect = effectTypeCombo.selectedItem.value;
                                            }
                                        }

                                    }

                                    // Target Value
                                    TextField {
                                        id: textFieldTargetValue

                                        anchors {
                                            verticalCenter : parent.verticalCenter
                                        }

                                        visible : myEffect && myEffect.effectType === ActionEffectType.VALUE
                                        enabled : visible

                                        height: 25
                                        width: 49

                                        horizontalAlignment: TextInput.AlignLeft
                                        verticalAlignment: TextInput.AlignVCenter

                                        text : actionM  ? actionM.validityDuration : "0.0"
                                        //   inputMethodHints: Qt.ImhFormattedNumbersOnly
                                        //   validator: RegExpValidator { regExp: /(\d{1,4})([.]\d{3})?$/ }

                                        style: I2TextFieldStyle {
                                            backgroundColor: MasticTheme.darkBlueGreyColor
                                            borderColor: MasticTheme.lightGreyColor;
                                            borderErrorColor: MasticTheme.redColor
                                            radiusTextBox: 1
                                            borderWidth: 0;
                                            borderWidthActive: 1
                                            textIdleColor: MasticTheme.lightGreyColor;
                                            textDisabledColor: MasticTheme.darkGreyColor;

                                            padding.left: 3
                                            padding.right: 3

                                            font {
                                                pixelSize:15
                                                family: MasticTheme.textFontFamily
                                            }

                                        }

                                        onTextChanged: {
                                            if (activeFocus && (myEffect && myEffect.effect)) {
                                                myEffect.effect.value = text;
                                            }
                                        }

                                        Binding {
                                            target : textFieldTargetValue
                                            property :  "text"
                                            value : if  (myEffect && myEffect.effect) {
                                                        myEffect.effect.value
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
                                    visible : myEffect && myEffect.effectType === ActionEffectType.MAPPING

                                    // Agent FROM
                                    MasticComboBox {
                                        id : agentFROMEffectMappingCombo

                                        anchors {
                                            left : parent.left
                                            bottom : oEffectsMappingFROMCombo.top
                                            bottomMargin: 6
                                        }

                                        height : 25
                                        width : 148

                                        model : controller ? controller.agentsInMappingList : 0
                                        function modelToString(model)
                                        {
                                            return model.name;
                                        }


                                        Binding {
                                            target : agentFROMEffectMappingCombo
                                            property : "selectedItem"
                                            value : if (myEffect && myEffect.effect)
                                                    {
                                                        myEffect.effect.agentModel;
                                                    } else {
                                                        null;
                                                    }
                                        }


                                        onSelectedItemChanged:
                                        {
                                            if (myEffect && myEffect.effect)
                                            {
                                                myEffect.effect.agentModel = agentFROMEffectMappingCombo.selectedItem;
                                            }
                                        }

                                    }

                                    // Agent FROM Outputs
                                    MasticComboBoxAgentsIOP {
                                        id : oEffectsMappingFROMCombo

                                        enabled : visible
                                        anchors {
                                            left : parent.left
                                            bottom : parent.bottom
                                        }

                                        height : 25
                                        width : 148

                                        model : (myEffect && myEffect.effect && myEffect.effect) ? myEffect.effect.fromAgentIopList : 0
                                        function modelToString(model)
                                        {
                                            return model.name;
                                        }


                                        Binding {
                                            target : oEffectsMappingFROMCombo
                                            property : "selectedItem"
                                            value : if (myEffect && myEffect.effect)
                                                    {
                                                        myEffect.effect.fromAgentIOP;
                                                    } else {
                                                        null;
                                                    }
                                        }


                                        onSelectedItemChanged:
                                        {
                                            if (myEffect && myEffect.effect)
                                            {
                                                myEffect.effect.fromAgentIOP = oEffectsMappingFROMCombo.selectedItem;
                                            }
                                        }

                                    }

                                    Item {
                                        id : disableMappingItem
                                        anchors {
                                            left : agentFROMEffectMappingCombo.right
                                            right : agentTOEffectMappingCombo.left
                                            top : agentFROMEffectMappingCombo.top
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
                                                topMargin: agentFROMEffectMappingCombo.height/2
                                                bottomMargin: agentFROMEffectMappingCombo.height/2
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
                                                topMargin: agentFROMEffectMappingCombo.height/2
                                                bottomMargin: agentFROMEffectMappingCombo.height/2
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
                                                if (myEffect && myEffect.effect)
                                                {
                                                    myEffect.effect.effect = checked ? ActionEffectValueType.ENABLE : ActionEffectValueType.DISABLE;
                                                }
                                            }

                                            Binding {
                                                target : enabledbutton
                                                property : "checked"
                                                value : myEffect && myEffect.effect && myEffect.effect.effect === ActionEffectValueType.ENABLE ? true : false;

                                            }

                                        }
                                    }

                                    // Agent TO
                                    MasticComboBox {
                                        id : agentTOEffectMappingCombo

                                        anchors {
                                            right : parent.right
                                            bottom : iEffectsMappingTOCombo.top
                                            bottomMargin: 6
                                        }

                                        height : 25
                                        width : 148

                                        model : controller ? controller.agentsInMappingList : 0
                                        function modelToString(model)
                                        {
                                            return model.name;
                                        }


                                        Binding {
                                            target : agentTOEffectMappingCombo
                                            property : "selectedItem"
                                            value : if (myEffect && myEffect.effect)
                                                    {
                                                        myEffect.effect.toAgentModel;
                                                    } else {
                                                        null;
                                                    }
                                        }


                                        onSelectedItemChanged:
                                        {
                                            if (myEffect && myEffect.effect)
                                            {
                                                myEffect.effect.toAgentModel = agentTOEffectMappingCombo.selectedItem;
                                            }
                                        }

                                    }

                                    // Agent TO Intpus
                                    MasticComboBoxAgentsIOP {
                                        id : iEffectsMappingTOCombo

                                        enabled : visible
                                        anchors {
                                            right : parent.right
                                            bottom : parent.bottom
                                        }

                                        height : 25
                                        width : 148

                                        model : (myEffect && myEffect.effect && myEffect.effect) ? myEffect.effect.toAgentIopList : 0
                                        function modelToString(model)
                                        {
                                            return model.name;
                                        }


                                        Binding {
                                            target : iEffectsMappingTOCombo
                                            property : "selectedItem"
                                            value : if (myEffect && myEffect.effect)
                                                    {
                                                        myEffect.effect.toAgentIOP;
                                                    } else {
                                                        null;
                                                    }
                                        }


                                        onSelectedItemChanged:
                                        {
                                            if (myEffect && myEffect.effect)
                                            {
                                                myEffect.effect.toAgentIOP = iEffectsMappingTOCombo.selectedItem;
                                            }
                                        }

                                    }


                                }


                                // Delete Effect
                                Button {
                                    id: btnDeletEffect

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
                    }

                }



            }


            // Delete Action
            MouseArea {
                id : actionDeleteBtn
                enabled: visible
                visible: (model && model.originalAction !== null)
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
                    if (controller && model && model.originalAction) {
                        controller.deleteAction(model.originalAction);
                    } else {
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

}

