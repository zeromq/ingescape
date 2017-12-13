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


    // graphical properties
    property int linesNumber : controller ? controller.linesNumberInTimeLine : 0;
    property int lineHeight : MasticTheme.lineInTimeLineHeight

    //--------------------------------
    //
    // Functions
    //
    //--------------------------------

    function updateZoomOfTimeLine(deltaScale, centerPointX, centerPointY) {
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
                    Qt.point(centerPointX, centerPointY)
                    //  Qt.point(contentArea.contentX + contentArea.width/2, contentArea.contentY + contentArea.height/2)
                    );

        // Update current time unit
        viewController.pixelsPerMinute *= deltaScale;
    }


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
        // Time ticks
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

            }
        }




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

                    rootItem.updateZoomOfTimeLine (deltaScale, contentArea.contentX + contentArea.width/2, contentArea.contentY + contentArea.height/2);
                }
            }

            onPinchFinished: {
                // Re-enable our flickable
                //contentArea.interactive = true;

                // Move content of our flickable within bounds
                contentArea.returnToBounds();
            }



            //
            // MouseArea to capture scroll gesture events (trackpad)
            //
            MouseArea {
                anchors.fill: parent

                scrollGestureEnabled: true

                onPressed: {
                    rootItem.forceActiveFocus();
                }

                onWheel: {
                    wheel.accepted = true;

                    if ((wheel.pixelDelta.x !== 0) || (wheel.pixelDelta.y !== 0))
                    {
                        //
                        // Trackpad flick gesture => scroll our workspace
                        //
                        var maxXOfTimeline = contentArea.contentWidth - contentArea.width;
                        var maxYOfTimeline = contentArea.contentHeight - contentArea.height;

                        if (maxXOfTimeline > 0) {
                            if ((contentArea.contentX + wheel.pixelDelta.x >= 0)
                                    && (contentArea.contentX + wheel.pixelDelta.x <= maxXOfTimeline)) {
                                contentArea.contentX += wheel.pixelDelta.x;
                            } else if (contentArea.contentX + wheel.pixelDelta.x < 0) {
                                contentArea.contentX = 0;
                            } else if (contentArea.contentX + wheel.pixelDelta.x > maxXOfTimeline) {
                                contentArea.contentX = maxXOfTimeline;
                            }
                        }

                        if (maxYOfTimeline > 0) {
                            if ((contentArea.contentY + wheel.pixelDelta.y >= 0)
                                    && (contentArea.contentY + wheel.pixelDelta.y <= maxYOfTimeline)) {
                                contentArea.contentY += wheel.pixelDelta.y;
                            } else if (contentArea.contentY + wheel.pixelDelta.y < 0) {
                                contentArea.contentY = 0;
                            } else if (contentArea.contentY + wheel.pixelDelta.y > maxYOfTimeline) {
                                contentArea.contentY = maxYOfTimeline;
                            }
                        }


                    }
                }

                //
                // MouseArea used to drag-n-drop our workspace AND handle real mouse wheel events (zoom-in, zoom-out)
                //
                MouseArea {
                    id: mouseAreaWorkspaceDragNDropAndWheelZoom

                    anchors.fill: parent

                    // 2-finger-flick gesture should pass through to our parent MouseArea
                    scrollGestureEnabled: false

                    onPressed: {
                        rootItem.forceActiveFocus();
                    }

                    onPositionChanged: {
                        if (pressed) {

                        }
                    }


                    onWheel: {
                        wheel.accepted = true;

                        // with ctrl => zoom In and zoom out
                        if (wheel.modifiers && Qt.ControlModifier) {
                            var previousPixelsPerMinute = viewController.pixelsPerMinute;
                            var deltaScale;

                            // Check if we must zoom-in or zoom-out
                            if (wheel.angleDelta.y < 0)
                            {

                                // Compute delta scale according to wheel.angleDelta
                                deltaScale = Math.pow(1/1.2, Math.abs(wheel.angleDelta.y)/120) ;

                                rootItem.updateZoomOfTimeLine (deltaScale, contentArea.contentX + wheel.x,  contentArea.contentY + contentArea.height/2);
                            }
                            else if (wheel.angleDelta.y > 0)
                            {

                                // Compute delta scale according to wheel.angleDelta
                                deltaScale = Math.pow(1.2, Math.abs(wheel.angleDelta.y)/120) ;

                                rootItem.updateZoomOfTimeLine (deltaScale, contentArea.contentX + wheel.x,  contentArea.contentY + contentArea.height/2);
                            }
                            // Else: wheel.angleDelta.y  == 0  => invalid wheel event
                        }

                        // else navigation in vertical
                        else {
                            var yMaxOfTimeLine = rootItem.lineHeight * rootItem.linesNumber - viewController.viewportHeight;
                            var nbCranMolette = wheel.angleDelta.y/120.0;
                            contentArea.contentY = Math.max(0, Math.min(contentArea.contentY - nbCranMolette * 100, yMaxOfTimeLine));
                        }
                    }


                    Flickable {
                        id: contentArea

                        anchors.fill: parent
                        clip : true
                        contentWidth: content.width
                        contentHeight: content.height
                        interactive: false

                        boundsBehavior: Flickable.StopAtBounds;

                        //
                        // Dynamic content of our view
                        //
                        Item {
                            id: content
                            width: viewController.timeTicksTotalWidth
                            height: rootItem.lineHeight * rootItem.linesNumber

                            MouseArea {
                                anchors.fill: parent
                                onClicked: {
                                    // deselect action in timeline
                                    if (controller && controller.selectedActionVMInTimeline) {
                                        controller.selectedActionVMInTimeline = null;

                                    }
                                }
                            }

                            Repeater {
                                model : controller ? controller.actionsInTimeLine : 0;

                                ActionInTimeLine {
                                    myActionVM : model.QtObject;
                                    controller : rootItem.controller
                                }
                            }

                            // dropArea allow dropping actions in timeline
                            DropArea {
                                anchors.fill: parent
                                keys: ["ActionsListItem", "ActionInTimeLine"]

                                onEntered: {
                                    var dragItem = drag.source;
                                    // display ghost
                                    if (typeof dragItem.action !== 'undefined') {
                                        ghostAction.actionModelGhost = dragItem.action;
                                    }
                                }

                                onPositionChanged: {
                                    var dragItem = drag.source;

                                    var startInQTime = viewController.convertAbscissaInCoordinateSystemToQTime(drag.x, viewController.pixelsPerMinute)
                                    var starttimeInMilliseconds = viewController.convertAbscissaInCoordinateSystemToTimeInMilliseconds(drag.x, viewController.pixelsPerMinute)

                                    var lineNumber = Math.floor(drag.y / rootItem.lineHeight)
                                    var canInsertActionVM = false;

                                    // action comes from the actions list
                                    if (controller && (typeof dragItem.action !== 'undefined')) {
                                        // test if the drop is possible
                                        canInsertActionVM = controller.canInsertActionVMTo(dragItem.action, starttimeInMilliseconds, lineNumber)

                                        if (canInsertActionVM) {
                                            ghostDropImpossible.visible = false;
                                            // move ghost
                                            ghostAction.actionModelGhost = dragItem.action;
                                            ghostAction.x = viewController.convertQTimeToAbscissaInCoordinateSystem(startInQTime, viewController.pixelsPerMinute);
                                            ghostAction.y = lineNumber * rootItem.lineHeight;
                                            ghostAction.startTime = startInQTime;
                                            if  (typeof dragItem.temporaryStartTime !== 'undefined') {
                                                dragItem.temporaryStartTime = startInQTime;
                                            }
                                        }
                                        else {
                                            // remove ghost
                                            ghostAction.actionModelGhost = null;

                                            // ghost drop impossible
                                            ghostDropImpossible.x = viewController.convertQTimeToAbscissaInCoordinateSystem(startInQTime, viewController.pixelsPerMinute);
                                            ghostDropImpossible.y = lineNumber * rootItem.lineHeight + (rootItem.lineHeight/2 - ghostDropImpossible.width/2);
                                            ghostDropImpossible.visible = true;
                                        }

                                    }

                                    // action comes from the timeline
                                    if (controller && (typeof dragItem.myActionVM !== 'undefined' && dragItem.myActionVM.modelM !== null)) {
                                        // test if the drop is possible
                                        canInsertActionVM = controller.canInsertActionVMTo(dragItem.myActionVM.modelM, starttimeInMilliseconds, lineNumber, dragItem.myActionVM)

                                        if (canInsertActionVM) {
                                            ghostDropImpossible.visible = false;
                                            // move ghost
                                            ghostAction.actionModelGhost = dragItem.myActionVM.modelM;
                                            ghostAction.x = viewController.convertQTimeToAbscissaInCoordinateSystem(startInQTime, viewController.pixelsPerMinute);
                                            ghostAction.y = lineNumber * rootItem.lineHeight;
                                            ghostAction.startTime = startInQTime;
                                            if  (typeof dragItem.temporaryStartTime !== 'undefined') {
                                                dragItem.temporaryStartTime = startInQTime;
                                            }
                                        }
                                        else {
                                            // remove ghost
                                            ghostAction.actionModelGhost = null;

                                            // ghost drop impossible
                                            ghostDropImpossible.x = viewController.convertQTimeToAbscissaInCoordinateSystem(startInQTime, viewController.pixelsPerMinute);
                                            ghostDropImpossible.y = lineNumber * rootItem.lineHeight + (rootItem.lineHeight/2 - ghostDropImpossible.width/2);
                                            ghostDropImpossible.visible = true;
                                        }

                                    }



                                }

                                onExited: {
                                    // remove ghost
                                    ghostAction.actionModelGhost = null;
                                    ghostDropImpossible.visible = false;
                                    var dragItem = drag.source;
                                    if  (typeof dragItem.temporaryStartTime !== 'undefined') {
                                        dragItem.temporaryStartTime = null;
                                    }
                                }

                                onDropped: {
                                    var dragItem = drag.source;
                                    var timeInMilliseconds = viewController.convertAbscissaInCoordinateSystemToTimeInMilliseconds(drag.x, viewController.pixelsPerMinute)
                                    var lineNumber = Math.floor(drag.y / rootItem.lineHeight)

                                    // action comes from the actions list : add the action on the time line
                                    if (typeof dragItem.action !== 'undefined' && controller)
                                    {
                                        controller.addActionVMAtTime(dragItem.action, timeInMilliseconds, lineNumber);
                                    }


                                    // action comes from the timeline : update start time and line number
                                    if (typeof dragItem.myActionVM !== 'undefined' && dragItem.myActionVM.modelM !== null)
                                    {
                                         controller.moveActionVMAtTimeAndLine(dragItem.myActionVM, timeInMilliseconds, lineNumber);
                                    }


                                    // remove ghost
                                    ghostAction.actionModelGhost = null;
                                    ghostDropImpossible.visible = false;
                                    if  (typeof dragItem.temporaryStartTime !== 'undefined') {
                                        dragItem.temporaryStartTime = null;
                                    }

                                }
                            }


                            // Ghost Drop Impossible
                            I2SvgItem {
                                id : ghostDropImpossible
                                visible : false
                                height : 15
                                width : height
                                opacity : 0.8

                                svgFileCache: MasticTheme.svgFileMASTIC
                                svgElementId: "dropImpossible"
                            }

                            // Ghost Action
                            Item {
                                id : ghostAction
                                x: 0
                                y : 0

                                property var actionModelGhost : null;
                                property var startTime : null;
                                opacity : (actionModelGhost !== null) ? 0.5 : 0

                                height : rootItem.lineHeight/2
                                width : if (actionModelGhost) {
                                            switch (actionModelGhost.validityDurationType)
                                            {
                                            case ValidationDurationType.IMMEDIATE:
                                                0;
                                                break;
                                            case ValidationDurationType.FOREVER:
                                                (viewController.timeTicksTotalWidth - viewController.convertQTimeToAbscissaInCoordinateSystem(startTime, viewController.pixelsPerMinute))
                                                break;
                                            case ValidationDurationType.CUSTOM:
                                                viewController.convertDurationInMillisecondsToLengthInCoordinateSystem(actionModelGhost.validityDuration, viewController.pixelsPerMinute)
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
                                    id : rect
                                    anchors {
                                        top : parent.top
                                        left : parent.left
                                        right : parent.right
                                    }

                                    height : rootItem.lineHeight/2
                                    color : MasticTheme.blueGreyColor2
                                }

                                // Not revert action
                                I2SvgItem {
                                    x : - width /2.0;
                                    anchors {
                                        verticalCenter: parent.verticalCenter
                                    }

                                    visible : ghostAction.actionModelGhost && !ghostAction.actionModelGhost.shallRevert

                                    svgFileCache : MasticTheme.svgFileMASTIC;
                                    svgElementId: "timelineAction";
                                }

                                // Revert action
                                Item {
                                    visible : ghostAction.actionModelGhost && ghostAction.actionModelGhost.shallRevert
                                    height : childrenRect.height
                                    anchors {
                                        verticalCenter: parent.verticalCenter
                                    }

                                    Rectangle {
                                        anchors {
                                            verticalCenter: actionExecutionGhost.verticalCenter
                                            left : actionExecutionGhost.horizontalCenter
                                            right : revertActionExecutionGhost.horizontalCenter
                                        }
                                        height : 1
                                        color : MasticTheme.lightGreyColor;

                                    }

                                    I2SvgItem {
                                        id : actionExecutionGhost
                                        x : 0;
                                        y : 0

                                        svgFileCache : MasticTheme.svgFileMASTIC;
                                        svgElementId: "revertAction"
                                    }

                                    I2SvgItem {
                                        id : revertActionExecutionGhost
                                        x : if (ghostAction.actionModelGhost) {
                                                if (ghostAction.actionModelGhost.shallRevertWhenValidityIsOver)
                                                {
                                                    rect.width - width;
                                                }
                                                else if (ghostAction.actionModelGhost.shallRevertAfterTime) {
                                                    viewController.convertDurationInMillisecondsToLengthInCoordinateSystem(ghostAction.actionModelGhost.revertAfterTime, viewController.pixelsPerMinute) - width;
                                                }
                                                else {
                                                    0;
                                                }
                                            }
                                            else {
                                                0;
                                            }

                                        y : 0
                                        rotation : 180
                                        svgFileCache : MasticTheme.svgFileMASTIC;
                                        svgElementId:"revertAction" ;
                                    }
                                }
                            }

                        }
                    }
                }


            }

        }



        //            ScrollView {
        //                id : contentAreaScrollView
        //                anchors.fill: parent
        //                horizontalScrollBarPolicy : Qt.ScrollBarAlwaysOff

        //                style: MasticScrollViewStyle {
        //                }

        //                // Prevent drag overshoot on Windows
        //                flickableItem.boundsBehavior: Flickable.OvershootBounds


        //      }


        //
        // Current Time
        //
        Flickable {
            id: currenttimeLine

            anchors.fill: parent

            interactive: false
            contentX: contentArea.contentX

            contentWidth: viewController.timeTicksTotalWidth
            contentHeight: timeLineArea.height
            clip : true

            Item {
                id: currentTimeContent

                width: viewController.timeTicksTotalWidth
                height: timeLineArea.height


                // Current time
                // NB: two items to avoid complex QML bindings that
                //     are interpreted by the Javascript stack
                Item {
                    id: currentTimeLine
                    x: currentTimeItem.x
                    y: 0

                    Rectangle {
                        x: 0
                        y: 0

                        width: 1
                        height: timeLinesContent.height

                        color: MasticTheme.whiteColor
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
            topMargin: 26
            left: parent.left
            leftMargin: 105
            right: parent.right
            rightMargin: 35
        }

        height: 40
        clip : true


        // Time ticks and current time label
        Flickable {
            id: columnHeaders

            anchors.fill: parent

            contentWidth: viewController.timeTicksTotalWidth
            contentHeight: columnHeadersArea.height
            boundsBehavior: Flickable.StopAtBounds;

            //
            // MouseArea to capture scroll gesture events (trackpad)
            //
            MouseArea {
                anchors.fill: parent

                scrollGestureEnabled: true

                onPressed: {
                    rootItem.forceActiveFocus();
                }

                onWheel: {
                }

                //
                // MouseArea used to drag-n-drop our workspace AND handle real mouse wheel events (zoom-in, zoom-out)
                //
                MouseArea {
                    anchors.fill: parent

                    // drag.target: workspace

                    // 2-finger-flick gesture should pass through to our parent MouseArea
                    scrollGestureEnabled: false

                    onPressed: {
                        rootItem.forceActiveFocus();
                    }


                    onWheel: {
                        wheel.accepted = true;

                        // with ctrl => zoom In and zoom out
                        if (wheel.modifiers && Qt.ControlModifier) {

                            var previousPixelsPerMinute = viewController.pixelsPerMinute;
                            var deltaScale;

                            // Check if we must zoom-in or zoom-out
                            if (wheel.angleDelta.y < 0)
                            {

                                // Compute delta scale according to wheel.angleDelta
                                deltaScale = Math.pow(1/1.2, Math.abs(wheel.angleDelta.y)/120) ;

                                rootItem.updateZoomOfTimeLine (deltaScale,  wheel.x,  contentArea.contentY + contentArea.height/2);
                            }
                            else if (wheel.angleDelta.y > 0)
                            {

                                // Compute delta scale according to wheel.angleDelta
                                deltaScale = Math.pow(1.2, Math.abs(wheel.angleDelta.y)/120) ;

                                rootItem.updateZoomOfTimeLine (deltaScale, wheel.x,  contentArea.contentY + contentArea.height/2);
                            }
                            // Else: wheel.angleDelta.y  == 0  => invalid wheel event
                        }

                        // else navigation along timeline
                        else {
                            var xMaxOfTimeLine = viewController.timeTicksTotalWidth - viewController.viewportWidth;
                            var nbCranMolette = wheel.angleDelta.y/120.0;
                            contentArea.contentX = Math.max(0, Math.min(contentArea.contentX - nbCranMolette * 100, xMaxOfTimeLine));
                        }
                    }

                }

            }

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
                            }
                            height : (model.isBigTick) ? 8 : 4
                            width : 1
                            color: MasticTheme.darkGreyColor
                        }

                        Text {
                            anchors {
                                horizontalCenter: timeticks.horizontalCenter
                                bottom : timeticks.top
                                bottomMargin: 1
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


                //
                // Current time
                //
                Item {
                    id: currentTimeItem

                    anchors {
                        bottom : columnHeadersContent.bottom
                    }

                    Rectangle {
                        id: currentTimeLabel

                        anchors {
                            horizontalCenter : svgCurrentTime.horizontalCenter
                            bottom : svgCurrentTime.top
                            bottomMargin: -1
                        }
                        width : 80
                        height: 20
                        radius : 2
                        color :  currentTimeMouseArea.pressed ? MasticTheme.darkBlueGreyColor : MasticTheme.blueGreyColor2
                        border {
                            width : 1
                            color: currentTimeMouseArea.pressed ? MasticTheme.lightGreyColor : MasticTheme.whiteColor
                        }

                        Text {
                            id : currentTimeText
                            anchors {
                                centerIn : parent
                                verticalCenterOffset: 1
                            }

                            text : controller ? controller.currentTime.toLocaleTimeString(Qt.locale(), "HH':'mm':'ss':'zzz") : "00:00:00.000"
                            color: MasticTheme.lightGreyColor
                            font {
                                family: MasticTheme.textFontFamily
                                pixelSize: 14
                            }

                        }
                    }

                    I2SvgItem {
                        id :svgCurrentTime
                        anchors {
                            horizontalCenter : parent.left
                            bottom : parent.bottom
                        }

                        svgFileCache : MasticTheme.svgFileMASTIC;
                        svgElementId: "currentTime"
                    }


                    MouseArea {
                        id: currentTimeMouseArea

                        anchors {
                            top : currentTimeLabel.top
                            left: currentTimeLabel.left
                            right: currentTimeLabel.right
                            bottom : parent.bottom
                        }

                        hoverEnabled: true

                        drag.smoothed: false
                        drag.target: currentTimeItem

                        drag.minimumX : viewController.timeMarginInPixels
                        drag.maximumX : viewController.timeTicksTotalWidth - viewController.timeMarginInPixels
                        drag.minimumY : 0
                        drag.maximumY : 0

                        // viewController => CurrentTimeItem
                        Binding {
                            target: controller
                            property: "currentTime"
                            value: if (viewController) {
                                       viewController.convertAbscissaInCoordinateSystemToQTime(currentTimeItem.x, viewController.pixelsPerMinute)
                                   }
                                   else {
                                       0
                                   }
                            when: currentTimeMouseArea.drag.active
                        }

                        // CurrentTimeItem => Scrollbar
                        Binding {
                            target: currentTimeItem
                            property: "x"
                            value: if (controller && viewController)
                                   {
                                       viewController.convertQTimeToAbscissaInCoordinateSystem(controller.currentTime, viewController.pixelsPerMinute)
                                   }
                                   else {
                                       0
                                   }
                            when: !currentTimeMouseArea.drag.active
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
            topMargin: 13
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
                                        }
                                        else {
                                            0
                                        }
            width : scrollBarSize
            color:  mouseArea.containsPress? MasticTheme.veryDarkGreyColor : MasticTheme.darkGreyColor;
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


        Rectangle {
            anchors {
                left : playScenarioBtn.left
                leftMargin: -10
                right : playScenarioBtn.right
                rightMargin: -10
                top : playScenarioBtn.bottom
                topMargin: 6
            }
            height: 21
            radius : 2
            color :  MasticTheme.blueGreyColor2
            border {
                width : 1
                color: MasticTheme.whiteColor
            }

            Text {
                id : currenTime
                anchors {
                    centerIn : parent
                    verticalCenterOffset: 1
                }

                text : currentTimeText.text
                color: MasticTheme.lightGreyColor
                font {
                    family: MasticTheme.textFontFamily
                    pixelSize: 14
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

    // columnHeaders => contentArea
    Binding {
        target: columnHeaders
        property: "contentX"
        value: contentArea.contentX
    }

    // contentArea => columnHeaders
    Binding {
        target: contentArea
        property: "contentX"
        value: columnHeaders.contentX
    }





    // Scrollbar => contentArea
    Binding {
        target: contentArea
        property: "contentX"
        value: if (viewController) {
                   (scrollBar.x * viewController.timeTicksTotalWidth)/scrollTimeLine.width
               }
               else {
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
               }
               else {
                   0
               }
        when: !mouseArea.drag.active
    }


}
