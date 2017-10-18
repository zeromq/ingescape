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
 *
 */

#include "definitionm.h"

/**
 * @brief Constructor
 * @param name
 * @param version
 * @param description
 * @param parent
 */
DefinitionM::DefinitionM(QString name,
                         QString version,
                         QString description,
                         QObject *parent) : QObject(parent),
    _name(name),
    _version(version),
    _description(description),
    _md5Hash(""),
    _isVariant(false)
{
    // Force ownership of our object, it will prevent Qml from stealing it
    QQmlEngine::setObjectOwnership(this, QQmlEngine::CppOwnership);

    qInfo() << "New Model of Agent Definition" << _name << "with version" << _version << "about" << _description;

    // Connect to signal "Count Changed" from the list of outputs
    connect(&_outputsList, &AbstractI2CustomItemListModel::countChanged, this, &DefinitionM::_onOutputsListChanged);
}


/**
 * @brief Destructor
 */
DefinitionM::~DefinitionM()
{
    qInfo() << "Delete Model of Agent Definition" << _name;

    // DIS-connect from signal "Count Changed" from the list of outputs
    disconnect(&_outputsList, &AbstractI2CustomItemListModel::countChanged, this, &DefinitionM::_onOutputsListChanged);

    // Delete all models of Inputs, Outputs and Parameters
    _inputsList.deleteAllItems();
    _outputsList.deleteAllItems();
    _parametersList.deleteAllItems();
}


/**
 * @brief Slot when the list of outputs changed
 */
void DefinitionM::_onOutputsListChanged()
{
    QList<OutputM*> newOutputsList = _outputsList.toList();

    // Output added
    if (_previousOutputsList.count() < newOutputsList.count())
    {
        //qDebug() << _previousOutputsList.count() << "--> ADD --> " << newOutputsList.count();

        for (OutputM* output : newOutputsList) {
            if ((output != NULL) && !_previousOutputsList.contains(output))
            {
                // Connect to signals from the output
                connect(output, &OutputM::commandAsked, this, &DefinitionM::commandAsked);
                //connect(output, &OutputM::isMutedChanged, this, &DefinitionM::_onIsMutedChanged);
            }
        }
    }
    // Output removed
    else if (_previousOutputsList.count() > newOutputsList.count())
    {
        //qDebug() << _previousOutputsList.count() << "--> REMOVE --> " << newOutputsList.count();

        for (OutputM* output : _previousOutputsList) {
            if ((output != NULL) && !newOutputsList.contains(output))
            {
                // DIS-connect from signals from the output
                disconnect(output, &OutputM::commandAsked, this, &DefinitionM::commandAsked);
                //disconnect(output, &OutputM::isMutedChanged, this, &DefinitionM::_onIsMutedChanged);
            }
        }
    }

    _previousOutputsList = newOutputsList;
}


/**
 * @brief Slot when the flag "Is Muted" of an output changed
 * @param isMuted
 */
/*void DefinitionM::_onIsMutedChanged(bool isMuted)
{
}*/
