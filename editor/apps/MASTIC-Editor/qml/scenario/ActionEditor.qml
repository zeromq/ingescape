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

    width: 550
    height: 600

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

    // our controller
    property var controller: null;


    //--------------------------------
    //
    // Signals
    //
    //--------------------------------

    // Emitted when user pressed our popup
    signal bringToFront();


    onClosed: {
        if (contoller) {
            controller.closeActionEditor(model.QtObject);
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

            Column {
                anchors {
                    left : parent.left
                    right : parent.right
                    top : titleTxt.bottom
                    topMargin : 18
                }

                spacing: 5

                /// Name
                Item {
                    anchors {
                        left : parent.left
                        right : parent.right
                    }

                    height: textFieldName.height

                    Text {
                        anchors {
                            left : parent.left
                            verticalCenter : parent.verticalCenter
                        }

                        text : "Name:"

                        color: MasticTheme.lightGreyColor
                        font {
                            family: MasticTheme.textFontFamily
                            pixelSize: 16
                        }
                    }

                    TextField {
                        id: textFieldName

                        anchors {
                            right: parent.right
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

                /// Validity duration
                Item {
                    anchors {
                        left : parent.left
                        right : parent.right
                    }

                    height: validityDurationCombo.height


                    Text {
                        anchors {
                            left : parent.left
                            verticalCenter : parent.verticalCenter
                        }

                        text : "Validity duration:"

                        color: MasticTheme.lightGreyColor
                        font {
                            family: MasticTheme.textFontFamily
                            pixelSize: 16
                        }
                    }

                    MasticComboBox {
                        id : validityDurationCombo

                        anchors {
                            verticalCenter : parent.verticalCenter
                            right : textFieldValidity.left
                            rightMargin: 6
                        }

                        height : 25
                        width : 117

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
                            if (actionVM && controller)
                            {
                                controller.validationDurationsTypesList = validityDurationCombo.selectedItem.value;
                            }
                        }

                    }

                    TextField {
                        id: textFieldValidity

                        anchors {
                            right: parent.right
                            verticalCenter : parent.verticalCenter
                        }

                        height: 25
                        width: 80
                        horizontalAlignment: TextInput.AlignHCenter
                        verticalAlignment: TextInput.AlignVCenter

                        text : actionVM && actionVM.actionModel ? actionVM.actionModel.validityDuration : "0.0"
                        inputMethodHints: Qt.ImhFormattedNumbersOnly
                        validator: RegExpValidator { regExp: /^[0-9]{0,9}$/ }

                        style: I2TextFieldStyle {
                            backgroundColor: MasticTheme.darkBlueGreyColor
                            borderColor: MasticTheme.lightGreyColor;
                            borderErrorColor: ENEDISTheme.redColor
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

