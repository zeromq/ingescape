/*
 *	IngeScape Editor
 *
 *  Copyright © 2019 Ingenuity i/o. All rights reserved.
 *
 *	See license terms for the rights and conditions
 *	defined by copyright holders.
 *
 *
 *	Contributors:
 *      Chloé Roumieu   roumieu@ingenuity.io
 */

import QtQuick 2.8

import INGESCAPE 1.0

import "../theme" as Theme

Item {
    id: rootItem

    //--------------------------------------------------------
    //
    // Properties
    //
    //--------------------------------------------------------

    // Model of the record
    property var recordM : null

    // Controller of the timeline
    property var timeLineController: null;

    // height of a line in the time line
    property int lineHeight: IngeScapeTheme.lineInTimeLineHeight

     x: recordM && timeLineController
            ? timeLineController.convertTimeInMillisecondsToAbscissaInCoordinateSystem(recordM.startTimeInTimeline, timeLineController.pixelsPerMinute)
            : 0

     width: recordM && timeLineController
                ? timeLineController.convertTimeInMillisecondsToAbscissaInCoordinateSystem(recordM.endTimeInTimeline, timeLineController.pixelsPerMinute) - rootItem.x
                : 0

     height: 5


     //--------------------------------------------------------
     //
     // Content
     //
     //--------------------------------------------------------

     Rectangle {
         anchors.fill: parent
         opacity: 0.5
         color: IngeScapeAssessmentsTheme.blueButton
     }
}