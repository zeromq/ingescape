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
 *      Justine Limoges   <limoges@ingenuity.io>
 */

import QtQuick 2.8
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4

import I2Quick 1.0

import INGESCAPE 1.0
import "../theme" as Theme

Item {
    id: rootItem

    //--------------------------------
    //
    // Properties
    //
    //--------------------------------

    // Controller associated to our view
    property var controller : null;

    property var viewController : IngeScapeEditorC.timeLineC;


    // graphical properties
    property int linesNumber : controller ? controller.linesNumberInTimeLine : 0;
    property int lineHeight : IngeScapeTheme.lineInTimeLineHeight

    // flag indicating if our component is reduced or expanded
    property bool isReduced : true;

    onIsReducedChanged : {
        // Allow resize animations
        rootItem._canPerformResizeAnimations = true;

        // Set our new height
        if (isReduced)
        {
            rootItem.height = 0;
        }
        else
        {
            rootItem.height = IngeScapeTheme.bottomPanelHeight;
        }
    }



    // Flag used to check if we can perform resize animations
    // NB: this flag is used to avoid animations during a drag-n-drop
    property bool _canPerformResizeAnimations: true

    Behavior on height {
        enabled: rootItem._canPerformResizeAnimations

        NumberAnimation {}
    }


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

    function dragViewWithDelta (deltaX, deltaY) {

        var maxXOfTimeline = contentArea.contentWidth - contentArea.width;
        var maxYOfTimeline = contentArea.contentHeight - contentArea.height;

        if (maxXOfTimeline > 0) {
            if ((contentArea.contentX + deltaX >= 0)
                    && (contentArea.contentX + deltaX <= maxXOfTimeline)) {
                contentArea.contentX += deltaX;
            } else if (contentArea.contentX + deltaX < 0) {
                contentArea.contentX = 0;
            } else if (contentArea.contentX + deltaX > maxXOfTimeline) {
                contentArea.contentX = maxXOfTimeline;
            }
        }

        if (maxYOfTimeline > 0) {
            if ((contentArea.contentY + deltaY >= 0)
                    && (contentArea.contentY + deltaY <= maxYOfTimeline)) {
                contentArea.contentY += deltaY;
            } else if (contentArea.contentY + deltaY < 0) {
                contentArea.contentY = 0;
            } else if (contentArea.contentY + deltaY > maxYOfTimeline) {
                contentArea.contentY = maxYOfTimeline;
            }
        }

    }


    //--------------------------------
    //
    // Behavior
    //
    //--------------------------------
    Connections {
        target: IngeScapeEditorC
        onResetMappindAndTimeLineViews : {
            contentArea.contentX = 0;
            contentArea.contentY = 0;
        }
    }

    //--------------------------------
    //
    // Content
    //
    //--------------------------------
    // Background
    I2CustomRectangle {
        id: background

        anchors {
            fill : parent
        }
        color: IngeScapeTheme.scenarioBackgroundColor
        fuzzyRadius: 8
    }

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

                        color : IngeScapeTheme.blackColor

                        // Lower separator
                        Rectangle {
                            anchors.left: parent.left
                            anchors.right: parent.right
                            anchors.bottom: parent.bottom

                            height: 1
                            color: IngeScapeTheme.veryDarkGreyColor
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
                    model: viewController.filteredListTimeTicks

                    // NB: two items to avoid complex QML bindings that
                    //     are interpreted by the Javascript stack
                    delegate : Item {
                        x: viewController.convertTimeInMillisecondsToAbscissaInCoordinateSystem(model.timeInMilliSeconds, viewController.pixelsPerMinute)
                        y: 0

                        I2Line {
                            useSvgGeometry: true
                            x1: 0
                            x2: 0
                            y1: 0
                            y2: timeLinesContent.height
                            visible: model.isBigTick
                            stroke: IngeScapeTheme.veryDarkGreyColor
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
                // update time coordinates X axis
                if (viewController)
                {
                    viewController.updateTimeCoordinatesOfTimeTicks();
                }

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
                }

                onWheel: {
                    wheel.accepted = true;

                    if ((wheel.pixelDelta.x !== 0) || (wheel.pixelDelta.y !== 0))
                    {
                        //
                        // Trackpad flick gesture => scroll our workspace
                        //
                        rootItem.dragViewWithDelta (wheel.pixelDelta.x, wheel.pixelDelta.y)
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
                    hoverEnabled: true

                    property real previousPositionX : 0.0;
                    property real previousPositionY : 0.0;

                    onPressed: {
                        rootItem.forceActiveFocus();
                        previousPositionX = mouse.x;
                        previousPositionY = mouse.y;
                    }

                    onPositionChanged: {
                        if (pressed) {
                            var deltaX = previousPositionX - mouse.x;
                            var deltaY = previousPositionY - mouse.y;

                            rootItem.dragViewWithDelta (deltaX, deltaY)

                            previousPositionX = mouse.x;
                            previousPositionY = mouse.y;
                        }
                    }

                    onClicked: {
                        // deselect action in timeline
                        if (controller && controller.selectedActionVMInTimeline) {
                            controller.selectedActionVMInTimeline = null;

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


                            // update time coordinates X axis
                            if (viewController)
                            {
                                viewController.updateTimeCoordinatesOfTimeTicks();
                            }
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

                            Repeater {
                                model : controller ? controller.filteredListActionsInTimeLine : 0;

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

                                svgFileCache: IngeScapeTheme.svgFileINGESCAPE
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
                                    color : IngeScapeTheme.blueGreyColor2
                                }

                                // Not revert action
                                I2SvgItem {
                                    x : - width /2.0;
                                    anchors {
                                        verticalCenter: parent.verticalCenter
                                    }

                                    visible : ghostAction.actionModelGhost && !ghostAction.actionModelGhost.shallRevert

                                    svgFileCache : IngeScapeTheme.svgFileINGESCAPE;
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
                                        color : IngeScapeTheme.lightGreyColor;

                                    }

                                    I2SvgItem {
                                        id : actionExecutionGhost
                                        x : 0;
                                        y : 0

                                        svgFileCache : IngeScapeTheme.svgFileINGESCAPE;
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
                                        svgFileCache : IngeScapeTheme.svgFileINGESCAPE;
                                        svgElementId:"revertAction" ;
                                    }
                                }
                            }

                        }


                    }

                    //
                    // Vertical scroll bar
                    //
                    Rectangle {
                        id : scrollBarVertical
                        y: contentArea.visibleArea.yPosition * contentArea.height
                        anchors {
                            right : contentArea.right
                        }
                        width : 8
                        height: contentArea.visibleArea.heightRatio * contentArea.height

                        color: IngeScapeTheme.lightGreyColor
                        opacity : 0.8
                        radius: 10
                        visible : contentArea.visibleArea.heightRatio < 1
                    }
                }


            }

        }


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

                        color: IngeScapeTheme.whiteColor
                    }
                }

            }
        }


    }



    // Timeline Header
    Item {
        id: columnHeadersArea

        anchors {
            top: scrollTimeLine.bottom
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
                    wheel.accepted = true;

                    if ((wheel.pixelDelta.x !== 0) || (wheel.pixelDelta.y !== 0))
                    {
                        //
                        // Trackpad flick gesture => scroll our workspace
                        //
                        rootItem.dragViewWithDelta (wheel.pixelDelta.x, wheel.pixelDelta.y)
                    }
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


                            // update time coordinates X axis
                            if (viewController)
                            {
                                viewController.updateTimeCoordinatesOfTimeTicks();
                            }
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
                    model:  viewController.filteredListTimeTicks

                    delegate: Item {
                        x: viewController.convertTimeInMillisecondsToAbscissaInCoordinateSystem(model.timeInMilliSeconds, viewController.pixelsPerMinute)
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
                            color: IngeScapeTheme.darkGreyColor
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
                            font.family: IngeScapeTheme.textFontFamily

                            color: IngeScapeTheme.darkGreyColor
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
                        color :  currentTimeMouseArea.pressed ? IngeScapeTheme.darkBlueGreyColor : IngeScapeTheme.blueGreyColor2
                        border {
                            width : 1
                            color: currentTimeMouseArea.pressed ? IngeScapeTheme.lightGreyColor : IngeScapeTheme.whiteColor
                        }

                        Text {
                            id : currentTimeText
                            anchors {
                                centerIn : parent
                                verticalCenterOffset: 1
                            }

                            text : controller ? controller.currentTime.toLocaleTimeString(Qt.locale(), "HH':'mm':'ss':'zzz") : "00:00:00.000"
                            color: IngeScapeTheme.whiteColor
                            font {
                                family: IngeScapeTheme.textFontFamily
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

                        svgFileCache : IngeScapeTheme.svgFileINGESCAPE;
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

                        // CurrentTimeItem => viewController
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
            topMargin: 22
            left: columnHeadersArea.left
            right: columnHeadersArea.right
        }

        height: 13
        color : IngeScapeTheme.blackColor

        Rectangle {
            id : scrollBarHorizontal
            anchors {
                verticalCenter: parent.verticalCenter
            }
            height : 13

            property var scrollBarSize: if (viewController) {
                                           Math.max(8,(viewController.viewportWidth*scrollTimeLine.width)/viewController.timeTicksTotalWidth);
                                        }
                                        else {
                                            0
                                        }
            width : scrollBarSize
            color:  mouseArea.containsPress? IngeScapeTheme.veryDarkGreyColor : IngeScapeTheme.darkGreyColor;
            border {
                color : IngeScapeTheme.blackColor;
                width : 3
            }

            MouseArea {
                id: mouseArea

                anchors.fill: scrollBarHorizontal

                hoverEnabled: true

                drag.smoothed: false
                drag.target: scrollBarHorizontal

                drag.minimumX : 0
                drag.maximumX : scrollTimeLine.width - scrollBarHorizontal.width
                drag.minimumY : 0
                drag.maximumY :  0
            }
        }
    }


    // Button to reduce timeline
    I2CustomRectangle {
        anchors {
            right : parent.right
            bottom : parent.top
            rightMargin: 40
        }
        height : 20
        width : 77

        topLeftRadius: 5
        topRightRadius: 5

        color :  mouseAreaReduceTimeLine.pressed? IngeScapeTheme.darkBlueGreyColor : IngeScapeTheme.editorsBackgroundColor

        I2SvgItem {
            anchors.centerIn: parent

            svgFileCache: IngeScapeTheme.svgFileINGESCAPE
            svgElementId: "reduceTimeline"

            rotation : (rootItem.isReduced) ? 180 : 0;
        }

        MouseArea {
            id : mouseAreaReduceTimeLine
            anchors.fill: parent

            onClicked: {
                rootItem.isReduced = !rootItem.isReduced
            }
        }
    }


    //Zone allowing to change height of the time line
    Rectangle {
        id : resizeRectangle
        anchors {
            left : parent.left
            right : parent.right
            top : parent.top
        }
        height : 14

        color :  mouseAreaResizeTimeLine.pressed? IngeScapeTheme.darkBlueGreyColor : IngeScapeTheme.editorsBackgroundColor

        I2SvgItem {
            anchors.centerIn: parent

            svgFileCache: IngeScapeTheme.svgFileINGESCAPE
            svgElementId: "resizeTimeline"
        }

        Rectangle {
            anchors {
                left : parent.left
                right : parent.right
                bottom : parent.bottom
            }
            height : 1
            color : IngeScapeTheme.blackColor
        }

        MouseArea {
            id : mouseAreaResizeTimeLine

            anchors.fill: parent

            hoverEnabled: true

            smooth: false

            property real previousPositionY : 0.0;

            onPressed: {
                rootItem.forceActiveFocus();
                previousPositionY = mouse.y;

                // Disable resize animations
                rootItem._canPerformResizeAnimations = false;
            }

            onPositionChanged: {
                if (mouseAreaResizeTimeLine.pressed) {
                    var deltaY = previousPositionY - mouse.y;

                    if (rootItem.height + deltaY > IngeScapeTheme.bottomPanelHeight) {
                        rootItem.height += deltaY;
                    }
                    else {
                        rootItem.height = IngeScapeTheme.bottomPanelHeight;
                    }
                }
            }

            onReleased: {
                // Restore resize animations
                rootItem._canPerformResizeAnimations = true;
            }
        }
    }

    // Play Button
    Item {
        anchors {
            left : parent.left
            right : columnHeadersArea.left
            top : parent.top
            topMargin: 20
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
                fileCache: IngeScapeTheme.svgFileINGESCAPE

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
            color :  IngeScapeTheme.blueGreyColor2
            border {
                width : 1
                color: IngeScapeTheme.whiteColor
            }

            Text {
                id : currenTime
                anchors {
                    centerIn : parent
                    verticalCenterOffset: 1
                }

                text : currentTimeText.text
                color: IngeScapeTheme.whiteColor
                font {
                    family: IngeScapeTheme.textFontFamily
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





    // scrollBarHorizontal => contentArea
    Binding {
        target: contentArea
        property: "contentX"
        value: if (viewController) {
                   (scrollBarHorizontal.x * viewController.timeTicksTotalWidth)/scrollTimeLine.width
               }
               else {
                   0
               }
        when: mouseArea.drag.active
    }

    // viewController => scrollBarHorizontal
    Binding {
        target: scrollBarHorizontal
        property: "x"
        value: if (viewController) {
                   Math.max(0, (viewController.viewportX*scrollTimeLine.width)/viewController.timeTicksTotalWidth);
               }
               else {
                   0
               }
        when: !mouseArea.drag.active
    }


}
