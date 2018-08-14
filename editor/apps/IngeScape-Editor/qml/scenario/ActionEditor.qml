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
import ".." as Editor;

Window {
    id: rootItem

    title: (actionM ? actionM.name : "Action")

    height: minimumHeight
    width: minimumWidth

    minimumWidth: 475
    minimumHeight: IngeScapeTheme.appMinHeight

    //maximumWidth: 475
    maximumHeight: IngeScapeTheme.appMinHeight

    flags: Qt.Dialog

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
    property var actionM: panelController ?  panelController.editedAction : null;
    // action view model
    property var actionVM: panelController ? panelController.editedViewModel : null;

    // our scenario controller
    property var controller: null;
    // our panel controller
    property var panelController: null;

    property var heightStartTime: (startTimeItem.visible ? (startTimeItem.height + startTimeItem.anchors.topMargin) : 0)


    //--------------------------------
    //
    // Signals
    //
    //--------------------------------



    //--------------------------------
    //
    // Behavior
    //
    //--------------------------------

    Connections {
        target: panelController

        //ignoreUnknownSignals: true

        onBringToFront: {
            //console.log("QML of Action Editor: onBringToFront");

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
            width: 1
            color: IngeScapeTheme.editorsBackgroundBorderColor
        }
        color: IngeScapeTheme.editorsBackgroundColor


        Item {
            id: mainItem

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

                text: (actionVM ? qsTr("Action in timeline") : qsTr("Action"))

                elide: Text.ElideRight
                color: IngeScapeTheme.whiteColor
                font {
                    family: IngeScapeTheme.textFontFamily
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
                    text: (actionM ? actionM.name : "")

                    style: I2TextFieldStyle {
                        backgroundColor: IngeScapeTheme.darkBlueGreyColor
                        borderColor: IngeScapeTheme.whiteColor;
                        borderErrorColor: IngeScapeTheme.redColor
                        radiusTextBox: 1
                        borderWidth: 0;
                        borderWidthActive: 1
                        textIdleColor: IngeScapeTheme.whiteColor;
                        textDisabledColor: IngeScapeTheme.darkGreyColor;

                        padding.left: 3
                        padding.right: 3

                        font {
                            pixelSize:15
                            family: IngeScapeTheme.textFontFamily
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
                        }
                        else {
                            textFieldName.selectAll();
                        }
                    }


                    Binding {
                        target: textFieldName
                        property: "text"
                        value: (actionM ? actionM.name : "")
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

                    color: IngeScapeTheme.lightGreyColor
                    font {
                        family: IngeScapeTheme.textFontFamily
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
                        backgroundColor: IngeScapeTheme.darkBlueGreyColor
                        borderColor: IngeScapeTheme.whiteColor;
                        borderErrorColor: IngeScapeTheme.redColor
                        radiusTextBox: 1
                        borderWidth: 0;
                        borderWidthActive: 1
                        textIdleColor: IngeScapeTheme.whiteColor;
                        textDisabledColor: IngeScapeTheme.darkGreyColor;

                        padding.left: 3
                        padding.right: 3

                        font {
                            pixelSize:15
                            family: IngeScapeTheme.textFontFamily
                        }

                    }

                    onActiveFocusChanged: {
                        if (!activeFocus) {
                            // Move cursor to our first character when we lose focus
                            // (to always display the beginning or our text instead of
                            // an arbitrary part if our text is too long)
                            cursorPosition = 0;
                        }
                        else {
                            textFieldStartTime.selectAll();
                        }
                    }


                    onTextChanged: {
                        if (activeFocus && actionVM) {
                            actionVM.startTimeString = text;
                        }
                    }

                    Binding {
                        target: textFieldStartTime
                        property: "text"
                        value: (actionVM ? actionVM.startTimeString : "")
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
                    top : (startTimeItem.visible) ? startTimeItem.bottom : nameItem.bottom
                    topMargin: 15
                }
                height: Math.min(titleEffects.height + 6 + scrollView.anchors.topMargin + scrollView.contentItem.height,
                                 mainItem.height - (titleTxt.height + nameItem.height + nameItem.anchors.topMargin + heightStartTime + effectsListItem.anchors.topMargin + conditionsItem.height + conditionsItem.anchors.topMargin + advancedModesItem.height + advancedModesItem.anchors.topMargin + 10 + okButton.height))

                Behavior on height {
                    NumberAnimation {}
                }

//                Rectangle {
//                    color: "red"
//                    anchors.fill: parent
//                }

                // Title
                Text {
                    id : titleEffects
                    anchors {
                        left : parent.left
                        right : parent.right
                    }

                    text : "Effects"

                    color: IngeScapeTheme.whiteColor
                    font {
                        family: IngeScapeTheme.textFontFamily
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
                    color : IngeScapeTheme.whiteColor
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

                    style: IngeScapeScrollViewStyle {
                    }

                    // Prevent drag overshoot on Windows
                    flickableItem.boundsBehavior: Flickable.OvershootBounds

                    /// Effects List
                    contentItem: Column {
                        id: effectsList
                        spacing: 6
                        height: childrenRect.height
                        width: effectsListItem.width - 9 // scrollbar size

                        Repeater {
                            model : actionM ? actionM.effectsList : 0

                            Rectangle {

                                // my effect
                                property var myEffect: model.QtObject

                                height: (myEffect && (myEffect.effectType === ActionEffectTypes.MAPPING)) ? 90 : 62
                                anchors {
                                    left: parent.left
                                    right: parent.right
                                }

                                color : "transparent"
                                radius: 5
                                border {
                                    width : 1
                                    color : IngeScapeTheme.blackColor
                                }

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
                                        model : controller ? controller.effectsTypesList : 0

                                        CheckBox {
                                            id : effectTypeCB
                                            anchors {
                                                verticalCenter: parent.verticalCenter;
                                            }

                                            checked: myEffect && (myEffect.effectType === model.value)
                                            exclusiveGroup: effectTypesExclusifGroup
                                            activeFocusOnPress: true;

                                            style: CheckBoxStyle {
                                                label: Text {
                                                    anchors {
                                                        verticalCenter: parent.verticalCenter
                                                        verticalCenterOffset: 2
                                                    }
                                                    color: control.checked ? IngeScapeTheme.whiteColor : IngeScapeTheme.lightGreyColor

                                                    text: model.name
                                                    elide: Text.ElideRight

                                                    font {
                                                        family: IngeScapeTheme.textFontFamily
                                                        pixelSize: 15
                                                    }
                                                }

                                                indicator: Rectangle {
                                                    implicitWidth: 14
                                                    implicitHeight: 14
                                                    radius : height / 2
                                                    border.width: 0;
                                                    color : IngeScapeTheme.darkBlueGreyColor

                                                    Rectangle {
                                                        anchors.centerIn: parent
                                                        visible : control.checked
                                                        width: 8
                                                        height: 8
                                                        radius: height / 2

                                                        border.width: 0
                                                        color: IngeScapeTheme.whiteColor
                                                    }
                                                }

                                            }

                                            onCheckedChanged : {
                                                if (myEffect && checked) {
                                                    myEffect.effectType = model.value
                                                }
                                            }


                                            Binding {
                                                target: effectTypeCB
                                                property: "checked"
                                                value: (myEffect && (myEffect.effectType === model.value))
                                            }
                                        }
                                    }
                                }


                                //
                                // Effect Details for Agent and Value
                                //
                                Item {
                                    anchors {
                                        right: parent.right
                                        rightMargin: 10
                                        left: rowEffectsTypes.left
                                        bottom: parent.bottom
                                        bottomMargin: 6
                                    }
                                    height: 25

                                    visible: (myEffect && myEffect.effectType !== ActionEffectTypes.MAPPING)

                                    // Agent
                                    IngeScapeComboBox {
                                        id : agentEffectCombo

                                        anchors {
                                            left: parent.left
                                            verticalCenter : parent.verticalCenter
                                        }
                                        height: parent.height
                                        width: 148

                                        model: controller ? controller.agentsInMappingList : 0

                                        enabled: (controller && controller.agentsInMappingList.count !== 0)
                                        placeholderText: (controller && (controller.agentsInMappingList.count === 0) ? "- No Item -"
                                                                                                                     : "- Select an item -")

                                        function modelToString(model)
                                        {
                                            return model.name;
                                        }


                                        Binding {
                                            target: agentEffectCombo
                                            property: "selectedItem"
                                            value: if (myEffect && myEffect.modelM)
                                                   {
                                                       myEffect.modelM.agent;
                                                   }
                                                   else {
                                                       null;
                                                   }
                                        }

                                        onSelectedItemChanged: {
                                            if (myEffect && myEffect.modelM)
                                            {
                                                myEffect.modelM.agent = agentEffectCombo.selectedItem;
                                            }
                                        }
                                    }

                                    // Agent Inputs/Outputs
                                    IngeScapeComboBoxAgentsIOP {
                                        id : iopEffectsCombo

                                        anchors {
                                            left: agentEffectCombo.right
                                            leftMargin: 6
                                            verticalCenter : parent.verticalCenter
                                        }

                                        height: 25
                                        width: 148

                                        visible: myEffect && (myEffect.effectType === ActionEffectTypes.VALUE)
                                        enabled: visible

                                        model: (myEffect && myEffect.modelM) ? myEffect.modelM.iopMergedList : 0

                                        inputsNumber: (myEffect && myEffect.modelM && myEffect.modelM.agent) ? myEffect.modelM.agent.inputsList.count : 0;
                                        outputsNumber: (myEffect && myEffect.modelM && myEffect.modelM.agent) ? myEffect.modelM.agent.outputsList.count : 0;
                                        parametersNumber: (myEffect && myEffect.modelM && myEffect.modelM.agent) ? myEffect.modelM.agent.parametersList.count : 0;

                                        function modelToString(model)
                                        {
                                            return model.name;
                                        }


                                        Binding {
                                            target: iopEffectsCombo
                                            property: "selectedItem"
                                            value: if (myEffect && myEffect.modelM && myEffect.modelM.agentIOP)
                                                   {
                                                       console.log("QML (Binding): Combo set agent IOP to " + myEffect.modelM.agentIOP.name);
                                                       myEffect.modelM.agentIOP;
                                                   }
                                                   else {
                                                       console.log("QML (Binding): Combo set agent IOP to NULL !!!");
                                                       null;
                                                   }
                                        }


                                        onSelectedItemChanged: {
                                            if (myEffect && myEffect.modelM)
                                            {
                                                if (myEffect.modelM.iopMergedList && myEffect.modelM.iopMergedList.count > 0) {
                                                    console.log("QML: Combo Selected Item changed...set agent IOP to " + iopEffectsCombo.selectedItem + " (iopMergedList.count = " + myEffect.modelM.iopMergedList.count + ")");
                                                }
                                                else {
                                                    console.log("QML: Combo Selected Item changed...set agent IOP to " + iopEffectsCombo.selectedItem + " (iopMergedList is EMPTY !!!)");
                                                }

                                                myEffect.modelM.agentIOP = iopEffectsCombo.selectedItem;
                                            }
                                        }

                                    }

                                    // Effect Type (on Agent)
                                    IngeScapeComboBox {
                                        id : effectTypeCombo

                                        anchors {
                                            left: agentEffectCombo.right
                                            leftMargin: 6
                                            verticalCenter : parent.verticalCenter
                                        }
                                        height: 25
                                        width: 98

                                        visible: (myEffect && (myEffect.effectType === ActionEffectTypes.AGENT))
                                        enabled: visible

                                        model: (controller ? controller.agentEffectValuesList : 0)

                                        function modelToString(model)
                                        {
                                            return model.name;
                                        }


                                        Binding {
                                            target: effectTypeCombo
                                            property: "selectedItem"
                                            value: if (myEffect && myEffect.modelM && controller)
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
                                            left: iopEffectsCombo.right
                                            leftMargin: 6

                                            right: (btnWarningActionEditor.visible ? btnWarningActionEditor.left : parent.right)
                                            rightMargin: (btnWarningActionEditor.visible ? 6 : 0)

                                            verticalCenter: parent.verticalCenter
                                        }
                                        height: 25

                                        visible: myEffect && (myEffect.effectType === ActionEffectTypes.VALUE)
                                        enabled: visible

                                        horizontalAlignment: TextInput.AlignLeft
                                        verticalAlignment: TextInput.AlignVCenter

                                        text: (myEffect && myEffect.modelM) ? myEffect.modelM.value : ""

                                        style: I2TextFieldStyle {
                                            backgroundColor: IngeScapeTheme.darkBlueGreyColor
                                            borderColor: IngeScapeTheme.whiteColor;
                                            borderErrorColor: IngeScapeTheme.redColor
                                            radiusTextBox: 1
                                            borderWidth: 0;
                                            borderWidthActive: 1
                                            textIdleColor: IngeScapeTheme.whiteColor;
                                            textDisabledColor: IngeScapeTheme.darkGreyColor;

                                            padding.left: 3
                                            padding.right: 3

                                            font {
                                                pixelSize:15
                                                family: IngeScapeTheme.textFontFamily
                                            }

                                        }

                                        onActiveFocusChanged: {
                                            if (!activeFocus) {
                                                // Move cursor to our first character when we lose focus
                                                // (to always display the beginning or our text instead of
                                                // an arbitrary part if our text is too long)
                                                cursorPosition = 0;
                                            }
                                            else {
                                                textFieldTargetValue.selectAll();
                                            }
                                        }

                                        onTextChanged: {
                                            if (activeFocus && (myEffect && myEffect.modelM)) {
                                                myEffect.modelM.value = text;
                                            }
                                        }

                                        Binding {
                                            target: textFieldTargetValue
                                            property: "text"
                                            value: if (myEffect && myEffect.modelM) {
                                                       myEffect.modelM.value
                                                   }
                                                   else {
                                                       "";
                                                   }
                                        }
                                    }


                                    Button {
                                        id: btnWarningActionEditor

                                        anchors {
                                            right: parent.right
                                            rightMargin: 0
                                            verticalCenter: parent.verticalCenter
                                        }

                                        visible: (myEffect && (myEffect.effectType === ActionEffectTypes.VALUE) && myEffect.modelM && (myEffect.modelM.agentIOPType !== AgentIOPTypes.OUTPUT))

                                        activeFocusOnPress: true
                                        checkable: true

                                        style: Theme.LabellessSvgButtonStyle {
                                            fileCache: IngeScapeTheme.svgFileINGESCAPE

                                            pressedID: releasedID + "-pressed"
                                            releasedID: "warningActionEditor"
                                            disabledID : releasedID
                                        }

                                        onClicked: {
                                            infosBulle.open();
                                        }



                                        I2PopupBase {
                                            id: infosBulle

                                            height : backgroundPopup.height
                                            width : backgroundPopup.width
                                            keepRelativePositionToInitialParent: true
                                            layerColor : 'transparent'

                                            onOpened: {
                                                infosBulle.visible = true
                                                infosBulle.x = infosBulle.x - infosBulle.width + 25
                                                infosBulle.y = infosBulle.y - infosBulle.height - 3
                                            }

                                            onClosed: {
                                                // the pop up moves otherwise before being not visible
                                                infosBulle.visible = false
                                            }

                                            I2SvgItem {
                                                id : backgroundPopup

                                                svgFileCache : IngeScapeTheme.svgFileINGESCAPE;
                                                svgElementId: "bulleInfos"
                                            }

                                            Text {
                                                id : textInfos
                                                anchors {
                                                    fill : parent
                                                    margins: 6
                                                }

                                                color : IngeScapeTheme.veryDarkGreyColor
                                                wrapMode: Text.Wrap
                                                text : qsTr("Writing to inputs or parameters cannot be reverted.")
                                                font {
                                                    family : IngeScapeTheme.textFontFamily
                                                    pixelSize: 13
                                                }
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
                                    IngeScapeComboBox {
                                        id : comboEffectOnMapping_OutputAgent

                                        anchors {
                                            left : parent.left
                                            bottom : comboEffectOnMapping_Output.top
                                            bottomMargin: 6
                                        }

                                        height : 25
                                        width : 148

                                        model: controller ? controller.agentsInMappingList : 0

                                        enabled: controller && (controller.agentsInMappingList.count !== 0)
                                        placeholderText: (controller && (controller.agentsInMappingList.count === 0) ? "- No Item -"
                                                                                                                     : "- Select an item -")

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
                                    IngeScapeComboBoxAgentsIOP {
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
                                        id: disableMappingItem

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
                                            color : IngeScapeTheme.blackColor
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
                                            color : IngeScapeTheme.veryDarkGreyColor
                                            border {
                                                width: 1
                                                color : IngeScapeTheme.blackColor
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
                                            color : IngeScapeTheme.veryDarkGreyColor
                                            border {
                                                width: 1
                                                color : IngeScapeTheme.blackColor
                                            }
                                        }

                                        Button {
                                            id : enabledbutton
                                            anchors.centerIn: parent

                                            style: I2SvgToggleButtonStyle {
                                                fileCache: IngeScapeTheme.svgFileINGESCAPE

                                                toggleCheckedReleasedID: "enabledToggle-checked";
                                                toggleCheckedPressedID: "enabledToggle-checked-pressed";
                                                toggleUncheckedReleasedID: "enabledToggle";
                                                toggleUncheckedPressedID: "enabledToggle-pressed";

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
                                                target: enabledbutton
                                                property: "checked"
                                                value: (myEffect && myEffect.modelM && (myEffect.modelM.mappingEffectValue === MappingEffectValues.MAPPED)) ? true
                                                                                                                                                            : false
                                            }
                                        }


                                        Text {
                                            anchors {
                                                horizontalCenter: enabledbutton.horizontalCenter
                                                top: enabledbutton.bottom
                                                topMargin: 3
                                            }

                                            text: (myEffect && myEffect.modelM && (myEffect.modelM.mappingEffectValue === MappingEffectValues.MAPPED)) ? "Mapped"
                                                                                                                                                        : "Not mapped"
                                            color: IngeScapeTheme.whiteColor
                                            font {
                                                family: IngeScapeTheme.textFontFamily
                                                pixelSize: 12
                                            }
                                        }
                                    }

                                    // Input Agent
                                    IngeScapeComboBox {
                                        id : comboEffectOnMapping_InputAgent

                                        anchors {
                                            right : parent.right
                                            bottom : comboEffectOnMapping_Input.top
                                            bottomMargin: 6
                                        }

                                        height : 25
                                        width : 148

                                        model : controller ? controller.agentsInMappingList : 0

                                        enabled: (controller && (controller.agentsInMappingList.count !== 0))
                                        placeholderText: (controller && controller.agentsInMappingList.count === 0 ? "- No Item -"
                                                                                                                   : "- Select an item -")

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
                                    IngeScapeComboBoxAgentsIOP {
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
                                        fileCache: IngeScapeTheme.svgFileINGESCAPE

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

                        // Add effect
                        Button {
                            id: addEffects

                            activeFocusOnPress: true

                            anchors {
                                left: parent.left
                            }

                            style: Theme.LabellessSvgButtonStyle {
                                fileCache: IngeScapeTheme.svgFileINGESCAPE

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


            //
            // Conditions
            //
            Item {
                id : conditionsItem
                anchors {
                    left : parent.left
                    right : parent.right
                    top : effectsListItem.bottom
                    topMargin: 15
                }
                height: isOpened ? Math.min(230, titleConditionsMouseArea.height + 1 + validityDuration.height + validityDuration.anchors.topMargin + scrollViewConditions.contentItem.height + scrollViewConditions.anchors.topMargin)
                                 : titleConditionsMouseArea.height + 1
                clip: true

                Behavior on anchors.top {
                    NumberAnimation {}
                }
                Behavior on height {
                    NumberAnimation {}
                }

                property bool isOpened : false

                Connections {
                    target : rootItem

                    onActiveChanged : {
                        // make the conditions list visible if there are conditions
                        if (rootItem.active && actionM && actionM.conditionsList.count > 0) {
                            conditionsItem.isOpened = true;
                        }
                    }
                }

//                Rectangle {
//                    color: "red"
//                    anchors.fill: parent
//                }

                // Title
                MouseArea {
                    id : titleConditionsMouseArea
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

                        svgFileCache : IngeScapeTheme.svgFileINGESCAPE;
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

                        text: qsTr("Conditions for this action")

                        color: titleConditionsMouseArea.containsPress ? IngeScapeTheme.lightGreyColor : IngeScapeTheme.whiteColor
                        font {
                            family: IngeScapeTheme.textFontFamily
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
                        top : titleConditionsMouseArea.bottom
                    }
                    height : 1
                    color : IngeScapeTheme.whiteColor
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

                        color: IngeScapeTheme.whiteColor
                        font {
                            family: IngeScapeTheme.textFontFamily
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

                        IngeScapeComboBox {
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
                                if (actionM) {
                                    actionM.validityDurationType = validityDurationCombo.selectedItem.value;
                                }
                            }

                        }

                        TextField {
                            id: textFieldValidity

                            anchors {
                                verticalCenter : parent.verticalCenter
                            }

                            visible: actionM && (actionM.validityDurationType === ValidationDurationTypes.CUSTOM)
                            enabled: visible
                            height: 25
                            width: 57
                            horizontalAlignment: TextInput.AlignLeft
                            verticalAlignment: TextInput.AlignVCenter

                            text : actionM ? actionM.validityDurationString : "0.0"
                            inputMethodHints: Qt.ImhFormattedNumbersOnly
                            validator: RegExpValidator { regExp: /(\d{1,5})([.]\d{3})?$/ }

                            style: I2TextFieldStyle {
                                backgroundColor: IngeScapeTheme.darkBlueGreyColor
                                borderColor: IngeScapeTheme.whiteColor;
                                borderErrorColor: IngeScapeTheme.redColor
                                radiusTextBox: 1
                                borderWidth: 0;
                                borderWidthActive: 1
                                textIdleColor: IngeScapeTheme.whiteColor;
                                textDisabledColor: IngeScapeTheme.darkGreyColor;

                                padding.left: 3
                                padding.right: 3

                                font {
                                    pixelSize:15
                                    family: IngeScapeTheme.textFontFamily
                                }

                            }

                            onActiveFocusChanged: {
                                if (!activeFocus) {
                                    // Move cursor to our first character when we lose focus
                                    // (to always display the beginning or our text instead of
                                    // an arbitrary part if our text is too long)
                                    cursorPosition = 0;
                                }
                                else {
                                    textFieldValidity.selectAll();
                                }
                            }

                            onTextChanged: {
                                if (activeFocus &&  actionM ) {
                                    actionM.validityDurationString = text;
                                }
                            }

                            Binding {
                                target: textFieldValidity
                                property: "text"
                                value: (actionM ? actionM.validityDurationString : "")
                            }
                        }

                        Text {
                            anchors {
                                verticalCenter : parent.verticalCenter
                            }

                            visible : textFieldValidity.visible
                            text : "seconds"

                            color: IngeScapeTheme.whiteColor
                            font {
                                family: IngeScapeTheme.textFontFamily
                                pixelSize: 15
                            }
                        }

                    }
                }

                //
                // Conditions List
                //
                ScrollView {
                    id : scrollViewConditions

                    anchors {
                        top : validityDuration.bottom
                        topMargin: 8
                        right : parent.right
                        left : parent.left
                        bottom : conditionsItem.bottom
                    }

                    enabled: conditionsItem.isOpened

                    style: IngeScapeScrollViewStyle {
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
                        width : scrollViewConditions.width - 9 // scrollbar size + 1

                        Repeater {
                            model: (actionM && conditionsItem.isOpened) ? actionM.conditionsList : 0

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
                                    color : IngeScapeTheme.blackColor
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
                                        model : controller ? controller.conditionsTypesList : 0

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
                                                    color: control.checked? IngeScapeTheme.whiteColor : IngeScapeTheme.lightGreyColor

                                                    text: model.name
                                                    elide: Text.ElideRight

                                                    font {
                                                        family: IngeScapeTheme.textFontFamily
                                                        pixelSize: 15
                                                    }
                                                }

                                                indicator: Rectangle {
                                                    implicitWidth: 14
                                                    implicitHeight: 14
                                                    radius : height / 2
                                                    border.width: 0;
                                                    color : IngeScapeTheme.darkBlueGreyColor

                                                    Rectangle {
                                                        anchors.centerIn: parent
                                                        visible : control.checked
                                                        width: 8
                                                        height: 8
                                                        radius : height / 2

                                                        border.width: 0;
                                                        color : IngeScapeTheme.whiteColor
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
                                    IngeScapeComboBox {
                                        id : agentCombo

                                        anchors {
                                            verticalCenter : parent.verticalCenter
                                        }

                                        height : 25
                                        width : 148

                                        model : controller ? controller.agentsInMappingList : 0
                                        enabled: (controller && controller.agentsInMappingList.count !== 0 )
                                        placeholderText : (controller && controller.agentsInMappingList.count === 0 ? "- No Item -" : "- Select an item -")

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
                                    IngeScapeComboBoxAgentsIOP {
                                        id : ioCombo

                                        visible: (myCondition && myCondition.conditionType === ActionConditionTypes.VALUE)
                                        enabled: visible
                                        anchors {
                                            verticalCenter: parent.verticalCenter
                                        }

                                        height : 25
                                        width : 148

                                        model: (myCondition && myCondition.modelM) ? myCondition.modelM.agentIopList : 0

                                        // Condition on a value is available only for outputs (no need of separator position)
                                        inputsNumber: 0 // (myCondition && myCondition.modelM && myCondition.modelM.agent) ? myCondition.modelM.agent.inputsList.count : 0;
                                        outputsNumber: 0
                                        parametersNumber: 0

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


                                    // Combo to select the value of the agent condition
                                    IngeScapeComboBox {
                                        id : comboAgentConditionValues

                                        anchors {
                                            verticalCenter : parent.verticalCenter
                                        }

                                        visible: (myCondition && myCondition.conditionType === ActionConditionTypes.AGENT)

                                        height: 25
                                        width: 78

                                        model: (controller ? controller.allAgentConditionValues : 0)

                                        function modelToString(model)
                                        {
                                            return model.name;
                                        }


                                        Binding {
                                            target: comboAgentConditionValues
                                            property: "selectedItem"
                                            value: (myCondition && myCondition.modelM && controller ? controller.allAgentConditionValues.getItemWithValue(myCondition.modelM.agentConditionValue) : null)
                                        }

                                        onSelectedItemChanged: {
                                            if (myCondition && myCondition.modelM && comboAgentConditionValues.selectedItem)
                                            {
                                                myCondition.modelM.agentConditionValue = comboAgentConditionValues.selectedItem.value;
                                            }
                                        }
                                    }


                                    // Combo to select the type of the value comparison
                                    IngeScapeComboBox {
                                        id : comboValueComparisonTypes

                                        anchors {
                                            verticalCenter : parent.verticalCenter
                                        }

                                        visible: (myCondition && myCondition.conditionType === ActionConditionTypes.VALUE)

                                        height: 25
                                        width: 44

                                        model: (controller ? controller.allValueComparisonTypes : 0)

                                        function modelToString(model)
                                        {
                                            return model.name;
                                        }


                                        Binding {
                                            target: comboValueComparisonTypes
                                            property: "selectedItem"
                                            value: (myCondition && myCondition.modelM && controller ? controller.allValueComparisonTypes.getItemWithValue(myCondition.modelM.valueComparisonType) : null)
                                        }

                                        onSelectedItemChanged: {
                                            if (myCondition && myCondition.modelM && comboValueComparisonTypes.selectedItem)
                                            {
                                                myCondition.modelM.valueComparisonType = comboValueComparisonTypes.selectedItem.value;
                                            }
                                        }
                                    }


                                    // Comparison Value
                                    TextField {
                                        id: textFieldComparisonValue

                                        anchors {
                                            verticalCenter : parent.verticalCenter
                                        }

                                        visible: (myCondition && myCondition.conditionType === ActionConditionTypes.VALUE)

                                        enabled : visible
                                        height: 25
                                        width: 49

                                        horizontalAlignment: TextInput.AlignLeft
                                        verticalAlignment: TextInput.AlignVCenter

                                        text : myCondition && myCondition.modelM ? myCondition.modelM.value : ""

                                        style: I2TextFieldStyle {
                                            backgroundColor: IngeScapeTheme.darkBlueGreyColor
                                            borderColor: IngeScapeTheme.whiteColor;
                                            borderErrorColor: IngeScapeTheme.redColor
                                            radiusTextBox: 1
                                            borderWidth: 0;
                                            borderWidthActive: 1
                                            textIdleColor: IngeScapeTheme.whiteColor;
                                            textDisabledColor: IngeScapeTheme.darkGreyColor;

                                            padding.left: 3
                                            padding.right: 3

                                            font {
                                                pixelSize:15
                                                family: IngeScapeTheme.textFontFamily
                                            }

                                        }

                                        onActiveFocusChanged: {
                                            if (!activeFocus) {
                                                // Move cursor to our first character when we lose focus
                                                // (to always display the beginning or our text instead of
                                                // an arbitrary part if our text is too long)
                                                cursorPosition = 0;
                                            }
                                            else {
                                                textFieldComparisonValue.selectAll();
                                            }
                                        }

                                        onTextChanged: {
                                            if (activeFocus && (myCondition && myCondition.modelM)) {
                                                myCondition.modelM.value = text;
                                            }
                                        }

                                        Binding {
                                            target: textFieldComparisonValue
                                            property: "text"
                                            value: if (myCondition && myCondition.modelM) {
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
                                        fileCache: IngeScapeTheme.svgFileINGESCAPE

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
                                fileCache: IngeScapeTheme.svgFileINGESCAPE

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
                id : advancedModesItem
                anchors {
                    left : parent.left
                    right : parent.right
                    top : conditionsItem.bottom
                    topMargin: 15
                }
                clip : true
                height: isOpened ? (titleadvModeMouseArea.height + 1 + revertActionitem.height + revertActionitem.anchors.topMargin + rearmActionitem.height + rearmActionitem.anchors.topMargin + 2)
                                 : titleadvModeMouseArea.height + 1

//                Rectangle {
//                    color: "red"
//                    anchors.fill: parent
//                }

                Behavior on anchors.top {
                    NumberAnimation {}
                }
                Behavior on height {
                    NumberAnimation {}
                }

                property bool isOpened: false

                Connections {
                    target : rootItem
                    Component.onCompleted : {
                        // make the advanced modes visible if there are some modes checked
                        if (actionM && (actionM.shallRevert || actionM.shallRearm)) {
                            advancedModesItem.isOpened = true;
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
                        advancedModesItem.isOpened = !advancedModesItem.isOpened;
                    }

                    I2SvgItem {
                        id : arrowadvModes
                        anchors {
                            left : parent.left
                            verticalCenter: titleAdvMode.verticalCenter
                        }

                        svgFileCache : IngeScapeTheme.svgFileINGESCAPE;
                        svgElementId: "arrowWhite"

                        rotation: advancedModesItem.isOpened? 0 : 270
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

                        color: titleadvModeMouseArea.containsPress ? IngeScapeTheme.lightGreyColor : IngeScapeTheme.whiteColor
                        font {
                            family: IngeScapeTheme.textFontFamily
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
                    color : IngeScapeTheme.whiteColor
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
                    visible : advancedModesItem.isOpened
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
                            label: Text {
                                anchors {
                                    verticalCenter: parent.verticalCenter
                                    verticalCenterOffset: 2
                                }

                                color: control.checked ? IngeScapeTheme.whiteColor : IngeScapeTheme.lightGreyColor

                                text: "Revert action"
                                elide: Text.ElideRight

                                font {
                                    family: IngeScapeTheme.textFontFamily
                                    pixelSize: 16
                                }

                            }

                            indicator: Rectangle {
                                implicitWidth: 14
                                implicitHeight: 14
                                border.width: 0;
                                color : IngeScapeTheme.darkBlueGreyColor

                                I2SvgItem {
                                    visible: control.checked
                                    anchors.centerIn: parent

                                    svgFileCache: IngeScapeTheme.svgFileINGESCAPE;
                                    svgElementId: "check";

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
                                    color: control.enabled ? (control.checked? IngeScapeTheme.whiteColor : IngeScapeTheme.lightGreyColor) : "#3C3C3B"

                                    text: "when conditions check terminates"
                                    elide: Text.ElideRight

                                    font {
                                        family: IngeScapeTheme.textFontFamily
                                        pixelSize: 16
                                    }
                                }

                                indicator: Rectangle {
                                    implicitWidth: 14
                                    implicitHeight: 14
                                    radius : height / 2
                                    border.width: 0;
                                    color : control.enabled ?  IngeScapeTheme.darkBlueGreyColor : "#3C3C3B"

                                    Rectangle {
                                        anchors.centerIn: parent
                                        visible : control.checked
                                        width: 8
                                        height: 8
                                        radius : height / 2

                                        border.width: 0;
                                        color : IngeScapeTheme.whiteColor
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
                                        color: control.enabled ? (control.checked? IngeScapeTheme.whiteColor : IngeScapeTheme.lightGreyColor) : "#3C3C3B"

                                        text: "after " // space allowing to keep selection possible for the whole row

                                        elide: Text.ElideRight

                                        font {
                                            family: IngeScapeTheme.textFontFamily
                                            pixelSize: 16
                                        }
                                    }

                                    indicator: Rectangle {
                                        implicitWidth: 14
                                        implicitHeight: 14
                                        radius : height / 2
                                        border.width: 0;
                                        color : control.enabled ?  IngeScapeTheme.darkBlueGreyColor : "#3C3C3B"

                                        Rectangle {
                                            anchors.centerIn: parent
                                            visible : control.checked
                                            width: 8
                                            height: 8
                                            radius : height / 2

                                            border.width: 0;
                                            color: IngeScapeTheme.whiteColor
                                        }
                                    }

                                }

                                onCheckedChanged : {
                                    if (actionM) {
                                        actionM.shallRevertAfterTime = checked
                                    }
                                }

                                Binding {
                                    target: revertActionAfterCB
                                    property: "checked"
                                    value: (actionM && actionM.shallRevertAfterTime)
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
                                    backgroundColor: IngeScapeTheme.darkBlueGreyColor
                                    backgroundDisabledColor: "#3C3C3B"
                                    borderColor: IngeScapeTheme.whiteColor;
                                    borderErrorColor: IngeScapeTheme.redColor
                                    radiusTextBox: 1
                                    borderWidth: 0;
                                    borderWidthActive: 1
                                    textIdleColor: IngeScapeTheme.whiteColor;
                                    textDisabledColor: IngeScapeTheme.darkGreyColor;

                                    padding.left: 3
                                    padding.right: 3

                                    font {
                                        pixelSize:14
                                        family: IngeScapeTheme.textFontFamily
                                    }

                                }

                                onActiveFocusChanged: {
                                    if (!activeFocus) {
                                        // Move cursor to our first character when we lose focus
                                        // (to always display the beginning or our text instead of
                                        // an arbitrary part if our text is too long)
                                        cursorPosition = 0;
                                    }
                                    else {
                                        textFieldDuration.selectAll();
                                    }
                                }

                                onTextChanged: {
                                    if (activeFocus &&  actionM ) {
                                        actionM.revertAfterTimeString = text;
                                    }
                                }

                                Binding {
                                    target: textFieldDuration
                                    property: "text"
                                    value: (actionM ? actionM.revertAfterTimeString : "")
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
                                color: revertActionAfterCB.enabled ? (revertActionAfterCB.checked? IngeScapeTheme.whiteColor : IngeScapeTheme.lightGreyColor) : "#3C3C3B"

                                text: " seconds"
                                elide: Text.ElideRight

                                font {
                                    family: IngeScapeTheme.textFontFamily
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
                    visible : advancedModesItem.isOpened
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
                                    color: control.checked? IngeScapeTheme.whiteColor : IngeScapeTheme.lightGreyColor

                                    text: "Allow multiple triggers as long as conditions are verified"
                                    elide: Text.ElideRight

                                    font {
                                        family: IngeScapeTheme.textFontFamily
                                        pixelSize: 16
                                    }
                                }

                                indicator: Rectangle {
                                    implicitWidth: 14
                                    implicitHeight: 14
                                    border.width: 0;
                                    color : IngeScapeTheme.darkBlueGreyColor

                                    I2SvgItem {
                                        visible: control.checked
                                        anchors.centerIn: parent

                                        svgFileCache: IngeScapeTheme.svgFileINGESCAPE;
                                        svgElementId: "check";

                                    }
                                }

                            }

                            onCheckedChanged : {
                                if (actionM) {
                                    actionM.shallRearm = checked
                                }
                            }


                            Binding {
                                target: rearmActionCB
                                property: "checked"
                                value: (actionM && actionM.shallRearm)
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

                                color: rearmActionCB.enabled ? (rearmActionCB.checked? IngeScapeTheme.whiteColor : IngeScapeTheme.lightGreyColor) : "#3C3C3B"

                                text: "with trigger every "
                                elide: Text.ElideRight

                                font {
                                    family: IngeScapeTheme.textFontFamily
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
                                    backgroundColor: IngeScapeTheme.darkBlueGreyColor
                                    backgroundDisabledColor: "#3C3C3B"
                                    borderColor: IngeScapeTheme.whiteColor;
                                    borderErrorColor: IngeScapeTheme.redColor
                                    radiusTextBox: 1
                                    borderWidth: 0;
                                    borderWidthActive: 1
                                    textIdleColor: IngeScapeTheme.whiteColor;
                                    textDisabledColor: IngeScapeTheme.darkGreyColor;

                                    padding.left: 3
                                    padding.right: 3

                                    font {
                                        pixelSize:14
                                        family: IngeScapeTheme.textFontFamily
                                    }

                                }

                                onActiveFocusChanged: {
                                    if (!activeFocus) {
                                        // Move cursor to our first character when we lose focus
                                        // (to always display the beginning or our text instead of
                                        // an arbitrary part if our text is too long)
                                        cursorPosition = 0;
                                    }
                                    else {
                                        textFieldTimeBeforeRearm.selectAll();
                                    }
                                }

                                onTextChanged: {
                                    if (activeFocus &&  actionM ) {
                                        actionM.rearmAfterTimeString = text;
                                    }
                                }

                                Binding {
                                    target: textFieldTimeBeforeRearm
                                    property: "text"
                                    value: (actionM ? actionM.rearmAfterTimeString : "")
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

                                color: rearmActionCB.enabled ? (rearmActionCB.checked? IngeScapeTheme.whiteColor : IngeScapeTheme.lightGreyColor) : "#3C3C3B"

                                text: " seconds"
                                elide: Text.ElideRight

                                font {
                                    family: IngeScapeTheme.textFontFamily
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
                    if (controller && panelController && panelController.originalAction)
                    {
                        if (controller.isInsertedInTimeLine(panelController.originalAction))
                        {
                            deleteConfirmationPopup.open();
                        }
                        else {
                            // Delete our action
                            controller.deleteAction(panelController.originalAction);
                        }
                    }
                }

                Text {
                    id: actionDelete

                    anchors {
                        left : parent.left
                    }
                    text : "Delete Action"
                    color: actionDeleteBtn.pressed ? IngeScapeTheme.greyColor : IngeScapeTheme.lightGreyColor
                    elide: Text.ElideRight

                    font {
                        family: IngeScapeTheme.textFontFamily
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


            // Button "Cancel"
            Button {
                id: cancelButton
                activeFocusOnPress: true

                property var boundingBox: IngeScapeTheme.svgFileINGESCAPE.boundsOnElement("button");
                height: boundingBox.height
                width: boundingBox.width

                enabled : visible
                text : "Cancel"

                anchors {
                    verticalCenter: okButton.verticalCenter
                    right : okButton.left
                    rightMargin: 20
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

            // Button "OK"
            Button {
                id: okButton

                property var boundingBox: IngeScapeTheme.svgFileINGESCAPE.boundsOnElement("button");
                height: boundingBox.height
                width: boundingBox.width

                enabled: visible
                activeFocusOnPress: true
                text: "OK"

                anchors {
                    bottom: parent.bottom
                    right : parent.right
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
                    if (controller) {
                        controller.validateActionEditor(model.QtObject);
                    }

                    // Close our popup
                    rootItem.close();
                }
            }
        }

    }


    //
    // Delete Confirmation
    //
    Editor.DeleteConfirmationPopup {
        id : deleteConfirmationPopup

        confirmationText : "This action is used in the scenario.\nDo you want to completely delete it?"

        onDeleteConfirmed: {
            if (panelController.originalAction) {
                // Delete our action
                controller.deleteAction(panelController.originalAction);
            }

        }
    }


    I2Layer {
        id: overlayLayerComboBox
        objectName: "overlayLayerComboBox"

        anchors.fill: parent
    }

    I2Layer {
        id: overlayLayer
        objectName: "overlayLayer"

        anchors.fill: parent
    }
}

