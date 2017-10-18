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
    property var actionVM: model;


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
                    text: actionVM && actionVM.actionModel ? actionVM.actionModel.name : ""
                    width:100
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
                    text: actionVM && actionVM.actionModel ? actionVM.actionModel.startTime.toLocaleTimeString(Qt.locale(), "HH'h'mm") : ""
                    width:100
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
                    CheckBox {
                        checked: actionVM && actionVM.actionModel && actionVM.actionModel.shallRearm ? true : false

                        text : "Récurrence"
                    }
                }

                Column  {
                    height: 50
                    spacing: 2
                    Row {
                        height: 25
                        RadioButton {
                            text : "après"
                            checked: actionVM && actionVM.actionModel && actionVM.actionModel.revertWhenValidityIsOver ? true : false
                        }

                        TextField {
                            text : actionVM && actionVM.actionModel ? actionVM.actionModel.validityDuration : "0"
                            width:100
                        }
                    }
                    Row {
                        height: 25
                        RadioButton {
                            text : "à"
                            checked: actionVM && actionVM.actionModel && actionVM.actionModel.revertAtTime !== 0 ? true : false
                        }

                        TextField {
                            text : actionVM && actionVM.actionModel ? actionVM.actionModel.revertAtTime : "0"
                            width:100
                        }
                    }
                }
            }
        }
    }
}
