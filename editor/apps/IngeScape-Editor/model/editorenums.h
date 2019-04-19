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
 *      Vincent Peyruqueou <peyruqueou@ingenuity.io>
 *
 */

#ifndef EDITORENUMS_H
#define EDITORENUMS_H

#include <QObject>
#include <I2PropertyHelpers.h>
#include <model/enums.h>


static const QString command_LoadDefinition = "LOAD_THIS_DEFINITION#";
static const QString command_LoadMapping = "LOAD_THIS_MAPPING#";
static const QString command_ClearMapping = "CLEAR_MAPPING";


/**
 * @brief The EditorEnums class is a helper for enumerations and constants specific to the "Editor" application
 */
class EditorEnums : public QObject
{
    Q_OBJECT


public:
    /**
     * @brief Constructor
     * @param parent
     */
    explicit EditorEnums(QObject *parent = nullptr);


};

QML_DECLARE_TYPE(EditorEnums)

#endif // EDITORENUMS_H
