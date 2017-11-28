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
                clip : true
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
                            id : actionVMItem

                            property  var myActionVM : model.QtObject;

                            x : myActionVM? viewController.convertTimeInMillisecondsToAbscissaInCoordinateSystem(myActionVM.startTime, viewController.pixelsPerMinute) : 0;
                            y : myActionVM? (rootItem.lineHeight * myActionVM.lineInTimeLine) : à;
                            height : rootItem.lineHeight
                            width : if (myActionVM && myActionVM.actionModel) {
                                        switch (myActionVM.actionModel.validityDurationType)
                                        {
                                        case ValidationDurationType.IMMEDIATE:
                                            0;
                                            break;
                                        case ValidationDurationType.FOREVER:
                                            (viewController.timeTicksTotalWidth - viewController.convertTimeInMillisecondsToAbscissaInCoordinateSystem(myActionVM.startTime, viewController.pixelsPerMinute))
                                            break;
                                        case ValidationDurationType.CUSTOM:
                                            viewController.convertDurationInSecondsToLengthInCoordinateSystem(myActionVM.actionModel.validityDuration/1000, viewController.pixelsPerMinute)
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
                                id : conditionsValidityRect
                                anchors {
                                    top : parent.top
                                    left : parent.left
                                    right : parent.right
                                }

                                height : rootItem.lineHeight/2
                                color : MasticTheme.blueGreyColor2
                            }

                            // executionsList
                            Repeater {
                                model: if (myActionVM) {
                                           myActionVM.executionsList;
                                       }
                                       else {
                                           0;
                                       }

                                Item {
                                    height : conditionsValidityRect.height
                                    width : conditionsValidityRect.width

                                    // Not revert action
                                    I2SvgItem {
                                        x : viewController.convertDurationInSecondsToLengthInCoordinateSystem(model.executionTime/1000, viewController.pixelsPerMinute) - width/2;

                                        anchors {
                                            verticalCenter: parent.verticalCenter
                                        }

                                        visible : !model.shallRevert

                                        svgFileCache : MasticTheme.svgFileMASTIC;
                                        svgElementId: (model.neverExecuted)?
                                                          "notExecutedAction"
                                                        : ((model.isExecuted) ? "timelineAction" : "currentAction");
                                    }

                                    // Revert action
                                    Item {
                                        visible : model.shallRevert
                                        height : childrenRect.height
                                        anchors {
                                            verticalCenter: parent.verticalCenter
                                        }

                                        I2SvgItem {
                                            id : actionExecution
                                            x : viewController.convertDurationInSecondsToLengthInCoordinateSystem(model.executionTime/1000, viewController.pixelsPerMinute);
                                            y : 0

                                            svgFileCache : MasticTheme.svgFileMASTIC;
                                            svgElementId: (model.neverExecuted)?
                                                              "notExecutedRevertAction"
                                                            : ((model.isExecuted) ? "revertAction" : "currentRevertAction");
                                        }

                                        Rectangle {
                                            anchors {
                                                verticalCenter: actionExecution.verticalCenter
                                                left : actionExecution.horizontalCenter
                                                right : revertActionExecution.horizontalCenter
                                            }
                                            height : 1
                                            color : MasticTheme.whiteColor;

                                        }

                                        I2SvgItem {
                                            id : revertActionExecution
                                            x : viewController.convertDurationInSecondsToLengthInCoordinateSystem(model.reverseTime/1000, viewController.pixelsPerMinute) - width;
                                            y : 0
                                            rotation : 180
                                            svgFileCache : MasticTheme.svgFileMASTIC;
                                            svgElementId: (model.neverExecuted)?
                                                              "notExecutedRevertAction"
                                                            : ((model.isExecuted) ? "revertAction" : "currentRevertAction");
                                        }
                                    }
                                }
                            }

                            Rectangle {
                                id : backgroundActionName
                                anchors {
                                    fill : actionName
                                    leftMargin:-1
                                    rightMargin:-1
                                }
                                color : MasticTheme.blackColor

                                MouseArea {
                                    id : openEditorMouseArea
                                    anchors.fill : parent
                                    hoverEnabled: true
                                    onClicked: {
                                        if (controller && actionVMItem.myActionVM) {
                                            controller.openActionEditorFromActionVM(actionVMItem.myActionVM)
                                        }
                                    }
                                }
                            }

                            Text {
                                id: actionName
                                anchors {
                                    top : parent.verticalCenter
                                    bottomMargin: 1
                                    bottom : parent.bottom
                                    left : parent.left
                                }
                                verticalAlignment: Text.AlignVCenter
                                color : openEditorMouseArea.pressed? MasticTheme.greyColor : MasticTheme.darkGreyColor
                                text : model.actionModel ? model.actionModel.name : ""
                                font {
                                    family : MasticTheme.textFontFamily
                                    pixelSize: 11
                                }


                                // underline
                                Rectangle {
                                    visible: openEditorMouseArea.containsMouse

                                    anchors {
                                        left : parent.left
                                        right : parent.right
                                        bottom : parent.bottom
                                        bottomMargin : 1
                                    }

                                    height : 1

                                    color : actionName.color
                                }


                            }

                        }
                    }



                    // allow dropping actions in timeline
                    DropArea {
                        anchors.fill: parent
                        keys: ["ActionsListItem"]

                        onEntered: {
                            var dragItem = drag.source;
                            // display ghost
                            ghostAction.actionModelGhost = dragItem.action;
                        }

                        onPositionChanged: {
                            var dragItem = drag.source;

                            var timeInMilliSeconds = viewController.convertAbscissaInCoordinateSystemToTimeInMilliseconds(drag.x, viewController.pixelsPerMinute)
                            var lineNumber = Math.floor(drag.y / rootItem.lineHeight)
                            var canInsertActionVM = false;

                            if (controller && (typeof dragItem.action !== 'undefined' && controller)) {
                                // test if the drop is possible
                                canInsertActionVM = controller.canInsertActionVMTo(dragItem.action, timeInMilliSeconds, lineNumber)

                                if ( canInsertActionVM ) {
                                    // move ghost
                                    ghostAction.actionModelGhost = dragItem.action;
                                    ghostAction.x = viewController.convertTimeInMillisecondsToAbscissaInCoordinateSystem(timeInMilliSeconds, viewController.pixelsPerMinute);
                                    ghostAction.y = lineNumber * rootItem.lineHeight;
                                    ghostAction.startTime = timeInMilliSeconds;
                                }
                                else {
                                    // remove ghost
                                    ghostAction.actionModelGhost = null;
                                }

                            }

                        }

                        onExited: {
                            // remove ghost
                            ghostAction.actionModelGhost = null;
                        }

                        onDropped: {
                            var dragItem = drag.source;
                            var timeInMilliSeconds = viewController.convertAbscissaInCoordinateSystemToTimeInMilliseconds(drag.x, viewController.pixelsPerMinute)
                            var lineNumber = Math.floor(drag.y / rootItem.lineHeight)

                            if (typeof dragItem.action !== 'undefined' && controller)
                            {

                                controller.addActionVMAtTime(dragItem.action, timeInMilliSeconds, lineNumber);

                                // remove ghost
                                ghostAction.actionModelGhost = null;
                            }

                        }
                    }


                    // Ghost Action
                    Item {
                        id : ghostAction
                        x: 0
                        y : 0

                        property var actionModelGhost : null;
                        property int startTime : 0;
                        opacity : (actionModelGhost !== null) ? 0.5 : 0

                        height : rootItem.lineHeight/2
                        width : if (actionModelGhost) {
                                    switch (actionModelGhost.validityDurationType)
                                    {
                                    case ValidationDurationType.IMMEDIATE:
                                        0;
                                        break;
                                    case ValidationDurationType.FOREVER:
                                        (viewController.timeTicksTotalWidth - viewController.convertTimeInMillisecondsToAbscissaInCoordinateSystem(startTime, viewController.pixelsPerMinute))
                                        break;
                                    case ValidationDurationType.CUSTOM:
                                        viewController.convertDurationInSecondsToLengthInCoordinateSystem(actionModelGhost.validityDuration/1000, viewController.pixelsPerMinute)
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
                            x : - width/2;
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

                            I2SvgItem {
                                id : actionExecutionGhost
                                x : 0;
                                y : 0

                                svgFileCache : MasticTheme.svgFileMASTIC;
                                svgElementId: "revertAction"
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
                                id : revertActionExecutionGhost
                                x : if (ghostAction.actionModelGhost) {
                                        if (ghostAction.actionModelGhost.shallRevertWhenValidityIsOver)
                                        {
                                            rect.width - width;
                                        }
                                        else if (ghostAction.actionModelGhost.shallRevertAfterTime) {
                                            viewController.convertDurationInSecondsToLengthInCoordinateSystem(ghostAction.actionModelGhost.revertAfterTime/1000, viewController.pixelsPerMinute) - width;
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
                        x: -1
                        y: 0

                        width: 1
                        height: timeLinesContent.height

                        color: MasticTheme.redColor
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
                        width : 62
                        height: 20
                        radius : 2
                        color :  MasticTheme.blueGreyColor2
                        border {
                            width : 1
                            color: MasticTheme.whiteColor
                        }

                        Text {
                            id : currentTimeText
                            anchors {
                                centerIn : parent
                                verticalCenterOffset: 1
                            }

                            text : controller ? controller.currentTime.toLocaleTimeString(Qt.locale(), "HH':'mm':'ss") : "00:00:00"
                            color: MasticTheme.lightGreyColor
                            font {
                                family: MasticTheme.textFontFamily
                                pixelSize: 14
                            }

                        }

                        MouseArea {
                            id: currentTimeMouseArea

                            anchors.fill: currentTimeLabel

                            hoverEnabled: true

                            drag.smoothed: false
                            drag.target: currentTimeItem

                            drag.minimumX : viewController.convertTimeInMillisecondsToAbscissaInCoordinateSystem(0, viewController.pixelsPerMinute)
                            drag.maximumX : viewController.convertTimeInMillisecondsToAbscissaInCoordinateSystem(86400000, viewController.pixelsPerMinute)
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

                    I2SvgItem {
                        id :svgCurrentTime
                        anchors {
                            horizontalCenter : parent.left
                            bottom : parent.bottom
                        }

                        svgFileCache : MasticTheme.svgFileMASTIC;
                        svgElementId: "currentTime"
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
                right : playScenarioBtn.right
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
