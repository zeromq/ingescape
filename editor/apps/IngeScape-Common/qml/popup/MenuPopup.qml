/*
 *	IngeScape Editor
 *
 *  Copyright © 2018 Ingenuity i/o. All rights reserved.
 *
 *	See license terms for the rights and conditions
 *	defined by copyright holders.
 *
 *
 *	Contributors:
 *      Fabien Esteveny   <esteveny@ingenuity.io>
 *
 */

import QtQuick 2.0
import I2Quick 1.0

I2PopupBase {
    id: popup

    // Open popup next to parent but check if height + y is not upper than screen's height
    function openInScreen()
    {
        // Call I2PopupBase open function
        open();

        // Configure x and y to stay in screen
        if (isOpened)
        {
            var parentHeight = parent.height;
            if (popup.y + popup.height > parentHeight) {
                popup.y = parentHeight - popup.height;
            }
        }
    }
}
