/*
 *	IngeScape Common
 *
 *  Copyright © 2017-2019 Ingenuity i/o. All rights reserved.
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

    //qInfo() << "New Model of Agent Definition" << _name << "with version" << _version << "about" << _description;

    // Connect to signal "Count Changed" from the list of Inputs/Outputs/Parameters
    connect(&_inputsList, &AbstractI2CustomItemListModel::countChanged, this, &DefinitionM::_onInputsListChanged);
    connect(&_outputsList, &AbstractI2CustomItemListModel::countChanged, this, &DefinitionM::_onOutputsListChanged);
    connect(&_parametersList, &AbstractI2CustomItemListModel::countChanged, this, &DefinitionM::_onParametersListChanged);
}


/**
 * @brief Destructor
 */
DefinitionM::~DefinitionM()
{
    //qInfo() << "Delete Model of Agent Definition" << _name;

    // DIS-connect from signal "Count Changed" from the list of Inputs/Outputs/Parameters
    disconnect(&_inputsList, &AbstractI2CustomItemListModel::countChanged, this, &DefinitionM::_onInputsListChanged);
    disconnect(&_outputsList, &AbstractI2CustomItemListModel::countChanged, this, &DefinitionM::_onOutputsListChanged);
    disconnect(&_parametersList, &AbstractI2CustomItemListModel::countChanged, this, &DefinitionM::_onParametersListChanged);

    // Clear maps
    _mapFromInputNameToInput.clear();
    _mapFromOutputNameToOutput.clear();
    _mapFromParameterNameToParameter.clear();

    // Delete all models of Inputs, Outputs and Parameters
    _previousInputsList.clear();
    _inputsList.deleteAllItems();

    _previousOutputsList.clear();
    _outputsList.deleteAllItems();

    _previousParametersList.clear();
    _parametersList.deleteAllItems();

    // Delete calls
    _callsList.deleteAllItems();
}


/**
 * @brief Set the flag "is Muted Output" of an output of our agent definition
 * @param isMuted
 * @param outputName
 */
void DefinitionM::setisMutedOutput(bool isMuted, QString outputName)
{
    OutputM* output = getOutputWithName(outputName);
    if (output != nullptr) {
        output->setisMutedOutput(isMuted);
    }
}


/**
 * @brief Get the flag "is Muted Output" of an output of our agent definition
 * @param outputName
 */
bool DefinitionM::getIsMutedOutput(QString outputName)
{
    OutputM* output = getOutputWithName(outputName);
    return output != nullptr && output->isMutedOutput();
}


/**
 * @brief Set the flag "is Muted" of all Outputs of our agent definition
 * @param isMuted
 */
void DefinitionM::setisMutedOfAllOutputs(bool isMuted)
{
    for (OutputM* output : _outputsList)
    {
        if (output != nullptr) {
            output->setisMutedAllOutputs(isMuted);
        }
    }
}


/**
 * @brief Slot when the list of inputs changed
 */
void DefinitionM::_onInputsListChanged()
{
    const QList<AgentIOPM*>& newInputsList = _inputsList.toList();

    // Input added
    if (_previousInputsList.count() < newInputsList.count())
    {
        for (AgentIOPM* input : newInputsList)
        {
            if ((input != nullptr) && !_previousInputsList.contains(input))
            {
                _inputsIdsList.append(input->uid());
                _mapFromInputNameToInput.insert(input->name(), input);
            }
        }
    }
    // Input removed
    else if (_previousInputsList.count() > newInputsList.count())
    {
        for (AgentIOPM* input : _previousInputsList)
        {
            if ((input != nullptr) && !newInputsList.contains(input))
            {
                _inputsIdsList.removeOne(input->uid());
                _mapFromInputNameToInput.remove(input->name());
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
    const QList<OutputM*>& newOutputsList = _outputsList.toList();

    // Output added
    if (_previousOutputsList.count() < newOutputsList.count())
    {
        for (OutputM* output : newOutputsList)
        {
            if ((output != nullptr) && !_previousOutputsList.contains(output))
            {
                _outputsIdsList.append(output->uid());
                _mapFromOutputNameToOutput.insert(output->name(), output);

                // Connect to signals from the output
                connect(output, &OutputM::commandAsked, this, &DefinitionM::commandAskedForOutput);
                connect(output, &OutputM::isMutedOutputChanged, this, &DefinitionM::_onIsMutedOutputChanged);
            }
        }
    }
    // Output removed
    else if (_previousOutputsList.count() > newOutputsList.count())
    {
        for (OutputM* output : _previousOutputsList)
        {
            if ((output != nullptr) && !newOutputsList.contains(output))
            {
                _outputsIdsList.removeOne(output->uid());
                _mapFromOutputNameToOutput.remove(output->name());

                // DIS-connect to signals from the output
                disconnect(output, nullptr, this, nullptr);
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
        for (AgentIOPM* parameter : newParametersList)
        {
            if ((parameter != nullptr) && !_previousParametersList.contains(parameter))
            {
                _parametersIdsList.append(parameter->uid());
                _mapFromParameterNameToParameter.insert(parameter->name(), parameter);
            }
        }
    }
    // Parameter removed
    else if (_previousParametersList.count() > newParametersList.count())
    {
        for (AgentIOPM* parameter : _previousParametersList)
        {
            if ((parameter != nullptr) && !newParametersList.contains(parameter))
            {
                _parametersIdsList.removeOne(parameter->uid());
                _mapFromParameterNameToParameter.remove(parameter->name());
            }
        }
    }

    _previousParametersList = newParametersList;

    //qDebug() << "Definition" << _name << "has parameters:" << _parametersIdsList;
}


/**
 * @brief Slot called when the flag "is Muted Output" of an output changed
 * @param isMutedOutput
 */
void DefinitionM::_onIsMutedOutputChanged(bool isMutedOutput)
{
    OutputM* output = qobject_cast<OutputM*>(sender());
    if (output != nullptr) {
        Q_EMIT isMutedOutputChanged(isMutedOutput, output->name());
    }
}


/**
 * @brief Get an Input with its name
 * @param inputName
 * @return
 */
AgentIOPM* DefinitionM::getInputWithName(QString inputName)
{
    if (_mapFromInputNameToInput.contains(inputName)) {
        return _mapFromInputNameToInput.value(inputName);
    }
    else {
        return nullptr;
    }
}


/**
 * @brief Get an Output with its name
 * @param outputName
 * @return
 */
OutputM* DefinitionM::getOutputWithName(QString outputName)
{
    if (_mapFromOutputNameToOutput.contains(outputName)) {
        return _mapFromOutputNameToOutput.value(outputName);
    }
    else {
        return nullptr;
    }
}


/**
 * @brief Get a Parameter with its name
 * @param parameterName
 * @return
 */
AgentIOPM* DefinitionM::getParameterWithName(QString parameterName)
{
    if (_mapFromParameterNameToParameter.contains(parameterName)) {
        return _mapFromParameterNameToParameter.value(parameterName);
    }
    else {
        return nullptr;
    }
}

/**
 * @brief Accessor for the list of calls.
 * The returned value is a const ref so the calls list cannot be modified from here.
 * @return
 */
QList<CallM*> DefinitionM::getCallsList() const
{
    return _callsList.toList();
}


/**
 * @brief Update the current value of an I/O/P of our agent definition
 * @param iopType
 * @param iopName
 * @param value
 */
void DefinitionM::updateCurrentValueOfIOP(AgentIOPTypes::Value iopType, QString iopName, QVariant value)
{
    switch (iopType)
    {
    case AgentIOPTypes::OUTPUT: {
        OutputM* output = getOutputWithName(iopName);
        if (output != nullptr) {
            output->setcurrentValue(value);
        }
        break;
    }
    case AgentIOPTypes::INPUT: {
        AgentIOPM* input = getInputWithName(iopName);
        if (input != nullptr) {
            input->setcurrentValue(value);
        }
        break;
    }
    case AgentIOPTypes::PARAMETER: {
        AgentIOPM* parameter = getParameterWithName(iopName);
        if (parameter != nullptr) {
            parameter->setcurrentValue(value);
        }
        break;
    }
    default:
        break;
    }
}


/**
 * @brief Make a copy of our definition
 * @return
 */
DefinitionM* DefinitionM::copy()
{
    DefinitionM* copy = new DefinitionM(_name, _version, _description);

    //
    // Inputs
    //
    QList<AgentIOPM*> copiesOfInputs;
    for (AgentIOPM* iterator : _inputsList)
    {
        if (iterator != nullptr)
        {
            AgentIOPM* copyOfInput = new AgentIOPM(iterator->agentIOPType(),
                                                   iterator->name(),
                                                   iterator->agentIOPValueType());

            copyOfInput->setdefaultValue(iterator->defaultValue());

            copiesOfInputs.append(copyOfInput);
        }
    }
    copy->inputsList()->append(copiesOfInputs);


    //
    // Outputs
    //
    QList<OutputM*> copiesOfOutputs;
    for (OutputM* iterator : _outputsList)
    {
        if (iterator != nullptr)
        {
            OutputM* copyOfOutput = new OutputM(iterator->name(),
                                                iterator->agentIOPValueType());

            copyOfOutput->setdefaultValue(iterator->defaultValue());

            copiesOfOutputs.append(copyOfOutput);
        }
    }
    copy->outputsList()->append(copiesOfOutputs);


    //
    // Parameters
    //
    QList<AgentIOPM*> copiesOfParameters;
    for (AgentIOPM* iterator : _parametersList)
    {
        if (iterator != nullptr)
        {
            AgentIOPM* copyOfParameter = new AgentIOPM(iterator->agentIOPType(),
                                                       iterator->name(),
                                                       iterator->agentIOPValueType());

            copyOfParameter->setdefaultValue(iterator->defaultValue());

            copiesOfParameters.append(copyOfParameter);
        }
    }
    copy->parametersList()->append(copiesOfParameters);


    //
    // Calls
    //
    QList<CallM*> copiesOfCalls;
    for (CallM* iterator : _callsList)
    {
        if (iterator != nullptr)
        {
            CallM* copyOfReply = nullptr;
            if (iterator->reply() != nullptr)
            {
                copyOfReply = new CallM(iterator->reply()->name()
                                        , iterator->reply()->description()
                                        , iterator->reply()->arguments()
                                        , nullptr);
            }

            CallM* copyOfCall = new CallM(iterator->name()
                                          , iterator->description()
                                          , iterator->arguments()
                                          , copyOfReply);

            copiesOfCalls.append(copyOfCall);
        }
    }
    copy->callsList()->append(copiesOfCalls);

    return copy;
}

/**
 * @brief Equality operator to compare two definition
 * @param left
 * @param right
 * @return
 */
bool operator==(const DefinitionM& left, const DefinitionM& right)
{
    // Lambda comparing unsorted QStringLists
    auto listsAreEqual = [](QStringList idsList1, QStringList idsList2) {
        idsList1.sort();
        idsList2.sort();
        return idsList1 == idsList2;
    };

    return left.name() == right.name()
            && left.version() == right.version()
            && listsAreEqual(left.inputsIdsList(), right.inputsIdsList())
            && listsAreEqual(left.outputsIdsList(), right.outputsIdsList())
            && listsAreEqual(left.parametersIdsList(), right.parametersIdsList())
            && left.getCallsList() == right.getCallsList();
}

/**
 * @brief Difference operator to compare two definition
 * Simply the negation of the equality operator
 * @param left
 * @param right
 * @return
 */
bool operator!=(const DefinitionM& left, const DefinitionM& right)
{
     return !(left == right);
}
