/*
 *	IngeScape Assessments
 *
 *  Copyright © 2019 Ingenuity i/o. All rights reserved.
 *
 *	See license terms for the rights and conditions
 *	defined by copyright holders.
 *
 *
 *	Contributors:
 *      Mathieu Soum       <soum@ingenuity.io>
 *
 */

import QtQuick 2.9
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4

import I2Quick 1.0

import INGESCAPE 1.0


// This TextArea has a Timer that delay the handling of the user inputed text.
// inputChanged signal is emitted when the Timer expired after the last text edition.
// The Timer delay can be specified by changing the timerDelayInMilliseconds property.
TextArea {
    id: rootItem

    //-------------------------------------------
    //
    // Property
    //
    //-------------------------------------------

    // Delay of the input Timer
    property real timerDelayInMilliseconds: 500


    //-------------------------------------------
    //
    // Signal
    //
    //-------------------------------------------

    // Signal emitted after the input changed and the Timer finished
    signal inputChanged(string text);


    //-------------------------------------------
    //
    // Throttle
    //
    //-------------------------------------------

    // Timer running during data edition to avoid process temporary data
    Timer {
        id: throttleTimer

        repeat: false
        running: false

        property var callback
        property var lastTime

        onTriggered: callback();
    }


    // Callback function bound to throttleTimer
    function throttle(timer, delay, callback) {
        var now = new Date().getTime();

        if (
            (timer.lastTime && (now < (timer.lastTime + delay)))
            ||
            !timer.running
            )
        {
            // Reset timer
            timer.stop();
            timer.callback = callback;
            timer.interval = delay;
            timer.lastTime = now;

            // Start timer
            timer.start();
        }
        else
        {
            timer.lastTime = now;
        }
    }


    Component.onDestruction: {
        if (throttleTimer.running && throttleTimer.callback)
        {
            throttleTimer.stop();
            throttleTimer.callback();
        }
    }


    onTextChanged: {
        if (activeFocus)
        {
            var value = text;

            throttle(throttleTimer, rootItem.timerDelayInMilliseconds, function() {
               rootItem.inputChanged(value);
            });
        }
    }

}
