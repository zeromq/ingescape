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
            color: MasticTheme.whiteColor
        }
        color: MasticTheme.definitionEditorsBackgroundColor


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


        Button {
            id: btnCloseEditor

            anchors {
                right: parent.right
                top: parent.top
            }

            text: "X"
            activeFocusOnPress: true

            onClicked: {
                // Close our popup
                rootItem.close();
            }
        }

        Column {
            id: headers

            anchors {
                top: parent.top
                left: parent.left
                right: parent.right
                bottom: parent.bottom
                margins: 4
            }
            //height: 120

            // Name
            Row {
                anchors {
                    left: parent.left
                    right: parent.right
                }
                height: 25

                Text {
                    text: "Nom "

                    color: MasticTheme.definitionEditorsLabelColor
                }
                TextField {
                    id : actionNameTextField

                    text: actionVM && actionVM.actionModel ? actionVM.actionModel.name : ""
                    width:100

                    onTextChanged: {
                        if (activeFocus &&  actionVM && actionVM.actionModel) {
                            actionVM.actionModel.name = text;
                        }
                    }
                }

                Binding {
                    target : actionNameTextField
                    property :  "text"
                    value : if (actionVM && actionVM.actionModel) {
                              actionVM.actionModel.name
                            }
                            else {
                                "";
                            }
                }



            }



            // Start
            Row {
                anchors {
                    left: parent.left
                    right: parent.right
                }
                height: 25
                Text {
                    text: "Début"

                    color: MasticTheme.definitionEditorsLabelColor
                }
                TextField {
                    id : actionStartTextField
                    text: actionVM ? actionVM.startTimeString: ""
                    width:100

                    Binding {
                        target : actionStartTextField
                        property :  "text"
                        value : actionVM.startTimeString
                    }

                    onTextChanged: {
                        if (activeFocus &&  actionVM) {
                            actionVM.startTimeString = text;
                        }
                    }
                }
            }



            Row {
                anchors {
                    left: parent.left
                    right: parent.right
                }
                spacing: 2
                Column {
                    height: 50
                    spacing: 2
                    CheckBox {
                        checked: actionVM && actionVM.actionModel && actionVM.actionModel.shallRevert ? true : false

                        text : "Réciproque"
                    }

                }

                Column  {
                    height: 50
                    spacing: 2
                    Row {
                        height: 25
                        RadioButton {
                            id : rdButtRevertWhenValidityIsOver
                            text : "après"
                            checked: actionVM && actionVM.revertAfterTime === true ? true : false;

                            Binding {
                                target : rdButtRevertWhenValidityIsOver
                                property :  "checked"
                                value : actionVM.revertAfterTime
                            }

                            onCheckedChanged: {
                                // Update out model
                                if (actionVM) {
                                    actionVM.revertAfterTime = checked;
                                }
                            }
                        }

                        TextField {
                            text : actionVM && actionVM.actionModel ? actionVM.actionModel.validityDuration : "0"
                            width:100
                        }
                    }
                    Row {
                        height: 25
                        RadioButton {
                            id : rdButtRevertAtTime
                            text : "à"
                            checked: actionVM && actionVM.revertAtTime === true ? true : false;

                            Binding {
                                target : rdButtRevertAtTime
                                property :  "checked"
                                value : actionVM.revertAtTime
                            }

                            onCheckedChanged: {
                                // Update out model
                                if (actionVM) {
                                    actionVM.revertAtTime = checked;
                                }
                            }
                        }

                        TextField {
                            text : actionVM && actionVM.actionModel ? actionVM.actionModel.revertAtTime : "0"
                            width:100
                        }
                    }
                }
            }

            Row {
                anchors {
                    left: parent.left
                    right: parent.right
                }
                spacing: 2
                CheckBox {
                    checked: actionVM && actionVM.actionModel && actionVM.actionModel.shallRearm ? true : false

                    text : "Récurrence"
                }
            }
        }


        Button {
            id: btnValideEditor

            anchors {
                right: parent.right
                bottom: parent.bottom
            }

            text: "OK"

            onClicked: {
                if(controller)
                {
                    controller.valideActionEditor(model.QtObject);
                }
            }
        }

        Button {
            id: btnCancelEditor

            anchors {
                right: btnValideEditor.left
                bottom: parent.bottom
                rightMargin: 10
            }

            text: "ANNULER"

            onClicked: {
                // Close our popup
                rootItem.close();
            }
        }

        Button {
            id: btnDeleteEditor

            anchors {
                right: btnCancelEditor.left
                bottom: parent.bottom
                rightMargin: 10
            }

            text: "SUPPRIMER"

            onClicked: {

                if(controller)
                {
                    controller.deleteActionEditor(model.QtObject);
                }
            }
        }

    }
}
