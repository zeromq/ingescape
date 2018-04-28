/*
 *  IngeScape - QML binding
 *
 *  Copyright (c) 2018 Ingenuity i/o. All rights reserved.
 *
 *  See license terms for the rights and conditions
 *  defined by copyright holders.
 *
 *
 *  Contributors:
 *      Alexandre Lemort <lemort@ingenuity.io>
 *
 */

#include "ingescapequickinputspropertymap.h"


#include <QDebug>

extern "C" {
#include <ingescape.h>
}



/**
 * @brief Default constructor
 * @param parent
 */
IngeScapeQuickInputsPropertyMap::IngeScapeQuickInputsPropertyMap(QObject *parent) : QQmlPropertyMap(this, parent)
{
}



/**
 * @brief Method provided to intercept updates to a property from QML
 *
 * @param key          Name of the property
 * @param newQmlValue  The new value provided by QML
 *
 * @return the value that will really be stored in our property
 */
QVariant IngeScapeQuickInputsPropertyMap::updateValue(const QString &key, const QVariant &newQmlValue)
{
    Q_UNUSED(newQmlValue)

    // Check if this input exists
    if (igs_checkInputExistence(key.toStdString().c_str()))
    {
        // Inputs are read-only
        qWarning() << "IngeScapeQuick warning: inputs are read-only." <<  QString("IngeScape.inputs.%1").arg(key) << "can not be updated from QML";

        // Return our previous value to preserve it
        return value(key);
    }
    else
    {
        // This input does not exist. It may be a typo
        qWarning() << "IngeScapeQuick warning:" << QString("IngeScape.inputs.%1").arg(key) << "is invalid because input"
                   << key << "does not exist. It may be a typo or this input it not yet created or has been removed";

        // Return an empty QVariant
        return QVariant();
    }
}
