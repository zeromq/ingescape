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

    width: 466
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
    property var actionVM: model.editedAction;

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
                    text: actionVM && actionVM.actionModel ? actionVM.actionModel.name : ""

                    style: I2TextFieldStyle {
                        backgroundColor: MasticTheme.darkBlueGreyColor
                        borderColor: MasticTheme.lightGreyColor;
                        borderErrorColor: MasticTheme.redColor
                        radiusTextBox: 1
                        borderWidth: 0;
                        borderWidthActive: 1
                        textColor: MasticTheme.lightGreyColor;

                        padding.left: 3
                        padding.right: 3

                        font {
                            pixelSize:15
                            family: MasticTheme.textFontFamily
                        }

                    }

                    onTextChanged: {
                        if (activeFocus &&  actionVM && actionVM.actionModel) {
                            actionVM.actionModel.name = text;
                        }
                    }

                    Binding {
                        target : textFieldName
                        property :  "text"
                        value : if (actionVM && actionVM.actionModel) {
                                    actionVM.actionModel.name
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
                                value : if (actionVM && controller)
                                        {
                                            controller.validationDurationsTypesList.getItemWithValue(actionVM.validityDurationType);
                                        } else {
                                            null;
                                        }
                            }


                            onSelectedItemChanged:
                            {
                                if (actionVM)
                                {
                                    actionVM.validityDurationType = validityDurationCombo.selectedItem.value;
                                }
                            }

                        }

                        TextField {
                            id: textFieldValidity

                            anchors {
                                verticalCenter : parent.verticalCenter
                            }

                            height: 25
                            width: 47
                            horizontalAlignment: TextInput.AlignLeft
                            verticalAlignment: TextInput.AlignVCenter

                            text : actionVM && actionVM.actionModel ? actionVM.actionModel.validityDuration : "0.0"
                            inputMethodHints: Qt.ImhFormattedNumbersOnly
                            validator: RegExpValidator { regExp: /(\d{1,4})([.]\d{3})?$/ }

                            style: I2TextFieldStyle {
                                backgroundColor: MasticTheme.darkBlueGreyColor
                                borderColor: MasticTheme.lightGreyColor;
                                borderErrorColor: MasticTheme.redColor
                                radiusTextBox: 1
                                borderWidth: 0;
                                borderWidthActive: 1
                                textColor: MasticTheme.lightGreyColor;

                                padding.left: 3
                                padding.right: 3

                                font {
                                    pixelSize:15
                                    family: MasticTheme.textFontFamily
                                }

                            }

                            onTextChanged: {
                                if (activeFocus &&  actionVM && actionVM.actionModel) {
                                    actionVM.actionModel.validityDuration = text;
                                }
                            }

                            Binding {
                                target : textFieldName
                                property :  "text"
                                value : if (actionVM && actionVM.actionModel) {
                                            actionVM.actionModel.validityDuration
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

                            text : "seconds"

                            color: MasticTheme.lightGreyColor
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
                Column {
                    id : conditionsListColumn
                    anchors {
                        top : validityDuration.bottom
                        topMargin: 18
                        right : parent.right
                        left : parent.left
                    }
                    spacing : 6

                    Repeater {
                        model : actionVM ? actionVM.conditionsList : 0

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

                            // my conditions
                            property var myCondition: model.QtObject

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
                                                color: control.checked? MasticTheme.lightGreyColor : MasticTheme.greyColor

                                                text: model.name
                                                elide: Text.ElideRight

                                                font {
                                                    family: MasticTheme.labelFontFamily
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
                                        return model.agentName;
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
                                        if (myCondition && myCondition.condition )
                                        {
                                            myCondition.condition.agentModel = agentCombo.selectedItem;
                                        }
                                    }

                                }

                                // Agent Inputs/Outputs
                                MasticComboBox {
                                    id : ioCombo

                                    visible : myCondition && myCondition.conditionType === ActionConditionType.VALUE

                                    anchors {
                                        verticalCenter : parent.verticalCenter
                                    }

                                    height : 25
                                    width : 148

                                    model : (myCondition && myCondition.condition && myCondition.condition.agentModel) ? myCondition.condition.agentModel.inputsList : 0
                                    function modelToString(model)
                                    {
                                        return model.agentName;
                                    }


                                    Binding {
                                        target : agentCombo
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

                                    anchors {
                                        verticalCenter : parent.verticalCenter
                                    }

                                    height : 25
                                    width : (myCondition && myCondition.conditionType === ActionConditionType.VALUE) ? 45 : 78

                                    model : controller ? controller.comparisonsAgentsTypesList : 0
                                    function modelToString(model)
                                    {
                                        return model.name;
                                    }


                                    Binding {
                                        target : comparisonCombo
                                        property : "selectedItem"
                                        value : if (myCondition && myCondition.condition && controller)
                                                {
                                                    controller.comparisonsAgentsTypesList.getItemWithValue(myCondition.condition.comparison);
                                                } else {
                                                    null;
                                                }
                                    }


                                    onSelectedItemChanged:
                                    {
                                        if (myCondition && myCondition.condition)
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

                                    height: 25
                                    width: 49

                                    horizontalAlignment: TextInput.AlignLeft
                                    verticalAlignment: TextInput.AlignVCenter

                                    text : actionVM && actionVM.actionModel ? actionVM.actionModel.validityDuration : "0.0"
                                    //   inputMethodHints: Qt.ImhFormattedNumbersOnly
                                    //   validator: RegExpValidator { regExp: /(\d{1,4})([.]\d{3})?$/ }

                                    style: I2TextFieldStyle {
                                        backgroundColor: MasticTheme.darkBlueGreyColor
                                        borderColor: MasticTheme.lightGreyColor;
                                        borderErrorColor: MasticTheme.redColor
                                        radiusTextBox: 1
                                        borderWidth: 0;
                                        borderWidthActive: 1
                                        textColor: MasticTheme.lightGreyColor;

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


                            // Delete Conditions
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


                // add conditions
                Button {
                    id: addCondition

                    activeFocusOnPress: true

                    anchors {
                        top: conditionsListColumn. bottom
                        topMargin: 6
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


            //
            // Advanced mode
            //
            Item {
                anchors {
                    left : parent.left
                    right : parent.right
                    top : conditionsItem.top
                    topMargin: 320
                }

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


                CheckBox {
                    id : revertActionCB
                    anchors {
                        left: parent.left;
                        right: parent.right;
                        top : separatorAdvMode.bottom
                        topMargin: 8
                    }

                    checked : actionVM && actionVM.shallRevert;
                    activeFocusOnPress: true;

                    style: CheckBoxStyle {
                        label: Text {
                            color: control.checked? MasticTheme.lightGreyColor : MasticTheme.greyColor

                            text: "Revert action"
                            elide: Text.ElideRight

                            font {
                                family: MasticTheme.labelFontFamily
                                pixelSize: 15
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
                        if (actionVM) {
                            actionVM.shallRevert = checked
                        }
                    }


                    Binding {
                        target : revertActionCB
                        property : "checked"
                        value : (actionVM && actionVM.shallRevert)
                    }
                }


            }






            //            Row {
            //                anchors {
            //                    left: parent.left
            //                    right: parent.right
            //                }
            //                spacing: 2
            //                Column {
            //                    height: 50
            //                    spacing: 2
            //                    CheckBox {
            //                        checked: actionVM && actionVM.actionModel && actionVM.actionModel.shallRevert ? true : false

            //                        text : "Réciproque"
            //                    }

            //                }

            //                Column  {
            //                    height: 50
            //                    spacing: 2
            //                    Row {
            //                        height: 25
            //                        RadioButton {
            //                            id : rdButtRevertWhenValidityIsOver
            //                            text : "après"
            //                            checked: actionVM && actionVM.revertAfterTime === true ? true : false;

            //                            Binding {
            //                                target : rdButtRevertWhenValidityIsOver
            //                                property :  "checked"
            //                                value : actionVM.revertAfterTime
            //                            }

            //                            onCheckedChanged: {
            //                                // Update out model
            //                                if (actionVM) {
            //                                    actionVM.revertAfterTime = checked;
            //                                }
            //                            }
            //                        }

            //                        TextField {
            //                            text : actionVM && actionVM.actionModel ? actionVM.actionModel.validityDuration : "0"
            //                            width:100
            //                        }
            //                    }
            //                    Row {
            //                        height: 25
            //                        RadioButton {
            //                            id : rdButtRevertAtTime
            //                            text : "à"
            //                            checked: actionVM && actionVM.revertAtTime === true ? true : false;

            //                            Binding {
            //                                target : rdButtRevertAtTime
            //                                property :  "checked"
            //                                value : actionVM.revertAtTime
            //                            }

            //                            onCheckedChanged: {
            //                                // Update out model
            //                                if (actionVM) {
            //                                    actionVM.revertAtTime = checked;
            //                                }
            //                            }
            //                        }

            //                        TextField {
            //                            text : actionVM && actionVM.actionModel ? actionVM.actionModel.revertAtTime : "0"
            //                            width:100
            //                        }
            //                    }
            //                }
            //            }

            //            Row {
            //                anchors {
            //                    left: parent.left
            //                    right: parent.right
            //                }
            //                spacing: 2
            //                CheckBox {
            //                    checked: actionVM && actionVM.actionModel && actionVM.actionModel.shallRearm ? true : false

            //                    text : "Récurrence"
            //                }
            //            }


            //        Button {
            //            id: btnDeleteEditor

            //            anchors {
            //                right: btnCancelEditor.left
            //                bottom: parent.bottom
            //                rightMargin: 10
            //            }

            //            text: "SUPPRIMER"

            //            onClicked: {

            //                if(controller)
            //                {
            //                    controller.deleteActionEditor(model.QtObject);
            //                }
            //            }
            //        }


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

