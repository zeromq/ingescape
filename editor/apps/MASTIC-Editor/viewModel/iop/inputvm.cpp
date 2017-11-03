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

#include "inputvm.h"
#include <viewModel/iop/outputvm.h>

InputVM::InputVM(QString inputName,
                 AgentIOPM* modelM,
                 QObject *parent) : PointMapVM(inputName,
                                               parent),
    _modelM(NULL)
{
    // Force ownership of our object, it will prevent Qml from stealing it
    QQmlEngine::setObjectOwnership(this, QQmlEngine::CppOwnership);

    setmodelM(modelM);

    if (_modelM != NULL) {
        qInfo() << "New Input VM" << "." << _modelM->name();
    }
}


/**
 * @brief Destructor
 */
InputVM::~InputVM()
{
    if (_modelM != NULL) {
        qInfo() << "Delete Input VM" << "." << _modelM->name();
    }

    setmodelM(NULL);
}


/**
 * @brief Return true if our input can link with the output (types are compatible)
 * @param pointMap
 * @return
 */
bool InputVM::canLinkWith(PointMapVM* pointMap)
{
    OutputVM* output = qobject_cast<OutputVM*>(pointMap);
    if ((output != NULL) && (output->modelM() != NULL)
            && (_modelM != NULL))
    {
        // Call parent class function
        return _canLinkOutputToInput(output->modelM()->agentIOPValueType(), _modelM->agentIOPValueType());
    }
    else {
        return false;
    }
}
