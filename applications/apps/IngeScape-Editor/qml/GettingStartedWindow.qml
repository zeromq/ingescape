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
 *      Mathieu Soum      <soum@ingenuity.io>
 */

import QtQuick 2.8
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4
import QtQuick.Window 2.3
import QtWebEngine 1.5

import I2Quick 1.0

import INGESCAPE 1.0


WindowBlockTouches {
    id: rootItem

    title: qsTr("Getting started")

    property string gettingStartedRemoteUrl: "https://services.ingescape.com/editor/getting_started"
    property string gettingStartedLocalUrl: "qrc:///resources/gettingStarted/gettingStarted.html"

    height: minimumHeight
    width: minimumWidth

    minimumWidth: 1200
    minimumHeight: 880

    // Flag indicating if neither the Internet URL nor the locale URL could be loaded
    property bool noContent : false


    //--------------------------------
    //
    // Content
    //
    //--------------------------------

    // Reset the URL to the Internet URL of the getting started page
    function resetInternetUrl()
    {
        if (rootItem.gettingStartedRemoteUrl !== "")
        {
            console.log("Reset Internet URL")
            webview.stop()
            webview.url = rootItem.gettingStartedRemoteUrl
        }
        else
        {
            console.log("Empty internet URL -> Switch to local URL")
            rootItem.switchToLocalUrl()
        }

    }

    // Reset the URL to the Internet URL of the getting started page
    function switchToLocalUrl()
    {
        if (rootItem.gettingStartedLocalUrl !== "")
        {
            webview.stop()
            console.log("Reset Local URL")
            webview.url = rootItem.gettingStartedLocalUrl
        }
        else
        {
            console.log("Empty local URL -> Loading failed")
            noContent = true;
        }
    }

    function changeUrl(currentUrl) {
        if (currentUrl === rootItem.gettingStartedRemoteUrl)
        {
            console.log(qsTr("Fail loading 'Getting Started' internet URL. Switching to local URL."))
            rootItem.switchToLocalUrl()
        }
        else if (currentUrl === rootItem.gettingStartedLocalUrl)
        {
            console.log(qsTr("Fail loading 'Getting Started' local URL. Showing feedback text."))
            noContent = true;
        }
    }


    //--------------------------------
    //
    // Behavior
    //
    //--------------------------------

    Component.onCompleted: {
        resetInternetUrl();
    }


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
            color: IngeScapeTheme.editorsBackgroundBorderColor
        }
        color: IngeScapeTheme.editorsBackgroundColor

        WebEngineView {
            id: webview
            anchors {
                top: parent.top
                left: parent.left
                right: parent.right
                bottom: showOnStartupCheckbox.top
                margins: 15
            }

            profile: WebEngineProfile {
                offTheRecord: true
            }

            visible: false

            onLoadingChanged: {
                if (loadRequest.status === WebEngineLoadRequest.LoadFailedStatus)
                { // Failed to load url
                    changeUrl(loadRequest.url.toString());
                }
                else if (loadRequest.status === WebEngineLoadRequest.LoadSucceededStatus)
                { // Succeed to load url
                    if (loadRequest.url.toString() === rootItem.gettingStartedRemoteUrl)
                    { // If remote request, check error code
                        if ((!loadRequest.errorCode) || ((loadRequest.errorCode) && (loadRequest.errorCode !== 200)))
                        { // Request sent error (only for remote url)
                            changeUrl(loadRequest.url.toString());
                        }
                        else
                        { // Success of the request
                            webview.visible = true;
                        }
                    }
                    else
                    { // If local request, succeed
                        webview.visible = true;
                    }

                }
            }
        }

        Text {
            id: loadingFailedFeedback

            anchors {
                top: parent.top
                left: parent.left
                right: parent.right
                bottom: showOnStartupCheckbox.top
                margins: 15
            }

            visible: noContent

            text: "Unable to load the 'Getting started' page.\nPlease try using a web browser to access <URL> directly." //FIXME Fallback URL to define
            verticalAlignment: Text.AlignVCenter
            horizontalAlignment: Text.AlignHCenter

            color: IngeScapeTheme.whiteColor
            wrapMode: Text.WordWrap
            font {
                family: IngeScapeTheme.textFontFamily
                pixelSize : 18
                italic: true
            }
        }

        CheckBox {
            id: showOnStartupCheckbox
            anchors {
                left: parent.left
                leftMargin: 15
                bottom: parent.bottom
                bottomMargin: 15
            }

            checked: IngeScapeEditorC.gettingStartedShowAtStartup

            onCheckedChanged: {
                IngeScapeEditorC.gettingStartedShowAtStartup = checked
            }

            activeFocusOnPress: true;

            style: CheckBoxStyle {
                label: Text {
                    anchors {
                        verticalCenter: parent.verticalCenter
                        verticalCenterOffset: 1
                    }

                    color: IngeScapeTheme.lightGreyColor

                    text: qsTr("Show this page at startup")

                    elide: Text.ElideRight

                    font {
                        family: IngeScapeTheme.textFontFamily
                        pixelSize: 16
                    }
                }
            }
        }
    }
}
