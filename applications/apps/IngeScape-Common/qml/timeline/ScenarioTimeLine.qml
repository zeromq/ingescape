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

    // Controllers associated to our view
    property var scenarioController: null;
    property var timeLineController: null;


    // graphical properties
    property int linesNumber: scenarioController ? scenarioController.linesNumberInTimeLine : 0;
    property int lineHeight: IngeScapeTheme.lineInTimeLineHeight;

    // flag indicating if our component is reduced or expanded
    property bool isReduced: true;

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
            rootItem.height = IngeScapeTheme.timeLineHeight;
            //rootItem.height = IngeScapeTheme.timeLineHeight_OneRow;
        }
    }


    // Licenses controller
    property LicensesController licensesController: IngeScapeEditorC.licensesC;

    // Flag indicating if the user have a valid license for the editor
    property bool isEditorLicenseValid: rootItem.licensesController && rootItem.licensesController.mergedLicense && rootItem.licensesController.mergedLicense.editorLicenseValidity


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
        var previousPixelsPerMinute = timeLineController.pixelsPerMinute;
        var newPixelsPerMinute = previousPixelsPerMinute * deltaScale;

        if (newPixelsPerMinute < timeLineController.minPixelsPerMinute)
        {
            newPixelsPerMinute = timeLineController.minPixelsPerMinute;
            deltaScale = newPixelsPerMinute/previousPixelsPerMinute;
        }
        else if (newPixelsPerMinute > timeLineController.maxPixelsPerMinute)
        {
            newPixelsPerMinute = timeLineController.maxPixelsPerMinute;
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
        timeLineController.pixelsPerMinute *= deltaScale;
    }

    function dragViewWithDelta (deltaX, deltaY) {

        var maxXOfTimeline = contentArea.contentWidth - contentArea.width;
        var maxYOfTimeline = contentArea.contentHeight - contentArea.height;

        if (maxXOfTimeline > 0) {
            if ((contentArea.contentX + deltaX >= 0)
                    && (contentArea.contentX + deltaX <= maxXOfTimeline)) {
                contentArea.contentX += deltaX;
            }
            else if (contentArea.contentX + deltaX < 0) {
                contentArea.contentX = 0;
            }
            else if (contentArea.contentX + deltaX > maxXOfTimeline) {
                contentArea.contentX = maxXOfTimeline;
            }
        }

        if (maxYOfTimeline > 0) {
            if ((contentArea.contentY + deltaY >= 0)
                    && (contentArea.contentY + deltaY <= maxYOfTimeline)) {
                contentArea.contentY += deltaY;
            }
            else if (contentArea.contentY + deltaY < 0) {
                contentArea.contentY = 0;
            }
            else if (contentArea.contentY + deltaY > maxYOfTimeline) {
                contentArea.contentY = maxYOfTimeline;
            }
        }

    }


    //--------------------------------
    //
    // Behaviors
    //
    //--------------------------------
    Connections {
        target: IngeScapeEditorC

        onResetMappindAndTimeLineViews : {
            contentArea.contentX = 0;
            contentArea.contentY = 0;
        }

        //ignoreUnknownSignals: true
    }


    //--------------------------------
    //
    // Signals
    //
    //--------------------------------

    // Signal emitted when the user tries to perform an action forbidden by the license
    signal unlicensedAction();


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
        color: IngeScapeTheme.veryDarkGreyColor
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
                            anchors {
                                left: parent.left
                                right: parent.right
                                bottom: parent.bottom
                            }

                            height: 1
                            color: IngeScapeTheme.veryDarkGreyColor
                        }

                        Text {
                            anchors {
                                left: parent.left
                                leftMargin: 5
                                verticalCenter: parent.verticalCenter
                            }

                            text: (index === 0) ? "Manual" : index

                            color: IngeScapeTheme.veryDarkGreyColor

                            font {
                                family: IngeScapeTheme.textFontFamily
                                pixelSize: 12
                            }
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

            contentWidth: timeLineController.timeTicksTotalWidth
            contentHeight: timeLineArea.height
            clip : true

            Item {
                id: timeLinesContent

                width: timeLineController.timeTicksTotalWidth
                height: timeLineArea.height


                // Time ticks
                Repeater {
                    model: timeLineController.filteredListTimeTicks

                    // NB: two items to avoid complex QML bindings that
                    //     are interpreted by the Javascript stack
                    delegate : Item {
                        x: timeLineController.convertTimeInMillisecondsToAbscissaInCoordinateSystem(model.timeInMilliSeconds, timeLineController.pixelsPerMinute)
                        y: 0

                        anchors {
                            top: parent.top
                            bottom: parent.bottom
                        }


                        DashedBar {
                            anchors {
                                top: parent.top
                                bottom: parent.bottom
                                horizontalCenter: parent.left
                            }

                            width: 1

                            visible: model.isBigTick

                            color: IngeScapeTheme.veryDarkGreyColor

                            dashArray: "3, 3"
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
                if (pinch.pointCount >= 2 && timeLineController)
                {
                    // Compute delta between our new scale factor and the previous one
                    var deltaScale = pinch.scale/pinch.previousScale;

                    rootItem.updateZoomOfTimeLine (deltaScale, contentArea.contentX + contentArea.width/2, contentArea.contentY + contentArea.height/2);
                }
            }

            onPinchFinished: {
                // update time coordinates X axis
                if (timeLineController)
                {
                    timeLineController.updateTimeCoordinatesOfTimeTicks();
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
                        if (scenarioController && scenarioController.selectedActionVMInTimeline) {
                            scenarioController.selectedActionVMInTimeline = null;

                        }
                    }

                    onWheel: {
                        wheel.accepted = true;

                        // with ctrl => zoom In and zoom out
                        if (wheel.modifiers && Qt.ControlModifier) {
                            var previousPixelsPerMinute = timeLineController.pixelsPerMinute;
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
                            if (timeLineController)
                            {
                                timeLineController.updateTimeCoordinatesOfTimeTicks();
                            }
                        }

                        // else navigation in vertical
                        else {
                            var yMaxOfTimeLine = rootItem.lineHeight * rootItem.linesNumber - timeLineController.viewportHeight;
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
                            width: timeLineController.timeTicksTotalWidth
                            height: rootItem.lineHeight * rootItem.linesNumber

                            Repeater {
                                model: scenarioController ? scenarioController.filteredListActionsInTimeLine : 0;

                                ActionInTimeLine {
                                    myActionVM: model.QtObject;

                                    scenarioController: rootItem.scenarioController
                                    timeLineController: rootItem.timeLineController
                                }
                            }

                            // dropArea allow dropping actions in timeline
                            DropArea {
                                anchors.fill: parent
                                keys: ["ActionsListItem", "ActionInTimeLine"]
                                enabled: rootItem.isEditorLicenseValid

                                onEntered: {
                                    if (drag.source !== null)
                                    {
                                        var dragItem = drag.source;

                                        // Check if our source has an "action" property
                                        if (typeof dragItem.action !== 'undefined')
                                        {
                                            // display ghost
                                            ghostAction.actionModelGhost = dragItem.action;
                                        }
                                    }
                                }

                                onPositionChanged: {
                                    var dragItem = drag.source;

                                    var startInDateTime = timeLineController.convertAbscissaInCoordinateSystemToQDateTime(drag.x, timeLineController.pixelsPerMinute);
                                    var starttimeInMilliseconds = timeLineController.convertAbscissaInCoordinateSystemToTimeInMilliseconds(drag.x, timeLineController.pixelsPerMinute)

                                    var lineNumber = Math.floor(drag.y / rootItem.lineHeight)
                                    var canInsertActionVM = false;

                                    // action comes from the actions list
                                    if (scenarioController && (typeof dragItem.action !== 'undefined'))
                                    {
                                        // test if the drop is possible
                                        canInsertActionVM = scenarioController.canInsertActionVMTo(dragItem.action, starttimeInMilliseconds, lineNumber)

                                        if (canInsertActionVM) {
                                            ghostDropImpossible.visible = false;
                                            // move ghost
                                            ghostAction.actionModelGhost = dragItem.action;
                                            ghostAction.x = timeLineController.convertQTimeToAbscissaInCoordinateSystem(startInDateTime, timeLineController.pixelsPerMinute);
                                            ghostAction.y = lineNumber * rootItem.lineHeight;
                                            ghostAction.startTime = startInDateTime;
                                            if  (typeof dragItem.temporaryStartTime !== 'undefined') {
                                                dragItem.temporaryStartTime = startInDateTime;
                                            }
                                        }
                                        else {
                                            // remove ghost
                                            ghostAction.actionModelGhost = null;

                                            // ghost drop impossible
                                            ghostDropImpossible.x = timeLineController.convertQTimeToAbscissaInCoordinateSystem(startInDateTime, timeLineController.pixelsPerMinute);
                                            ghostDropImpossible.y = lineNumber * rootItem.lineHeight + (rootItem.lineHeight/2 - ghostDropImpossible.width/2);
                                            ghostDropImpossible.visible = true;
                                        }

                                    }

                                    // action comes from the timeline
                                    if (scenarioController && (typeof dragItem.myActionVM !== 'undefined') && (dragItem.myActionVM.modelM !== null))
                                    {
                                        // test if the drop is possible
                                        canInsertActionVM = scenarioController.canInsertActionVMTo(dragItem.myActionVM.modelM, starttimeInMilliseconds, lineNumber, dragItem.myActionVM)

                                        if (canInsertActionVM) {
                                            ghostDropImpossible.visible = false;
                                            // move ghost
                                            ghostAction.actionModelGhost = dragItem.myActionVM.modelM;
                                            ghostAction.x = timeLineController.convertQTimeToAbscissaInCoordinateSystem(startInDateTime, timeLineController.pixelsPerMinute);
                                            ghostAction.y = lineNumber * rootItem.lineHeight;
                                            ghostAction.startTime = startInDateTime;
                                            if  (typeof dragItem.temporaryStartTime !== 'undefined') {
                                                dragItem.temporaryStartTime = startInDateTime;
                                            }
                                        }
                                        else {
                                            // remove ghost
                                            ghostAction.actionModelGhost = null;

                                            // ghost drop impossible
                                            ghostDropImpossible.x = timeLineController.convertQTimeToAbscissaInCoordinateSystem(startInDateTime, timeLineController.pixelsPerMinute);
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
                                    var timeInMilliseconds = timeLineController.convertAbscissaInCoordinateSystemToTimeInMilliseconds(drag.x, timeLineController.pixelsPerMinute);
                                    var lineNumber = Math.floor(drag.y / rootItem.lineHeight);

                                    // The first line is reserved for live insertions from the palette
                                    if (lineNumber > 0)
                                    {
                                        // action comes from the actions list : add the action on the time line
                                        if ((typeof dragItem.action !== 'undefined') && scenarioController)
                                        {
                                            scenarioController.addActionVMAtTime(dragItem.action, timeInMilliseconds, lineNumber);
                                        }


                                        // action comes from the timeline : update start time and line number
                                        if ((typeof dragItem.myActionVM !== 'undefined') && (dragItem.myActionVM.modelM !== null))
                                        {
                                            scenarioController.moveActionVMAtTimeAndLine(dragItem.myActionVM, timeInMilliseconds, lineNumber);
                                        }
                                    }

                                    // remove ghost
                                    ghostAction.actionModelGhost = null;
                                    ghostDropImpossible.visible = false;

                                    if (typeof dragItem.temporaryStartTime !== 'undefined') {
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

                                svgFileCache: IngeScapeTheme.svgFileIngeScape
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
                                            case ValidationDurationTypes.IMMEDIATE:
                                                0;
                                                break;
                                            case ValidationDurationTypes.FOREVER:
                                                (timeLineController.timeTicksTotalWidth - timeLineController.convertQTimeToAbscissaInCoordinateSystem(startTime, timeLineController.pixelsPerMinute))
                                                break;
                                            case ValidationDurationTypes.CUSTOM:
                                                timeLineController.convertDurationInMillisecondsToLengthInCoordinateSystem(actionModelGhost.validityDuration, timeLineController.pixelsPerMinute)
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

                                    svgFileCache: IngeScapeTheme.svgFileIngeScape
                                    svgElementId: "timelineAction"
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

                                        svgFileCache: IngeScapeTheme.svgFileIngeScape
                                        svgElementId: "revertAction"
                                    }

                                    I2SvgItem {
                                        id: revertActionExecutionGhost

                                        x: if (ghostAction.actionModelGhost) {
                                               if (ghostAction.actionModelGhost.shallRevertWhenValidityIsOver)
                                               {
                                                   rect.width - width;
                                               }
                                               else if (ghostAction.actionModelGhost.shallRevertAfterTime) {
                                                   timeLineController.convertDurationInMillisecondsToLengthInCoordinateSystem(ghostAction.actionModelGhost.revertAfterTime, timeLineController.pixelsPerMinute) - width;
                                               }
                                               else {
                                                   0;
                                               }
                                           }
                                           else {
                                               0;
                                           }

                                        y: 0
                                        rotation: 180
                                        svgFileCache: IngeScapeTheme.svgFileIngeScape
                                        svgElementId:"revertAction"
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
            id: currentTimeVerticalLine

            anchors.fill: parent

            interactive: false
            contentX: contentArea.contentX

            contentWidth: timeLineController.timeTicksTotalWidth
            contentHeight: timeLineArea.height
            clip : true

            Item {
                width: timeLineController.timeTicksTotalWidth
                height: timeLineArea.height

                // NB: two items to avoid complex QML bindings that
                //     are interpreted by the Javascript stack
                Item {
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

            contentWidth: timeLineController.timeTicksTotalWidth
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

                            var previousPixelsPerMinute = timeLineController.pixelsPerMinute;
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
                            if (timeLineController)
                            {
                                timeLineController.updateTimeCoordinatesOfTimeTicks();
                            }
                        }

                        // else navigation along timeline
                        else {
                            var xMaxOfTimeLine = timeLineController.timeTicksTotalWidth - timeLineController.viewportWidth;
                            var nbCranMolette = wheel.angleDelta.y/120.0;
                            contentArea.contentX = Math.max(0, Math.min(contentArea.contentX - nbCranMolette * 100, xMaxOfTimeLine));
                        }
                    }

                }

            }

            Item {
                id: columnHeadersContent

                width: timeLineController.timeTicksTotalWidth
                height: columnHeadersArea.height

                //
                // Time ticks
                //
                Repeater {
                    model: timeLineController.filteredListTimeTicks

                    delegate: Item {
                        x: timeLineController.convertTimeInMillisecondsToAbscissaInCoordinateSystem(model.timeInMilliSeconds, timeLineController.pixelsPerMinute)
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
                // Current time item
                //
                Item {
                    id: currentTimeItem

                    anchors {
                        bottom: columnHeadersContent.bottom
                    }

                    Rectangle {
                        id: currentTimeLabel

                        anchors {
                            horizontalCenter: svgCurrentTime.horizontalCenter
                            bottom: svgCurrentTime.top
                            bottomMargin: -1
                        }
                        width: IngeScapeTheme.timeWidth
                        height: 20

                        radius: 2

                        color: currentTimeMouseArea.pressed ? IngeScapeTheme.darkBlueGreyColor : IngeScapeTheme.blueGreyColor2
                        border {
                            width: 1
                            color: currentTimeMouseArea.pressed ? IngeScapeTheme.lightGreyColor : IngeScapeTheme.whiteColor
                        }

                        Text {
                            id: currentTimeText

                            anchors {
                                centerIn: parent
                                verticalCenterOffset: 1
                            }

                            text: scenarioController ? scenarioController.currentTime.toLocaleTimeString(Qt.locale(), "HH':'mm':'ss':'zzz")
                                                     : "00:00:00.000"

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

                        svgFileCache: IngeScapeTheme.svgFileIngeScape
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

                        drag.minimumX : timeLineController.timeMarginInPixels
                        drag.maximumX : timeLineController.timeTicksTotalWidth - timeLineController.timeMarginInPixels
                        drag.minimumY : 0
                        drag.maximumY : 0

                        // timeLineController => CurrentTimeItem
                        Binding {
                            target: scenarioController
                            property: "currentTime"
                            value: if (timeLineController) {
                                       timeLineController.convertAbscissaInCoordinateSystemToQDateTime(currentTimeItem.x, timeLineController.pixelsPerMinute)
                                   }
                                   else {
                                       0
                                   }
                            when: currentTimeMouseArea.drag.active
                        }

                        // CurrentTimeItem => timeLineController
                        Binding {
                            target: currentTimeItem
                            property: "x"
                            value: if (scenarioController && timeLineController)
                                   {
                                       timeLineController.convertQTimeToAbscissaInCoordinateSystem(scenarioController.currentTime, timeLineController.pixelsPerMinute)
                                   }
                                   else {
                                       0
                                   }
                            when: !currentTimeMouseArea.drag.active
                        }

                        // To prevent user to drag while the timeline is playing (pause it first)
                        onPressed: {
                            if (scenarioController && scenarioController.isPlaying)
                            {
                                scenarioController.pauseTimeLine();
                            }
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

            property var scrollBarSize: if (timeLineController) {
                                           Math.max(8,(timeLineController.viewportWidth*scrollTimeLine.width)/timeLineController.timeTicksTotalWidth);
                                        }
                                        else {
                                            0
                                        }
            width : scrollBarSize
            color: mouseArea.containsPress? IngeScapeTheme.veryDarkGreyColor : IngeScapeTheme.darkGreyColor;
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

                drag.minimumX: 0
                drag.maximumX: scrollTimeLine.width - scrollBarHorizontal.width
                drag.minimumY: 0
                drag.maximumY: 0
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

        color: mouseAreaReduceTimeLine.pressed ? IngeScapeTheme.darkBlueGreyColor : IngeScapeTheme.editorsBackgroundColor

        I2SvgItem {
            anchors.centerIn: parent

            svgFileCache: IngeScapeTheme.svgFileIngeScape
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


    // Zone allowing to change height of the time line
    Rectangle {
        id : resizeRectangle
        anchors {
            left : parent.left
            right : parent.right
            top : parent.top
        }
        height: 14

        color: mouseAreaResizeTimeLine.pressed ? IngeScapeTheme.darkBlueGreyColor : IngeScapeTheme.editorsBackgroundColor

        I2SvgItem {
            anchors.centerIn: parent

            svgFileCache: IngeScapeTheme.svgFileIngeScape
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

                    if (rootItem.height + deltaY > IngeScapeTheme.timeLineHeight) {
                        rootItem.height += deltaY;
                    }
                    else {
                        rootItem.height = IngeScapeTheme.timeLineHeight;
                    }
                }
            }

            onReleased: {
                // Restore resize animations
                rootItem._canPerformResizeAnimations = true;
            }
        }
    }


    // Buttons and time
    Item {
        anchors {
            left: parent.left
            right: columnHeadersArea.left
            top: parent.top
            topMargin: 20
        }


        /*// Start/Stop Record button
        Button {
            id: startOrStopRecordButton

            anchors {
                top: parent.top
                topMargin: -(20 + startOrStopRecordButton.height)
                horizontalCenter: parent.horizontalCenter
            }

            //enabled: (controller.currentReplay === null)
            //opacity: enabled ? 1.0 : 0.4

            style: I2SvgToggleButtonStyle {
                fileCache: IngeScapeTheme.svgFileIngeScape

                toggleCheckedReleasedID: "record-stop"
                toggleCheckedPressedID: toggleCheckedReleasedID + "-pressed"
                toggleUncheckedReleasedID: "record-start"
                toggleUncheckedPressedID: toggleUncheckedReleasedID + "-pressed"

                // No disabled states
                toggleCheckedDisabledID: toggleCheckedPressedID
                toggleUncheckedDisabledID: toggleUncheckedPressedID

                labelMargin: 0
            }

            onClicked: {
                if (scenarioController) {
                    console.log("QML: Start or Stop to Record");

                    //scenarioController.startOrStopToRecord();
                }
            }

            //Binding {
            //    target: startOrStopRecordButton
            //    property: "checked"
            //    value: scenarioController ? scenarioController.isRecording : false
            //}
        }*/


        // Play Button
        Button {
            id: playScenarioBtn

            anchors {
                top: parent.top
                horizontalCenter: parent.horizontalCenter
            }

            activeFocusOnPress: true
            checkable: true

            style: I2SvgToggleButtonStyle {
                fileCache: IngeScapeTheme.svgFileIngeScape

                toggleCheckedReleasedID: "timeline-pause"
                toggleCheckedPressedID: toggleCheckedReleasedID + "-pressed"
                toggleCheckedDisabledID: toggleCheckedReleasedID

                toggleUncheckedReleasedID: "timeline-play"
                toggleUncheckedPressedID: toggleUncheckedReleasedID + "-pressed"
                toggleUncheckedDisabledID: toggleUncheckedReleasedID

                labelMargin: 0;
            }

            onClicked: {
                if (scenarioController) {
                    if (!rootItem.isEditorLicenseValid) {
                        checked = false
                        rootItem.unlicensedAction();
                    }
                    else if (checked) {
                        scenarioController.playOrResumeTimeLine();
                    }
                    else {
                        scenarioController.pauseTimeLine();
                    }
                }
            }

            Binding {
                target: playScenarioBtn
                property: "checked"
                value: scenarioController ? scenarioController.isPlaying : false
            }
        }


        Rectangle {
            anchors {
                horizontalCenter: playScenarioBtn.horizontalCenter
                top: playScenarioBtn.bottom
                topMargin: 6
            }
            width: IngeScapeTheme.timeWidth
            height: 20

            radius : 2

            color: IngeScapeTheme.blueGreyColor2
            border {
                width : 1
                color: IngeScapeTheme.whiteColor
            }

            Text {
                id: currenTime

                anchors {
                    centerIn: parent
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
    // Timeline view - Connections / Bindings
    //
    //
    //--------------------------------------------------------

    // ContentArea => timeLineController
    Binding {
        target: timeLineController
        property: "viewportX"
        value: contentArea.contentX
    }

    // ContentArea => timeLineController
    Binding {
        target: timeLineController
        property: "viewportY"
        value: contentArea.contentY
    }

    // ContentArea => timeLineController
    Binding {
        target: timeLineController
        property: "viewportWidth"
        value: timeLineArea.width
    }

    // ContentArea => timeLineController
    Binding {
        target: timeLineController
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
        value: if (timeLineController) {
                   (scrollBarHorizontal.x * timeLineController.timeTicksTotalWidth) / scrollTimeLine.width;
               }
               else {
                   0;
               }
        when: mouseArea.drag.active
    }

    // timeLineController => scrollBarHorizontal
    Binding {
        target: scrollBarHorizontal
        property: "x"
        value: if (timeLineController) {
                   Math.max(0, (timeLineController.viewportX * scrollTimeLine.width) / timeLineController.timeTicksTotalWidth);
               }
               else {
                   0;
               }
        when: !mouseArea.drag.active
    }


}
