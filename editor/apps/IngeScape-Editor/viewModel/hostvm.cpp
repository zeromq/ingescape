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
 *      Bruno Lemenicier <lemenicier@ingenuity.io>
 *      Alexandre Lemort <lemort@ingenuity.io>
 *      Vincent Peyruqueou <peyruqueou@ingenuity.io>
 *
 */

#include "hostvm.h"

#include <QQmlEngine>


/**
 * @brief Constructor
 * @param model
 * @param parent
 */
HostVM::HostVM(HostM* model, QObject *parent) : QObject(parent),
    _name(""),
    _modelM(model),
    _canProvideStream(false),
    _isStreaming(false)
{
    // Force ownership of our object, it will prevent Qml from stealing it
    QQmlEngine::setObjectOwnership(this, QQmlEngine::CppOwnership);

    if (_modelM != NULL)
    {
        _name = _modelM->name();

        qInfo() << "New View Model of Host" << _name;

        if (_modelM->streamingPort().length() > 0)
        {
            _canProvideStream = true;
        }
    }
}


/**
 * @brief Destructor
 */
HostVM::~HostVM()
{
    qInfo() << "Delete View Model of Host" << _name;

    if (_modelM != NULL) {
        setmodelM(NULL);
    }

    // Clear the list of agents
    _listOfAgents.clear();
}


/**
 * @brief Change the state of our agent
 */
void HostVM::changeState()
{
    // is streaming => request streaming end
    if (_isStreaming)
    {
        //Q_EMIT commandAskedToHost("STOP_STREAMING", _modelM->name(), "");
        setisStreaming(false);
    }
    // is not streaming => request streaming
    else
    {
        //Q_EMIT commandAskedToHost("START_STREAMING", _modelM->name(), "");
        setisStreaming(true);
    }
}
