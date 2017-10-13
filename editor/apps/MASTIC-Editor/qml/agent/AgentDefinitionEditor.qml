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

Item {
    id: rootItem

    anchors.centerIn: parent
    width: 400
    height: 600


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

    // Emitted when user clicks on the close button
    signal clickedOnCloseButton();


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

        Button {
            id: btnCloseEditor

            anchors {
                right: parent.right
                top: parent.top
            }

            text: "X"

            onClicked: {
                // Emit signal "Clicked on Close Button"
                rootItem.clickedOnCloseButton();
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
                            margins: 4
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
                            margins: 4
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
                            margins: 4
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
            width: 300 //childrenRect.width

            Text {
                id: iopName
                text: model.name

                anchors {
                    left: parent.left
                    top: parent.top
                }

                width: 100
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

                width: 100
                height: parent.height
                color: MasticTheme.definitionEditorsLabelColor
                font: MasticTheme.normalFont
            }
        }
    }
}
