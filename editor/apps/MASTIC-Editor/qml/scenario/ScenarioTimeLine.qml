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
 *      Alexandre Lemort   <lemort@ingenuity.io>
 *      Justine Limoges   <limoges@ingenuity.io>
 */

import QtQuick 2.8
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4

import I2Quick 1.0

import MASTIC 1.0


Item {
    id: rootItem

    anchors.fill: parent

    //--------------------------------
    //
    // Properties
    //
    //--------------------------------

    // Controller associated to our view
    property var controller : null;

    property var viewController : MasticEditorC.timeLineC;


    // Minimum scale factor
    readonly property real minimumScale: 0.25;

    // Maximum scale factor
    readonly property real maximumScale: 4;

    // Duration of automatic pan and/or zoom animations in milliseconds
    readonly property int automaticPanZoomAnimationDuration: 300;

    // Zoom-in delta scale factor
    readonly property real zoomInDeltaScaleFactor: 1.2



    // graphical properties
    property int linesNumber : 15
    property int lineHeight : 30

    //--------------------------------
    //
    // Functions
    //
    //--------------------------------




    //--------------------------------
    //
    // Content
    //
    //--------------------------------

    Item {
        id: timeLineArea

        anchors {
            top: columnHeadersArea.bottom
            bottom: parent.bottom
            left: parent.left
            right: parent.right
        }


        //
        // Background for each line
        //
        Flickable {
            id: backgroundLines
            anchors.fill: parent

            interactive: false
            clip : true
            contentY: contentArea.contentY

            contentWidth: timeLineArea.width
            contentHeight: rootItem.lineHeight * rootItem.linesNumber


            Column {
                width: timeLineArea.width
                height: rootItem.lineHeight * rootItem.linesNumber

                //
                // A background for each action line
                //
                Repeater {
                    model: rootItem.linesNumber

                    delegate: Item {
                        id : backgroundActionLine

                        width: parent.width
                        height : rootItem.lineHeight

                        // Upper separator
                        Rectangle {
                            anchors.left: parent.left
                            anchors.right: parent.right
                            anchors.top: parent.top

                            height: 1
                            color: MasticTheme.lightGreyColor
                        }


                        // Lower separator
                        Rectangle {
                            anchors.left: parent.left
                            anchors.right: parent.right
                            anchors.bottom: parent.bottom

                            height: 1
                            color: MasticTheme.greyColor
                        }
                    }
                }
            }
        }


        //
        // Time ticks and current timeline
        //
        Flickable {
            id: timeLines

            anchors.fill: parent

            interactive: false
            contentX: contentArea.contentX

            contentWidth: viewController.timeTicksTotalWidth
            contentHeight: timeLineArea.height

            Item {
                id: timeLinesContent

                width: viewController.timeTicksTotalWidth
                height: timeLineArea.height


                // Time ticks
                Repeater {
                    model: viewController.timeTicks

                    // NB: two items to avoid complex QML bindings that
                    //     are interpreted by the Javascript stack
                    delegate : Item {
                        x: viewController.convertTimeToAbscissaInCoordinateSystem(model.timeInSeconds, viewController.pixelsPerMinute)
                        y: 0

                        I2Line {
                            useSvgGeometry: true

                            x1: -1
                            x2: -1
                            y1: 0
                            y2: timeLinesContent.height

                            stroke: MasticTheme.lightGreyColor
                            strokeWidth: 2
                            strokeDashArray: "7, 3"
                        }
                    }
                }


                // Current time
                // NB: two items to avoid complex QML bindings that
                //     are interpreted by the Javascript stack
                Item {
                    id: currentTimeLine
                    x: 0 //CregController.analysisViewController.convertDateTimeToAbscissaInCoordinateSystem(root.currentDateTime, CregController.analysisViewController.pixelsPerMinute)
                    y: 0

                    Rectangle {
                        x: -1
                        y: 0

                        width: 2
                        height: timeLinesContent.height

                        color: MasticTheme.whiteColor
                    }
                }

            }
        }



        //
        // Content associated to our train stations ("passages desservis", "emcombrements quai", "manoeuvres", etc.)
        //
        // NB: Flickable is inside PinchArea to avoid input issues on Linux
        //     i.e. sometimes PinchArea does not detect two fingers
        //          only one finger is detected and handled by our Flickable
        //
        // The FlickResize example (http://doc.qt.io/qt-5/qtquick-touchinteraction-example.html)
        // does not work as expected on Linux
        PinchArea {
            id : pinchAreaOfContentArea

            anchors.fill: parent
            enabled: true;

            onPinchStarted: {
                // Disable our flickable to avoid issues while we're pinching
                // i.e. we want to avoid that our flickable somehow figures out
                // that it should flick if we move our touch points too much
                contentArea.interactive = false;
            }

            onPinchUpdated: {
                // Check if we have at least two points
                if (pinch.pointCount >= 2 && viewController)
                {
                    // Compute delta between our new scale factor and the previous one
                    var deltaScale = pinch.scale/pinch.previousScale;

                    // Check bounds of our delta scale
                    var previousPixelsPerMinute = viewController.pixelsPerMinute;
                    var newPixelsPerMinute = previousPixelsPerMinute * deltaScale;

                    if (newPixelsPerMinute < viewController.minPixelsPerMinute)
                    {
                        newPixelsPerMinute = viewController.minPixelsPerMinute;
                        deltaScale = newPixelsPerMinute/previousPixelsPerMinute;
                    }
                    else if (newPixelsPerMinute > viewController.maxPixelsPerMinute)
                    {
                        newPixelsPerMinute = viewController.maxPixelsPerMinute;
                        deltaScale = newPixelsPerMinute/previousPixelsPerMinute;
                    }

                    // Resize content
                    contentArea.resizeContent(
                                contentArea.contentWidth * deltaScale,
                                contentArea.contentHeight,
                                Qt.point(contentArea.contentX + contentArea.width/2, contentArea.contentY + contentArea.height/2)
                                );

                    // Update current time unit
                    viewController.pixelsPerMinute *= deltaScale;
                }
            }

            onPinchFinished: {
                // Re-enable our flickable
                contentArea.interactive = true;

                // Move content of our flickable within bounds
                contentArea.returnToBounds();
            }


            Flickable {
                id: contentArea

                anchors.fill: parent

                contentWidth: content.width
                contentHeight: content.height

                boundsBehavior: Flickable.OvershootBounds;

                //
                // Dynamic content of our view
                //
                Item {
                    id: content
                    width: viewController.timeTicksTotalWidth
                    height: rootItem.lineHeight * rootItem.linesNumber


                }
            }
        }
    }



    Rectangle {
        id: columnHeadersArea

        anchors.top: parent.top
        anchors.left: parent.left
        anchors.right: parent.right

        height: 35

        color: "#2D2D2D"

        // Separator
        Rectangle {
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.bottom: parent.bottom

            height: 2
            color:  MasticTheme.whiteColor
        }

        // Time ticks and current time label
        Flickable {
            id: columnHeaders

            anchors.fill: parent
            interactive: false

            contentX: contentArea.contentX

            contentWidth: viewController.timeTicksTotalWidth
            contentHeight: 35

            Item {
                id: columnHeadersContent

                width: viewController.timeTicksTotalWidth
                height: 35


                //
                // Time ticks
                //
                Repeater {
                    model:  viewController.timeTicks

                    delegate: Text {
                        x: viewController.convertTimeToAbscissaInCoordinateSystem(model.timeInSeconds, viewController.pixelsPerMinute)
                        anchors.verticalCenter: columnHeadersContent.verticalCenter
                         //((model.isBigTick) ? 25 : ((model.isSmallTick) ? 33 : 29))

                        width: 1
                        horizontalAlignment: Text.AlignHCenter

                        text: model.label

                        font.pixelSize: ((model.isBigTick) ? 20 : ((model.isSmallTick) ? 12 : 16))
                        font.family: MasticTheme.textFontFamily

                        color: MasticTheme.whiteColor
                    }
                }


            }
        }

    }


    //--------------------------------------------------------
    //
    //
    // Analysis view - Connections / Bindings
    //
    //
    //--------------------------------------------------------

    // ContentArea => AnalysisViewController
    Binding {
        target: viewController
        property: "viewportX"
        value: contentArea.contentX
    }

    // ContentArea => AnalysisViewController
    Binding {
        target: viewController
        property: "viewportY"
        value: contentArea.contentY
    }

    // ContentArea => AnalysisViewController
    Binding {
        target: viewController
        property: "viewportWidth"
        value: timeLineArea.width
    }

    // ContentArea => AnalysisViewController
    Binding {
        target: viewController
        property: "viewportHeight"
        value: timeLineArea.height
    }


}
