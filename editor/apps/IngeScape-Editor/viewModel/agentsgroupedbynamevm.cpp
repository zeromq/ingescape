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

#include "agentsgroupedbynamevm.h"

/**
 * @brief Constructor
 * @param model
 * @param parent
 */
AgentsGroupedByNameVM::AgentsGroupedByNameVM(AgentM* model,
                                             QObject *parent) : QObject(parent)
{
    // Force ownership of our object, it will prevent Qml from stealing it
    QQmlEngine::setObjectOwnership(this, QQmlEngine::CppOwnership);

    if (model != NULL)
    {
        // Init the name
        _name = model->name();

        qInfo() << "New View Model of Agents grouped by name" << _name;

        // Connect to signal "Count Changed" from the list of models
        connect(&_models, &AbstractI2CustomItemListModel::countChanged, this, &AgentsGroupedByNameVM::_onModelsChanged);

        // Add to the list
        //_models.append(model);

        // Manage when a model of agent entered on our network
        manageAgentEnteredNetwork(model);
    }
    else
    {
        qCritical() << "The model of agent is NULL !";
    }
}


/**
 * @brief Destructor
 */
AgentsGroupedByNameVM::~AgentsGroupedByNameVM()
{
    qInfo() << "Delete View Model of Agents grouped by name" << _name;

    disconnect(&_models, &AbstractI2CustomItemListModel::countChanged, this, &AgentsGroupedByNameVM::_onModelsChanged);

    // Clear the previous list of models
    _previousAgentsList.clear();

    // Free the memory elsewhere
    //_models.deleteAllItems();
    _models.clear();

    //_hashFromHostnameToModels.clear();
}


/**
 * @brief Manage when a model of agent entered on our network
 * @param model
 */
void AgentsGroupedByNameVM::manageAgentEnteredNetwork(AgentM* model)
{
    if (model != NULL)
    {
        qDebug() << "Grouped by" << _name << "manage" << model << "entered";

        // FIXME: depends of cases (definition, ...)
        _models.append(model);
    }
}


/**
 * @brief Manage when a model of agent exited from our network
 * @param model
 */
void AgentsGroupedByNameVM::manageAgentExitedNetwork(AgentM* model)
{
    if (model != NULL)
    {
        qDebug() << "Grouped by" << _name << "manage" << model << "exited";
    }
}


/**
 * @brief Update the current value of an I/O/P of our agent(s)
 * @param publishedValue
 */
void AgentsGroupedByNameVM::updateCurrentValueOfIOP(PublishedValueM* publishedValue)
{
    if (publishedValue != NULL)
    {
        for (AgentM* agent : _models)
        {
            if ((agent != NULL) && (agent->definition() != NULL))
            {
                switch (publishedValue->iopType())
                {
                case AgentIOPTypes::OUTPUT: {
                    OutputM* output = agent->definition()->getOutputWithName(publishedValue->iopName());
                    if (output != NULL) {
                        output->setcurrentValue(publishedValue->value());
                    }
                    break;
                }
                case AgentIOPTypes::INPUT: {
                    AgentIOPM* input = agent->definition()->getInputWithName(publishedValue->iopName());
                    if (input != NULL) {
                        input->setcurrentValue(publishedValue->value());
                    }
                    break;
                }
                case AgentIOPTypes::PARAMETER: {
                    AgentIOPM* parameter = agent->definition()->getParameterWithName(publishedValue->iopName());
                    if (parameter != NULL) {
                        parameter->setcurrentValue(publishedValue->value());
                    }
                    break;
                }
                default:
                    break;
                }
            }
        }
    }
}


/**
 * @brief Slot called when the list of models changed
 */
void AgentsGroupedByNameVM::_onModelsChanged()
{
    QList<AgentM*> newAgentsList = _models.toList();

    // Model of agent added
    if (_previousAgentsList.count() < newAgentsList.count())
    {
        //qDebug() << _previousAgentsList.count() << "--> ADD --> " << newAgentsList.count();

        for (AgentM* model : newAgentsList)
        {
            if ((model != NULL) && !_previousAgentsList.contains(model))
            {
                //qDebug() << "New model" << model->name() << "ADDED (" << model->peerId() << ")";

                // Connect to signals of the model
                //connect(model, &AgentM::hostnameChanged, this, &AgentsGroupedByNameVM::_onHostnameOfModelChanged);
                connect(model, &AgentM::isONChanged, this, &AgentsGroupedByNameVM::_onIsONofModelChanged);

            }
        }
    }
    // Model of agent removed
    else if (_previousAgentsList.count() > newAgentsList.count())
    {
        //qDebug() << _previousAgentsList.count() << "--> REMOVE --> " << newAgentsList.count();

        for (AgentM* model : _previousAgentsList)
        {
            if ((model != NULL) && !newAgentsList.contains(model))
            {
                //qDebug() << "Old model" << model->name() << "REMOVED (" << model->peerId() << ")";

                // DIS-connect from signals of the model
                disconnect(model, 0, this, 0);
            }
        }
    }

    _previousAgentsList = newAgentsList;

    // Update with all models
    _updateWithAllModels();
}


/**
 * @brief Slot called when the flag "is ON" of a model changed
 * @param isON
 */
void AgentsGroupedByNameVM::_onIsONofModelChanged(bool isON)
{
    Q_UNUSED(isON)

    // Update the flag "is ON" in function of flags of all models
    _updateIsON();
}


/**
 * @brief Update with all models of agents
 */
void AgentsGroupedByNameVM::_updateWithAllModels()
{
    qDebug() << "Update with all models (" << _models.count() << ") of" << _name  << "(" << this << ")";

    // Update flags in function of models
    _updateIsON();
}


/**
 * @brief Update the flag "is ON" in function of flags of models
 */
void AgentsGroupedByNameVM::_updateIsON()
{
    bool globalIsON = false;

    for (AgentM* model : _models.toList())
    {
        if ((model != NULL) && model->isON())
        {
            globalIsON = true;
            break;
        }
    }

    setisON(globalIsON);
}
