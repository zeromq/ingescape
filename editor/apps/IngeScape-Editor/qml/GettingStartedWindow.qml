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
import QtWebView 1.1

import I2Quick 1.0

import INGESCAPE 1.0


Window {
    id: rootItem

    title: "Getting started"

    property url gettingStartedInternetUrl: "https://ingescape.com/"
    property url gettingStartedLocalUrl: "file:///tmp/hello.html"

    height: minimumHeight
    width: minimumWidth

    minimumWidth: 1200
    minimumHeight: 880

    flags: Qt.Dialog


    //--------------------------------
    //
    // Content
    //
    //--------------------------------

    // Reset the URL to the Internet URL of the getting started page
    function resetInternetUrl()
    {
        webview.stop()
        webview.url = gettingStartedInternetUrl
    }

    // Reset the URL to the Internet URL of the getting started page
    function switchToLocalUrl()
    {
        webview.stop()
        webview.url = gettingStartedLocalUrl
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


        Button {
            id: btnCloseEditor

            anchors {
                verticalCenter: pageTitle.verticalCenter
                right : parent.right
                rightMargin: 20
            }

            activeFocusOnPress: true

            style: LabellessSvgButtonStyle {
                fileCache: IngeScapeTheme.svgFileIngeScape

                pressedID: releasedID + "-pressed"
                releasedID: "closeEditor"
                disabledID : releasedID
            }

            onClicked: {
                // Close our popup
                rootItem.close();
            }
        }

        Text {
            id: pageTitle

            anchors {
                top: parent.top
                topMargin: 15
                left: parent.left
                leftMargin: 18
            }

            text: webview.title === "" ? "Getting started" : webview.title
            elide: Text.ElideRight
            color: IngeScapeTheme.whiteColor

            font {
                family: IngeScapeTheme.textFontFamily
                pixelSize: 23
                weight: Font.Medium
            }
        }

        WebView {
            id: webview
            anchors {
                top: btnCloseEditor.bottom
                left: parent.left
                right: parent.right
                bottom: parent.bottom
                margins: 15
            }

            onLoadingChanged: {
                if ((loadRequest.url === rootItem.gettingStartedInternetUrl) && (loadRequest.status === 3))
                {
                    console.log(qsTr("Fail loading 'Getting Started' internet URL. Switching to local URL."))
                    rootItem.switchToLocalUrl()
                }
            }

        }
    }
}
