/*
 *	IngeScape Common
 *
 *  Copyright © 2019 Ingenuity i/o. All rights reserved.
 *
 *	See license terms for the rights and conditions
 *	defined by copyright holders.
 *
 *
 *	Contributors:
 *      Mathieu Soum       <soum@ingenuity.io>
 *
 */

#include "urlutils.h"

#include <QFileInfo>

/**
 * @brief Extract the filename of the given file:// URL.
 * Only QUrl with 'file://' as scheme or an empty scheme are accepted.
 * In other cases, an empry string is returned
 * @param url
 * @return
 */
QString URLUtils::fileNameFromFileUrl(const QUrl& url)
{
    if (!url.scheme().isEmpty() && (url.scheme() != "file")) {
        // Discard other schemes than file:// or an empty string
        return "";
    }

    return QFileInfo(url.path()).fileName();
}
