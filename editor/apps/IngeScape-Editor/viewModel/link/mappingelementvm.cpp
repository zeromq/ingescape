/*
 *	IngeScape Editor
 *
 *  Copyright © 2017-2018 Ingenuity i/o. All rights reserved.
 *
 *	See license terms for the rights and conditions
 *	defined by copyright holders.
 *
 *
 *	Contributors:
 *      Vincent Peyruqueou <peyruqueou@ingenuity.io>
 *
 */

#include "mappingelementvm.h"

MappingElementVM::MappingElementVM(QString name,
                                   ElementMappingM* modelM,
                                   QObject *parent) : QObject(parent),
    _name(name)
    //_firstModel(modelM),
{
    // Force ownership of our object, it will prevent Qml from stealing it
    QQmlEngine::setObjectOwnership(this, QQmlEngine::CppOwnership);

    _models.append(modelM);
}


/**
 * @brief Destructor
 */
MappingElementVM::~MappingElementVM()
{
    // Reset the first model
    //setfirstModel(nullptr);

    // Models are deleted elsewhere
    //_previousModelsList.clear();
    _models.clear();
}
