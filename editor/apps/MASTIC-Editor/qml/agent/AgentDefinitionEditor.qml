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

    // our model is a definition
    property var definition: model;


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
                margins: 4
            }
            height: 120

            Text {
                text: definition ? definition.name : ""

                height: 25
                color: MasticTheme.definitionEditorsLabelColor
                font: MasticTheme.heading2Font
            }

            Text {
                text: definition ? definition.version : ""

                height: 25
                color: MasticTheme.definitionEditorsLabelColor
                font: MasticTheme.normalFont
            }

            Text {
                text: definition ? definition.description : ""

                width: parent.width
                wrapMode: Text.Wrap

                color: MasticTheme.definitionEditorsLabelColor
                font: MasticTheme.normalFont
            }
        }

        TabView {
            id: tabs

            anchors {
                top: headers.bottom
                left: parent.left
                right: parent.right
                bottom: parent.bottom
                margins: 4
            }

            Tab {
                title: "Inputs"

                Rectangle {
                    color: MasticTheme.definitionEditorsBackgroundColor
                    border {
                        width: 1
                        color: MasticTheme.whiteColor
                    }

                    ListView {
                        anchors {
                            fill: parent
                            //margins: 4
                            topMargin: 10
                            leftMargin: 2
                            rightMargin: 2
                            bottomMargin: 2
                        }

                        model: definition ? definition.inputsList : 0

                        delegate: componentIOP
                    }
                }
            }
            Tab {
                title: "Parameters"

                Rectangle {
                    color: MasticTheme.definitionEditorsBackgroundColor
                    border {
                        width: 1
                        color: MasticTheme.whiteColor
                    }

                    ListView {
                        anchors {
                            fill: parent
                            //margins: 4
                            topMargin: 10
                            leftMargin: 2
                            rightMargin: 2
                            bottomMargin: 2
                        }

                        model: definition ? definition.parametersList : 0

                        delegate: componentIOP
                    }
                }
            }
            Tab {
                title: "Outputs"

                Rectangle {
                    color: MasticTheme.definitionEditorsBackgroundColor
                    border {
                        width: 1
                        color: MasticTheme.whiteColor
                    }

                    ListView {
                        anchors {
                            fill: parent
                            //margins: 4
                            topMargin: 10
                            leftMargin: 2
                            rightMargin: 2
                            bottomMargin: 2
                        }

                        model: definition ? definition.outputsList : 0

                        delegate: componentIOP
                    }
                }
            }
        }
    }

    Component {
        id: componentIOP

        Item {
            height: 25
            //width: childrenRect.width

            Text {
                id: iopName
                text: model.name

                anchors {
                    left: parent.left
                    top: parent.top
                }

                width: 120
                height: parent.height
                color: MasticTheme.definitionEditorsLabelColor
                font: MasticTheme.normalFont
            }

            Text {
                id: iopType
                text: AgentIOPValueTypes.enumToString(model.agentIOPValueType)

                anchors {
                    left: iopName.right
                    top: parent.top
                }

                width: 100
                height: parent.height
                color: MasticTheme.definitionEditorsLabelColor
                font: MasticTheme.normalFont
            }

            Text {
                id: iopValue
                text: model.displayableDefaultValue

                anchors {
                    left: iopType.right
                    top: parent.top
                }

                width: 150
                height: parent.height
                color: MasticTheme.definitionEditorsLabelColor
                font: MasticTheme.normalFont
            }

            Button {
                id: btnMuteOutput
                visible: (model.agentIOPType === AgentIOPTypes.OUTPUT)

                anchors {
                    left: iopValue.right
                    top: parent.top
                }
                width: 120

                text: "Mute/UN-mute"

                onClicked: {
                    //console.log("QML: Mute/UN-mute output " + model.name);
                    model.QtObject.updateMute(!model.isMuted);
                }
            }
        }
    }
}
