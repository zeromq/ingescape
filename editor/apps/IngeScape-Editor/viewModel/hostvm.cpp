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
 *
 */

#include "hostvm.h"

#include <QQmlEngine>


/**
 * @brief Default constructor
 * @param model
 * @param parent
 */
HostVM::HostVM(HostM* model, QObject *parent) : QObject(parent),
    _hostModel(NULL),
    _canProvideStream(false),
    _isStreaming(false)
{
    // Force ownership of our object, it will prevent Qml from stealing it
    QQmlEngine::setObjectOwnership(this, QQmlEngine::CppOwnership);

    if (model != NULL)
    {
        // Init model
        sethostModel( model );

        if (model->streamingPort().length() > 0)
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
    if (_hostModel != NULL)
    {
        qInfo() << "Delete View Model of Host" << _hostModel->name();
    }
    sethostModel(NULL);
}


/**
 * @brief Change the state of our agent
 */
void HostVM::changeState()
{
    // is streaming => request streaming end
    if (_isStreaming)
    {
        //Q_EMIT commandAskedToHost("STOP_STREAMING", _hostModel->name(), "");
        setisStreaming(false);
    }
    // is not streaming => request streaming
    else
    {
        //Q_EMIT commandAskedToHost("START_STREAMING", _hostModel->name(), "");
        setisStreaming(true);
    }
}
