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
import "../theme" as Theme

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
    property int linesNumber : controller ? controller.linesNumberInTimeLine : 0;
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

    // Timeline Content
    Item {
        id: timeLineArea

        anchors {
            top: columnHeadersArea.bottom
            bottom: parent.bottom
            left: columnHeadersArea.left
            right: columnHeadersArea.right
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

                    delegate: Rectangle {
                        id : backgroundActionLine

                        width: parent.width
                        height : rootItem.lineHeight

                        color : MasticTheme.blackColor

                        // Lower separator
                        Rectangle {
                            anchors.left: parent.left
                            anchors.right: parent.right
                            anchors.bottom: parent.bottom

                            height: 1
                            color: MasticTheme.veryDarkGreyColor
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
            clip : true

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
                        x: viewController.convertTimeInMillisecondsToAbscissaInCoordinateSystem(model.timeInSeconds*1000, viewController.pixelsPerMinute)
                        y: 0

                        I2Line {
                            useSvgGeometry: true
                            x1: 0
                            x2: 0
                            y1: 0
                            y2: timeLinesContent.height
                            visible: model.isBigTick
                            stroke: MasticTheme.veryDarkGreyColor
                            strokeWidth: 1
                            strokeDashArray: "3, 3"
                        }
                    }
                }


                // Current time
                // NB: two items to avoid complex QML bindings that
                //     are interpreted by the Javascript stack
                Item {
                    id: currentTimeLine
                    x: viewController.convertQTimeToAbscissaInCoordinateSystem(controller.currentTime, viewController.pixelsPerMinute)
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


                    Repeater {
                        model : controller ? controller.actionsInTimeLine : 0;

                        Item {
                            id : actionVM
                            x : viewController.convertTimeInMillisecondsToAbscissaInCoordinateSystem(model.startTime, viewController.pixelsPerMinute)
                            y : rootItem.lineHeight * model.lineInTimeLine
                            height : rootItem.lineHeight
                            width : if (model.actionModel) {
                                        switch (model.actionModel.validityDurationType)
                                        {
                                        case ValidationDurationType.IMMEDIATE:
                                            0;
                                            break;
                                        case ValidationDurationType.FOREVER:
                                            (viewController.timeTicksTotalWidth - viewController.convertTimeInMillisecondsToAbscissaInCoordinateSystem(model.startTime, viewController.pixelsPerMinute))
                                            break;
                                        case ValidationDurationType.CUSTOM:
                                            viewController.convertDurationInSecondsToLengthInCoordinateSystem(model.actionModel.validityDuration/1000, viewController.pixelsPerMinute)
                                            break;
                                        default:
                                            0
                                            break;
                                        }
                                    }
                                    else {
                                        0;
                                    }

                            Rectangle {
                                anchors {
                                    top : parent.top
                                    left : parent.left
                                    right : parent.right
                                }

                                height : rootItem.lineHeight/2
                                color : MasticTheme.blueGreyColor2
                            }

                            Text {
                                anchors {
                                    top : parent.verticalCenter
                                    bottom : parent.bottom
                                    left : parent.left
                                    right : parent.right
                                }
                                verticalAlignment: Text.AlignVCenter
                                color : MasticTheme.darkGreyColor
                                text : model.actionModel ? model.actionModel.name : ""
                                font {
                                    family : MasticTheme.textFontFamily
                                    pixelSize: 12
                                }

                                Rectangle {
                                    anchors {
                                        fill : parent
                                        leftMargin:-1
                                        rightMargin:-1
                                    }
                                    color : MasticTheme.blackColor
                                }
                            }
                        }
                    }
                }
            }
        }
    }



    // Timeline Header
    Item {
        id: columnHeadersArea

        anchors {
            top: parent.top
            topMargin: 32
            left: parent.left
            leftMargin: 105
            right: parent.right
            rightMargin: 35
        }

        height: 30
        clip : true

        // Time ticks and current time label
        Flickable {
            id: columnHeaders

            anchors.fill: parent
            interactive: false

            contentX: contentArea.contentX

            contentWidth: viewController.timeTicksTotalWidth
            contentHeight: columnHeadersArea.height

            Item {
                id: columnHeadersContent

                width: viewController.timeTicksTotalWidth
                height: columnHeadersArea.height

                //
                // Time ticks
                //
                Repeater {
                    model:  viewController.timeTicks

                    delegate: Item {
                        x: viewController.convertTimeInMillisecondsToAbscissaInCoordinateSystem(model.timeInSeconds*1000, viewController.pixelsPerMinute)
                        anchors {
                            top : columnHeadersContent.top
                            bottom : columnHeadersContent.bottom
                        }

                        Rectangle {
                            id : timeticks
                            anchors {
                                horizontalCenter: parent.left
                                bottom : parent.bottom
                                bottomMargin: 1
                            }
                            height : (model.isBigTick) ? 10 : 4
                            width : 1
                            color: MasticTheme.darkGreyColor
                        }

                        Text {
                            anchors {
                                horizontalCenter: timeticks.horizontalCenter
                                bottom : timeticks.top
                            }
                            visible : (model.isBigTick)
                            horizontalAlignment: Text.AlignHCenter

                            text: model.label

                            font.pixelSize: 12
                            font.family: MasticTheme.textFontFamily

                            color: MasticTheme.darkGreyColor
                        }
                    }
                }


            }
        }

    }


    // Timeline ScrollBar
    Rectangle {
        id: scrollTimeLine

        anchors {
            top: parent.top
            topMargin: 19
            left: columnHeadersArea.left
            right: columnHeadersArea.right
        }

        height: 13
        color : MasticTheme.blackColor

        Rectangle {
            id : scrollBar
            anchors {
                verticalCenter: parent.verticalCenter
            }
            height : 13

            property var scrollBarSize: if (viewController) {
                                            (viewController.viewportWidth*scrollTimeLine.width)/viewController.timeTicksTotalWidth
                                        } else {
                                            0
                                        }
            width : scrollBarSize
            color:  mouseArea.containsPress? MasticTheme.veryDarkGreyColor : MasticTheme.darkGreyColor; //mouseArea.containsPress? MasticTheme.veryDarkGreyColor : MasticTheme.darkGreyColor;
            border {
                color : MasticTheme.blackColor;
                width : 3
            }

            MouseArea {
                id: mouseArea

                anchors.fill: scrollBar

                hoverEnabled: true

                drag.smoothed: false
                drag.target: scrollBar

                drag.minimumX : 0
                drag.maximumX : scrollTimeLine.width - scrollBar.width
                drag.minimumY : 0
                drag.maximumY :  0
            }
        }
    }



    // Play Button
    Item {
        anchors {
            left : parent.left
            right : columnHeadersArea.left
            top : parent.top
            topMargin: 16
        }

        Button {
            id: playScenarioBtn

            anchors {
                top: parent.top
                horizontalCenter: parent.horizontalCenter
            }

            activeFocusOnPress: true
            checkable: true

            style: Theme.LabellessSvgButtonStyle {
                fileCache: MasticTheme.svgFileMASTIC

                pressedID: releasedID + "-pressed"
                releasedID: (controller && controller.isPlaying)? "pause" : "play"
                disabledID : releasedID
            }

            onClicked: {
                if (controller) {
                    controller.isPlaying = checked;
                }
            }

            Binding {
                target : playScenarioBtn
                property : "checked"
                value : controller? controller.isPlaying : false
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

    // ContentArea => ViewController
    Binding {
        target: viewController
        property: "viewportY"
        value: contentArea.contentY
    }

    // ContentArea => ViewController
    Binding {
        target: viewController
        property: "viewportWidth"
        value: timeLineArea.width
    }

    // ContentArea => ViewController
    Binding {
        target: viewController
        property: "viewportHeight"
        value: timeLineArea.height
    }

    // Scrollbar => contentArea
    Binding {
        target: contentArea
        property: "contentX"
        value: if (viewController) {
                   (scrollBar.x * viewController.timeTicksTotalWidth)/scrollTimeLine.width
               } else {
                   0
               }
        when: mouseArea.drag.active
    }

    // viewController => Scrollbar
    Binding {
        target: scrollBar
        property: "x"
        value: if (viewController) {
                   (viewController.viewportX*scrollTimeLine.width)/viewController.timeTicksTotalWidth
               } else {
                   0
               }
        when: !mouseArea.drag.active
    }


}
