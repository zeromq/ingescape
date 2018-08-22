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
 *
 */

import QtQuick 2.8
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4

import I2Quick 1.0

import INGESCAPE 1.0

Item {


    //----------------------------
    //
    // Main BackgroundImage
    //
    //----------------------------

    Rectangle {
        anchors.fill: parent
        color: IngeScapeTheme.blackColor
    }

    Image {
        id: backgroundImage
        anchors.fill: parent
        fillMode: Image.PreserveAspectCrop
        source: "qrc:/resources/images/Network.jpg"
        opacity: 0.4
    }


    //----------------------------
    //
    // Bottom Right Images
    //
    //----------------------------

    Row {
        anchors {
            right: parent.right
            rightMargin: 20
            bottom: parent.bottom
            bottomMargin: 20
        }
        spacing: 20

        /*Image {
            id: logo?
            fillMode: Image.PreserveAspectFit
            source: "qrc:/resources/images/Logo-?.jpg"
        }*/

        Image {
            id: logoI2
            fillMode: Image.PreserveAspectFit
            height: 86
            source: "qrc:/resources/images/Logo-Ingenuity.png"
        }
    }

    //----------------------------
    //
    // Titles
    //
    //----------------------------

    Column {

        anchors {
            horizontalCenter: parent.horizontalCenter
            top: parent.top
            topMargin: 75
        }

        spacing: 45

        Text {

            anchors.horizontalCenter: parent.horizontalCenter

            text: qsTr("Bienvenue dans l'éditeur")

            verticalAlignment: Text.AlignHCenter

            font: IngeScapeTheme.headingFont

            color: IngeScapeTheme.whiteColor
        }
    }



    //----------------------------
    //
    // Loading
    //
    //----------------------------


    Item {
        id: ingescape

        anchors.centerIn: parent

        width: childrenRect.width
        height: childrenRect.height

        property int blockSize: 16
        property int blockRadius: 4
        property int blockSpacing: 2


        BlockLoadingAnimation {
            id: letterI

            blockSize: ingescape.blockSize
            blockRadius: ingescape.blockRadius
            blockSpacing: ingescape.blockSpacing

            visibilitiesOfBlock: [
                true, true, true, true, true,
                false, false, true, false, false,
                false, false, true, false, false,
                false, false, true, false, false,
                false, false, true, false, false,
                false, false, true, false, false,
                true, true, true, true, true
            ]
        }

        BlockLoadingAnimation {
            id: letterN

            anchors {
                left: letterI.right
                leftMargin: ingescape.blockSize + ingescape.blockSpacing
            }

            blockSize: ingescape.blockSize
            blockRadius: ingescape.blockRadius
            blockSpacing: ingescape.blockSpacing

            visibilitiesOfBlock: [
                true, false, false, false, true,
                true, true, false, false, true,
                true, false, true, false, true,
                true, false, true, false, true,
                true, false, true, false, true,
                true, false, false, true, true,
                true, false, false, false, true
            ]

        }

        BlockLoadingAnimation {
            id: letterG

            anchors {
                left: letterN.right
                leftMargin: ingescape.blockSize + ingescape.blockSpacing
            }

            blockSize: ingescape.blockSize
            blockRadius: ingescape.blockRadius
            blockSpacing: ingescape.blockSpacing

            visibilitiesOfBlock: [
                false, true, true, true, false,
                true, false, false, false, true,
                true, false, false, false, false,
                true, false, true, true, true,
                true, false, false, false, true,
                true, false, false, false, true,
                false, true, true, true, false
            ]

        }

        BlockLoadingAnimation {
            id: letterE

            anchors {
                left: letterG.right
                leftMargin: ingescape.blockSize + ingescape.blockSpacing
            }

            blockSize: ingescape.blockSize
            blockRadius: ingescape.blockRadius
            blockSpacing: ingescape.blockSpacing

            visibilitiesOfBlock: [
                false, true, true, true, true,
                true, false, false, false, false,
                true, false, false, false, false,
                true, true, true, true, false,
                true, false, false, false, false,
                true, false, false, false, false,
                false, true, true, true, true
            ]

        }

        BlockLoadingAnimation {
            id: letterS

            anchors {
                left: letterE.right
                leftMargin: ingescape.blockSize + ingescape.blockSpacing
            }

            blockSize: ingescape.blockSize
            blockRadius: ingescape.blockRadius
            blockSpacing: ingescape.blockSpacing

            visibilitiesOfBlock: [
                false, true, true, true, true,
                true, false, false, false, false,
                true, false, false, false, false,
                false, true, true, true, false,
                false, false, false, false, true,
                false, false, false, false, true,
                true, true, true, true, false
            ]
        }

        BlockLoadingAnimation {
            id: letterC

            anchors {
                left: letterS.right
                leftMargin: ingescape.blockSize + ingescape.blockSpacing
            }

            blockSize: ingescape.blockSize
            blockRadius: ingescape.blockRadius
            blockSpacing: ingescape.blockSpacing

            visibilitiesOfBlock: [
                false, true, true, true, false,
                true, false, false, false, true,
                true, false, false, false, false,
                true, false, false, false, false,
                true, false, false, false, false,
                true, false, false, false, true,
                false, true, true, true, false
            ]
        }

        BlockLoadingAnimation {
            id: letterA

            anchors {
                left: letterC.right
                leftMargin: ingescape.blockSize + ingescape.blockSpacing
            }

            blockSize: ingescape.blockSize
            blockRadius: ingescape.blockRadius
            blockSpacing: ingescape.blockSpacing

            visibilitiesOfBlock: [
                false, true, true, true, false,
                true, false, false, false, true,
                true, false, false, false, true,
                true, true, true, true, true,
                true, false, false, false, true,
                true, false, false, false, true,
                true, false, false, false, true
            ]
        }

        BlockLoadingAnimation {
            id: letterP

            anchors {
                left: letterA.right
                leftMargin: ingescape.blockSize + ingescape.blockSpacing
            }

            blockSize: ingescape.blockSize
            blockRadius: ingescape.blockRadius
            blockSpacing: ingescape.blockSpacing

            visibilitiesOfBlock: [
                true, true, true, true, false,
                true, false, false, false, true,
                true, false, false, false, true,
                true, true, true, true, false,
                true, false, false, false, false,
                true, false, false, false, false,
                true, false, false, false, false
            ]
        }

        BlockLoadingAnimation {
            id: letterEEnd

            anchors {
                left: letterP.right
                leftMargin: ingescape.blockSize + ingescape.blockSpacing
            }

            blockSize: ingescape.blockSize
            blockRadius: ingescape.blockRadius
            blockSpacing: ingescape.blockSpacing

            visibilitiesOfBlock: [
                false, true, true, true, true,
                true, false, false, false, false,
                true, false, false, false, false,
                true, true, true, true, false,
                true, false, false, false, false,
                true, false, false, false, false,
                false, true, true, true, true
            ]

        }
    }

    Patxi {
        anchors {
            top: ingescape.bottom
            topMargin: ingescape.blockSize
            horizontalCenter: ingescape.horizontalCenter
        }
    }


    //----------------------------
    //
    // Bottom informations
    //
    //----------------------------

    Column {

        anchors {
            bottom: parent.bottom
            bottomMargin: 20
            horizontalCenter: parent.horizontalCenter
        }

        Text {
            anchors.horizontalCenter: parent.horizontalCenter

            text: qsTr("IngeScape Editor")

            font: IngeScapeTheme.heading2Font

            color: IngeScapeTheme.whiteColor
        }

        // Version
        Text {
            id: version

            anchors.horizontalCenter: parent.horizontalCenter

            text: qsTr("Version %1").arg(Qt.application.version)

            font: IngeScapeTheme.heading2Font

            color: IngeScapeTheme.whiteColor
        }
    }
}
