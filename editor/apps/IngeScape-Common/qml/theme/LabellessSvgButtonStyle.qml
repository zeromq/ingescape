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
 *      Alexandre Lemort   <lemort@ingenuity.io>
 *      Justine Limoges    <limoges@ingenuity.io>
 *
 */

import QtQuick 2.8
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4

import I2Quick 1.0


//
//  Custom I2SvgButtonStyle to remove padding and label
//
I2SvgButtonStyle {
    // No padding
    padding { left: 0; right: 0; top: 0; bottom: 0; }

    // No label
    label: Item {}
}
