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
    _isVariant(false)
{
    // Force ownership of our object, it will prevent Qml from stealing it
    QQmlEngine::setObjectOwnership(this, QQmlEngine::CppOwnership);

    qInfo() << "New Model of Agent Definition" << _name << "with version" << _version << "about" << _description;

    // Connect to signal "Count Changed" from the list of outputs
    connect(&_inputsList, &AbstractI2CustomItemListModel::countChanged, this, &DefinitionM::_onInputsListChanged);
    connect(&_outputsList, &AbstractI2CustomItemListModel::countChanged, this, &DefinitionM::_onOutputsListChanged);
    connect(&_parametersList, &AbstractI2CustomItemListModel::countChanged, this, &DefinitionM::_onParametersListChanged);
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
 * @brief Set the flag "is Muted" of an Output of our agent definition
 * @param isMuted
 * @param outputName
 */
void DefinitionM::setisMutedOfOutput(bool isMuted, QString outputName)
{
    OutputM* output = getOutputWithName(outputName);
    if (output != NULL) {
        output->setisMuted(isMuted);
    }
}


/**
 * @brief Slot when the list of inputs changed
 */
void DefinitionM::_onInputsListChanged()
{
    QList<AgentIOPM*> newInputsList = _inputsList.toList();

    // Input added
    if (_previousInputsList.count() < newInputsList.count())
    {
        for (AgentIOPM* input : newInputsList) {
            if ((input != NULL) && !_previousInputsList.contains(input))
            {
                // Add its id
                _inputsIdsList.append(input->id());
            }
        }
    }
    // Input removed
    else if (_previousInputsList.count() > newInputsList.count())
    {
        for (AgentIOPM* input : _previousInputsList) {
            if ((input != NULL) && !newInputsList.contains(input))
            {
                // Remove its id
                _inputsIdsList.removeOne(input->id());
            }
        }
    }

    _previousInputsList = newInputsList;

    //qDebug() << "Definition" << _name << "has inputs:" << _inputsIdsList;
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
        for (OutputM* output : newOutputsList) {
            if ((output != NULL) && !_previousOutputsList.contains(output))
            {
                // Add its id
                _outputsIdsList.append(output->id());

                // Connect to signals from the output
                connect(output, &OutputM::commandAsked, this, &DefinitionM::commandAskedForOutput);
                //connect(output, &OutputM::isMutedChanged, this, &DefinitionM::_onIsMutedChanged);
            }
        }
    }
    // Output removed
    else if (_previousOutputsList.count() > newOutputsList.count())
    {
        for (OutputM* output : _previousOutputsList) {
            if ((output != NULL) && !newOutputsList.contains(output))
            {
                // Remove its id
                _outputsIdsList.removeOne(output->id());

                // DIS-connect from signals from the output
                disconnect(output, &OutputM::commandAsked, this, &DefinitionM::commandAskedForOutput);
                //disconnect(output, &OutputM::isMutedChanged, this, &DefinitionM::_onIsMutedChanged);
            }
        }
    }

    _previousOutputsList = newOutputsList;

    //qDebug() << "Definition" << _name << "has outputs:" << _outputsIdsList;
}


/**
 * @brief Slot when the list of parameters changed
 */
void DefinitionM::_onParametersListChanged()
{
    QList<AgentIOPM*> newParametersList = _parametersList.toList();

    // Parameter added
    if (_previousParametersList.count() < newParametersList.count())
    {
        for (AgentIOPM* parameter : newParametersList) {
            if ((parameter != NULL) && !_previousParametersList.contains(parameter))
            {
                // Add its id
                _parametersIdsList.append(parameter->id());
            }
        }
    }
    // Parameter removed
    else if (_previousParametersList.count() > newParametersList.count())
    {
        for (AgentIOPM* parameter : _previousParametersList) {
            if ((parameter != NULL) && !newParametersList.contains(parameter))
            {
                // Remove its id
                _parametersIdsList.removeOne(parameter->id());
            }
        }
    }

    _previousParametersList = newParametersList;

    //qDebug() << "Definition" << _name << "has parameters:" << _parametersIdsList;
}


/**
 * @brief Slot when the flag "is Muted" of an output changed
 * @param isMuted
 */
/*void DefinitionM::_onIsMutedChanged(bool isMuted)
{
}*/


/**
 * @brief Return true if the 2 definitions are strictly identicals
 * @param definition1
 * @param definition2
 * @return
 */
bool DefinitionM::areIdenticals(DefinitionM* definition1, DefinitionM* definition2)
{
    bool areIdenticals = false;

    if ((definition1 != NULL) && (definition2 != NULL))
    {
        // Same name and same version
        if ((definition1->name() == definition2->name())
                && (definition1->version() == definition2->version()))
        {
            // same number of inputs, outputs and parameters
            if ((definition1->inputsIdsList().count() == definition2->inputsIdsList().count())
                    && (definition1->outputsIdsList().count() == definition2->outputsIdsList().count())
                    && (definition1->parametersIdsList().count() == definition2->parametersIdsList().count()))
            {
                // check each id for inputs, outputs and parameters
                if (DefinitionM::_areIdenticalsIdsList(definition1->inputsIdsList(), definition2->inputsIdsList())
                        && DefinitionM::_areIdenticalsIdsList(definition1->outputsIdsList(), definition2->outputsIdsList())
                        && DefinitionM::_areIdenticalsIdsList(definition1->parametersIdsList(), definition2->parametersIdsList()))
                {
                    areIdenticals = true;
                }
            }
        }
    }

    return areIdenticals;
}


/**
 * @brief Get an Input with its name
 * @param name
 * @return
 */
AgentIOPM* DefinitionM::getInputWithName(QString name)
{
    foreach (AgentIOPM* input, _inputsList.toList()) {
        if ((input != NULL) && (input->name() == name)) {
            return input;
        }
    }
    return NULL;
}


/**
 * @brief Get an Output with its name
 * @param name
 * @return
 */
OutputM* DefinitionM::getOutputWithName(QString name)
{
    foreach (OutputM* output, _outputsList.toList()) {
        if ((output != NULL) && (output->name() == name)) {
            return output;
        }
    }
    return NULL;
}


/**
 * @brief Get a Parameter with its name
 * @param name
 * @return
 */
AgentIOPM* DefinitionM::getParameterWithName(QString name)
{
    foreach (AgentIOPM* parameter, _parametersList.toList()) {
        if ((parameter != NULL) && (parameter->name() == name)) {
            return parameter;
        }
    }
    return NULL;
}


/**
 * @brief Return true if the 2 list of ids are strictly identicals
 * @param idsList1
 * @param idsList2
 * @return
 */
bool DefinitionM::_areIdenticalsIdsList(QStringList idsList1, QStringList idsList2)
{
    // Same numbers of ids
    if (idsList1.count() == idsList2.count())
    {
        foreach (QString id, idsList1)
        {
            // id of list 1 is not in the list 2
            if (!idsList2.contains(id)) {
                return false;
            }
        }
        // All ids of list 1 are inside list 2
        return true;
    }
    // Numbers of ids in lists are different
    else {
        return false;
    }
}
