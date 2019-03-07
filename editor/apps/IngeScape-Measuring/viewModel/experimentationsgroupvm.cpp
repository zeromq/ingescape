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

#include "experimentationsgroupvm.h"

/**
 * @brief Constructor
 * @param name
 * @param parent
 */
ExperimentationsGroupVM::ExperimentationsGroupVM(QString name,
                                                 QObject *parent) : QObject(parent),
    _name(name)
{
    // Force ownership of our object, it will prevent Qml from stealing it
    QQmlEngine::setObjectOwnership(this, QQmlEngine::CppOwnership);

    qInfo() << "New Experimentations Group VM" << _name;


    // Experimentations are sorted on their name (alphabetical order)
    _experimentations.setSortProperty("name");

}


/**
 * @brief Destructor
 */
ExperimentationsGroupVM::~ExperimentationsGroupVM()
{
    qInfo() << "Delete Experimentations Group VM" << _name;

}
