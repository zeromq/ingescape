/*
 *	MASTIC Editor
 *
 *  Copyright © 2017 Ingenuity i/o. All rights reserved.
 *
 *	See license terms for the rights and conditions
 *	defined by copyright holders.
 *
 *
 *	Contributors:
 *      Bruno Lemenicier <lemenicier@ingenuity.io>
 *
 */

#include "hostvm.h"

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
    if (model != NULL)
    {
        // Init model
        _hostModel = model;

        if(_hostModel->streamingPort().length() > 0)
            _canProvideStream = true;
    }
}

/**
 * @brief Destructor
 */
HostVM::~HostVM()
{
    qInfo() << "Delete View Model of Host" << _hostModel->name();

}

/**
 * @brief Change the state of our agent
 */
void HostVM::changeState()
{
    // is streaming => request streaming end
    if (_isStreaming) {
        //Q_EMIT commandAskedToHost("STOP_STREAMING", _hostModel->name(), "");
        setisStreaming(false);
    }
    // is not streaming => request streaming
    else {
        //Q_EMIT commandAskedToHost("START_STREAMING", _hostModel->name(), "");
        setisStreaming(true);
    }
}
