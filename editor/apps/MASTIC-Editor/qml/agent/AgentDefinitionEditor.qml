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

    width: 666
    height: 420

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
            width: 2
            color: MasticTheme.definitionEditorsBackgroundBorderColor
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

            property var boundingBox: MasticTheme.svgFileMASTIC.boundsOnElement("supprimer");
            height : boundingBox.height
            width :  boundingBox.width

            anchors {
                verticalCenter: definitionNameItem.verticalCenter
                right : parent.right
                rightMargin: 20
            }

            style: I2SvgButtonStyle {
                fileCache: MasticTheme.svgFileMASTIC

                pressedID: releasedID + "-pressed"
                releasedID: "supprimer"
                disabledID : releasedID
            }

            onClicked: {
                // Close our popup
                rootItem.close();
            }
        }


        // Definition name and version
        Item {
            id : definitionNameItem

            anchors {
                top : parent.top
                topMargin: 15
                left : parent.left
                leftMargin: 18
                right : parent.right
                rightMargin: 18
            }

            height : definitionNameTxt.height

            TextMetrics {
                id : definitionName

                elideWidth: (definitionNameItem.width - versionName.width)
                elide: Text.ElideRight

                text: definition ? definition.name : ""
            }

            Text {
                id : definitionNameTxt

                anchors {
                    left : parent.left
                }

                text : definitionName.elidedText
                color: MasticTheme.definitionEditorsLabelColor
                font {
                    family: MasticTheme.textFontFamily
                    pixelSize: 23
                    weight: Font.Medium
                }

            }

            Text {
                id : versionName
                anchors {
                    bottom: definitionNameTxt.bottom
                    bottomMargin : 2
                    left : definitionNameTxt.right
                    leftMargin: 5
                }

                text: definition ? "(v" + definition.version + ")" : ""
                color: definitionNameTxt.color

                font {
                    family: MasticTheme.textFontFamily
                    pixelSize : 16
                    italic : true
                }
            }
        }


        Text {
            anchors {
                top : definitionNameItem.bottom
                topMargin: 18
                left : definitionNameItem.left
                right : parent.right
                rightMargin: 22
            }

            text: definition ? definition.description + "ddoekd dkodko dkodkoe zko djaiodj endodo,dz, dzop ad,zo d,zpaz z,sioa,d zps ;z;aos;osoz ddoekd dkodko dkodkoe zko djaiodj endodo,dz, dzop ad,zo d,zpaz z,sioa,d zps ;z;aos;osoz ddoekd dkodko dkodkoe zko djaiodj endodo,dz, dzop ad,zo d,zpaz z,sioa,d zps ;z;aos;osoz" : ""

            width: parent.width
            wrapMode: Text.Wrap
            elide : Text.ElideRight
            maximumLineCount : 3

            color: MasticTheme.definitionEditorsAgentDescriptionColor
            font {
                family: MasticTheme.textFontFamily
                pixelSize : 16
            }
        }



        TabView {
            id: tabs

            anchors {
                top: parent.top
                topMargin: 130
                left: parent.left
                right: parent.right
                bottom: parent.bottom
                margins: 20
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

                property var boundingBox: MasticTheme.svgFileMASTIC.boundsOnElement("muteactif");
                height : boundingBox.height
                width :  boundingBox.width

                visible: (model.agentIOPType === AgentIOPTypes.OUTPUT)
                enabled : visible

                anchors {
                    left: iopValue.right
                    top: parent.top
                }

                style: I2SvgButtonStyle {
                    fileCache: MasticTheme.svgFileMASTIC

                    pressedID: releasedID + "-pressed"
                    releasedID: model.isMuted? "muteactif" : "muteinactif"
                    disabledID : releasedID

                }

                onClicked: {
                    //console.log("QML: Mute/UN-mute output " + model.name);
                    model.QtObject.changeMuteOutput();
                }
            }


        }
    }
}
