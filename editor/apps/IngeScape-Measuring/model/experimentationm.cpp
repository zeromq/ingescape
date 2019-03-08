/*
 *	IngeScape Measuring
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

#include "experimentationm.h"

/**
 * @brief Constructor
 * @param parent
 */
ExperimentationM::ExperimentationM(QObject *parent) : QObject(parent),
    _name("")
{
    // Force ownership of our object, it will prevent Qml from stealing it
    QQmlEngine::setObjectOwnership(this, QQmlEngine::CppOwnership);

    qInfo() << "New Model of Experimentation" << _name;


    // Records are sorted on their start date/time (chronological order)
    _allRecords.setSortProperty("startDateTime");
}


/**
 * @brief Destructor
 */
ExperimentationM::~ExperimentationM()
{
    qInfo() << "Delete Model of Experimentation" << _name;

}

