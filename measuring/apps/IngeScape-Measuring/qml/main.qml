/*
 *	IngeScape Measuring
 *
 *  Copyright © 2019 Ingenuity i/o. All rights reserved.
 *
 *	See license terms for the rights and conditions
 *	defined by copyright holders.
 *
 *
 *	Contributors:
 *      Vincent Peyruqueou <peyruqueou@ingenuity.io>
 *
 */

import QtQuick 2.9
import QtQuick.Window 2.2
import QtQuick.Controls 1.4
import QtQml 2.2

//import I2Quick 1.0

//import INGESCAPE 1.0


ApplicationWindow {
    id: mainWindow

    //----------------------------------
    //
    // Properties
    //
    //----------------------------------

    visible: true

    // Default size
    width: 800 // IngeScapeTheme.appDefaultWidth
    height: 600 //IngeScapeTheme.appDefaultHeight

    // Minimum size
    //minimumWidth: IngeScapeTheme.appMinWidth
    //minimumHeight: IngeScapeTheme.appMinHeight

    title: qsTr("IngeScape Measuring - v%1").arg(Qt.application.version)

    //color: IngeScapeTheme.windowBackgroundColor


    //----------------------------------
    //
    // Menu
    //
    //----------------------------------

    Rectangle {
        color: "red"
        width: 200
        height: 200
        x: 50
        y: 50
    }
}
