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


    //----------------------------
    //
    // Main BackgroundImage
    //
    //----------------------------

    Rectangle {
        anchors.fill: parent
        color: MasticTheme.blackColor
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

            text: qsTr("Bienvenue dans l'éditeur MASTIC")

            verticalAlignment: Text.AlignHCenter

            font: MasticTheme.headingFont

            color: MasticTheme.whiteColor
        }
    }



    //----------------------------
    //
    // Loading Svg Item
    //
    //----------------------------

    /*I2SvgItem {
        svgFileCache: MasticTheme.svgFileMASTIC
        svgElementId: "login-chargement"

        anchors {
            verticalCenter: parent.verticalCenter
            horizontalCenter: parent.horizontalCenter
        }

        RotationAnimator on rotation {
            //easing.type: Easing.InOutSine
            loops: Animation.Infinite
            //easing.amplitude:
            from: 0;
            to: 360;
            duration: 2000
        }
    }*/


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

            text: qsTr("MASTIC Editor")

            font: MasticTheme.heading2Font

            color: MasticTheme.whiteColor
        }

        // Version
        Text {
            id: version

            anchors.horizontalCenter: parent.horizontalCenter

            text: qsTr("Version %1").arg(Qt.application.version)

            font: MasticTheme.heading2Font

            color: MasticTheme.whiteColor
        }
    }
}
