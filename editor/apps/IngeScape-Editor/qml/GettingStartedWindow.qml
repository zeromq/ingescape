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

    property string gettingStartedRemoteUrl: IngeScapeEditorC.gettingStartedRemoteUrl
    property string gettingStartedLocalUrl: IngeScapeEditorC.gettingStartedLocalUrl

    height: minimumHeight
    width: minimumWidth

    minimumWidth: 1200
    minimumHeight: 880

    flags: Qt.Dialog

    // Flag indicating if neither the Internet URL nor the locale URL could be loaded
    property bool loadingError: true

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
            webview.stop()
            webview.url = rootItem.gettingStartedRemoteUrl
        }
        else
        {
            rootItem.switchToLocalUrl()
        }

    }

    // Reset the URL to the Internet URL of the getting started page
    function switchToLocalUrl()
    {
        if (rootItem.gettingStartedLocalUrl !== "")
        {
            webview.stop()
            webview.url = rootItem.gettingStartedLocalUrl
        }
        else
        {
            rootItem.loadingError = true
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


        LabellessSvgButton {
            id: btnCloseEditor

            anchors {
                verticalCenter: pageTitle.verticalCenter
                right : parent.right
                rightMargin: 20
            }


            pressedID: releasedID + "-pressed"
            releasedID: "closeEditor"
            disabledID : releasedID


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
                bottom: showOnStartupCheckbox.top
                margins: 15
            }

            visible: !rootItem.loadingError

            onLoadingChanged: {
                if (loadRequest.status === 3)
                {
                    if (loadRequest.url.toString() === rootItem.gettingStartedRemoteUrl)
                    {
                        console.log(qsTr("Fail loading 'Getting Started' internet URL. Switching to local URL."))
                        rootItem.switchToLocalUrl()
                    }
                    else if (loadRequest.url.toString() === rootItem.gettingStartedLocalUrl)
                    {
                        console.log(qsTr("Fail loading 'Getting Started' local URL. Showing feedback text."))
                        rootItem.loadingError = true
                    }
                }
                else if (loadRequest.status == 2)
                {
                    rootItem.loadingError = false
                }

            }
        }

        Text {
            id: loadingFailedFeedback

            anchors {
                top: btnCloseEditor.bottom
                left: parent.left
                right: parent.right
                bottom: showOnStartupCheckbox.top
                margins: 15
            }

            visible: !webview.visible

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

                    text: "Show this page at startup"
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
