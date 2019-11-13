/*
 *    IngeScape Editor
 *
 *  Copyright © 2017 Ingenuity i/o. All rights reserved.
 *
 *    See license terms for the rights and conditions
 *    defined by copyright holders.
 *
 *
 *    Contributors:
 *      Chloé Roumieu      <roumieu@ingenuity.io>
 */

import QtQuick 2.8
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4
import QtQuick.Window 2.3
import QtWebEngine 1.5

import I2Quick 1.0

import INGESCAPE 1.0

I2PopupBase {
    id: popupNotification

    //----------------------------------
    //
    // Properties
    //
    //----------------------------------

    height: 800
    width: 1200

    isModal: true
    dismissOnOutsideTap: true
    keepRelativePositionToInitialParent: false

    property string notificationUrl : "https://services.ingescape.com/editor/notification"


    //--------------------------------
    //
    // Signals
    //
    //--------------------------------

    // Emitted when notification is available after request
    signal notificationAvailable();

    // Emitted when there is no notification after request
    signal noNotification();


    //--------------------------------
    //
    // Behavior
    //
    //--------------------------------

    // Popup only open itself when there is a remote notification
    onNotificationAvailable: {
        popupNotification.open();
    }


    //--------------------------------
    //
    // Content
    //
    //--------------------------------

    Rectangle {
        anchors.fill: parent

        radius: 5

        border {
            width: 2
            color: IngeScapeTheme.editorsBackgroundBorderColor
        }

        color: IngeScapeTheme.editorsBackgroundColor

        WebEngineView {
            id: webview
            anchors {
                fill: parent
                margins: 15
            }

            url: popupNotification.notificationUrl

            profile: WebEngineProfile {
                offTheRecord: true
            }

            onLoadingChanged: {
                // If our page is not currently loading
                if (loadRequest.status !== WebEngineLoadRequest.LoadStartedStatus) {
                    if ((loadRequest.status === WebEngineLoadRequest.LoadSucceededStatus) && (loadRequest.errorCode) && (loadRequest.errorCode === 200)) { // Loading succeed
                        popupNotification.notificationAvailable();
                    }
                    else { // Loading failed
                        popupNotification.noNotification();
                    }
                }
            }
        }
    }
}
