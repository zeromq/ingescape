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
 *      Vincent Peyruqueou <peyruqueou@ingenuity.io>
 *      Alexandre Lemort   <lemort@ingenuity.io>
 *
 */

#include "outputvm.h"
#include <viewModel/iop/inputvm.h>

OutputVM::OutputVM(QString outputName,
                   OutputM* modelM,
                   QObject *parent) : PointMapVM(outputName,
                                                 parent),
    _modelM(NULL),
    _isGhost(false),
    _isPublishedNewValue(false)
{
    // Force ownership of our object, it will prevent Qml from stealing it
    QQmlEngine::setObjectOwnership(this, QQmlEngine::CppOwnership);

    // Allow to benefit of "DELETE PROOF"
    setmodelM(modelM);

    if (_modelM != NULL) {
        qInfo() << "New Output VM" << _modelM->name();
    }
    else {
        _isGhost = true;
    }
}


/**
 * @brief Destructor
 */
OutputVM::~OutputVM()
{
    if (_modelM != NULL) {
        qInfo() << "Delete Output VM" << _modelM->name();
    }

    setmodelM(NULL);
}


/**
 * @brief Return true if our output can link with the input (types are compatible)
 * @param pointMap
 * @return
 */
bool OutputVM::canLinkWith(PointMapVM* pointMap)
{
    InputVM* input = qobject_cast<InputVM*>(pointMap);
    if ((input != NULL) && (input->modelM() != NULL)
            && (_modelM != NULL))
    {
        // Call parent class function
        return _canLinkOutputToInput(_modelM->agentIOPValueType(), input->modelM()->agentIOPValueType());
    }
    else {
        return false;
    }
}
