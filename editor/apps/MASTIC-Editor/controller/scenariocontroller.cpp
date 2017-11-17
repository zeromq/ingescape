/*
 *	ScenarioController
 *
 *  Copyright (c) 2016-2017 Ingenuity i/o. All rights reserved.
 *
 *	See license terms for the rights and conditions
 *	defined by copyright holders.
 *
 *
 *	Contributors:
 *
 */

#include "scenariocontroller.h"


#include <QDebug>
#include <QFileDialog>



//--------------------------------------------------------------
//
// ScenarioController
//
//--------------------------------------------------------------


/**
 * @brief Default constructor
 * @param scenarios files path
 * @param parent
 */
ScenarioController::ScenarioController(QString scenariosPath, QObject *parent) : QObject(parent),
    _selectedAction(NULL),
    _scenariosDirectoryPath(scenariosPath)
{
    // Force ownership of our object, it will prevent Qml from stealing it
    QQmlEngine::setObjectOwnership(this, QQmlEngine::CppOwnership);

    // Fill state comparisons types list
    _comparisonsAgentsTypesList.appendEnumValue(ActionComparisonValueType::ON);
    _comparisonsAgentsTypesList.appendEnumValue(ActionComparisonValueType::OFF);

    // Fill value comparisons types list
    _comparisonsValuesTypesList.fillWithAllEnumValues();
    _comparisonsValuesTypesList.removeEnumValue(ActionComparisonValueType::ON);
    _comparisonsValuesTypesList.removeEnumValue(ActionComparisonValueType::OFF);

    // Fill value effects types list
    _effectsAgentsTypesList.appendEnumValue(ActionEffectValueType::ON);
    _effectsAgentsTypesList.appendEnumValue(ActionEffectValueType::OFF);

    // Fill link effects types list
    _effectsLinksTypesList.appendEnumValue(ActionEffectValueType::ENABLE);
    _effectsLinksTypesList.appendEnumValue(ActionEffectValueType::DISABLE);

    // Fill general types
    _conditionsTypesList.fillWithAllEnumValues();
    _effectsTypesList.fillWithAllEnumValues();

    // Fill validity duration types list
    _validationDurationsTypesList.fillWithAllEnumValues();

    // Initialize the 9 items of the palette with NULL action
    _actionsInPaletteList.append(new ActionInPaletteVM(NULL,0));
    _actionsInPaletteList.append(new ActionInPaletteVM(NULL,1));
    _actionsInPaletteList.append(new ActionInPaletteVM(NULL,2));
    _actionsInPaletteList.append(new ActionInPaletteVM(NULL,3));
    _actionsInPaletteList.append(new ActionInPaletteVM(NULL,4));
    _actionsInPaletteList.append(new ActionInPaletteVM(NULL,5));
    _actionsInPaletteList.append(new ActionInPaletteVM(NULL,6));
    _actionsInPaletteList.append(new ActionInPaletteVM(NULL,7));
    _actionsInPaletteList.append(new ActionInPaletteVM(NULL,8));
    // FIXME: instead ?
    /*for (int i = 0; i < 9; i++) {
        _actionsInPaletteList.append(new ActionInPaletteVM(NULL, i));
    }*/

    QDate today = QDate::currentDate();
    _scenariosDefaultFilePath = QString("%1scenarios_%2.json").arg(_scenariosDirectoryPath, today.toString("ddMMyy"));

    // Create the helper to manage JSON definitions of agents
    _jsonHelper = new JsonHelper(this);

}


/**
 * @brief Destructor
 */
ScenarioController::~ScenarioController()
{
    // Clean-up current selection
    setselectedAction(NULL);

    // Delete actions VM from the timeline
    _actionsInTimeLine.deleteAllItems();

    // Delete actions VM from the palette
    _actionsInPaletteList.deleteAllItems();

    // Clear the list of editor opened
    _mapActionsEditorControllersFromActionVM.clear();
    _openedActionsEditorsControllers.deleteAllItems();

    // Delete actions Vm List
    _actionsList.deleteAllItems();

    // Clear map
    _mapActionsFromActionName.clear();
}


/**
  * @brief Open the action editor
  * @param action view model
  */
void ScenarioController::openActionEditor(ActionM* actionM)
{
    // We check that or editor is not already opened
    if(_mapActionsEditorControllersFromActionVM.contains(actionM) == false)
    {
        // Create an empty action if we create a new one
        if(actionM != NULL)
        {
            setselectedAction(actionM);

            // Create action editor controller
            ActionEditorController* actionEditorC = new ActionEditorController(_buildNewActionName(), actionM,agentsInMappingList());

            // Add action into our opened actions
            _mapActionsEditorControllersFromActionVM.insert(actionM,actionEditorC);

            // Add to list
            _openedActionsEditorsControllers.append(actionEditorC);
        }
        // Set selected action
        else
        {
            // Create action editor controller
            ActionEditorController* actionEditorC = new ActionEditorController(_buildNewActionName(), actionM ,agentsInMappingList());

            // Add action into our opened actions
            _mapActionsEditorControllersFromActionVM.insert(actionEditorC->editedAction(),actionEditorC);

            // Add to list
            _openedActionsEditorsControllers.append(actionEditorC);
        }
    }
}


/**
  * @brief Delete an action from the list
  * @param action view model
  */
void ScenarioController::deleteAction(ActionM * actionM)
{
    // Delete the popup if necessary
    if(actionM != NULL && _mapActionsEditorControllersFromActionVM.contains(actionM))
    {
        ActionEditorController* actionEditorC = _mapActionsEditorControllersFromActionVM.value(actionM);

        _mapActionsEditorControllersFromActionVM.remove(actionM);
        _openedActionsEditorsControllers.remove(actionEditorC);
    }

    // Unselect our action if needed
    if (_selectedAction == actionM) {
        setselectedAction(NULL);
    }

    // Delete the action item
    if(_actionsList.contains(actionM))
    {
        _actionsList.remove(actionM);

        _mapActionsFromActionName.remove(actionM->name());

        delete actionM;
        actionM = NULL;
    }
}

/**
  * @brief Valide action edition
  * @param action editor controller
  */
void ScenarioController::valideActionEditor(ActionEditorController* actionEditorC)
{
    // Valide modification
    actionEditorC->validateModification();

    ActionM* originalActionVM = actionEditorC->originalAction();

    // We check that or editor is not already opened
    if(_actionsList.contains(originalActionVM) == false)
    {
        // Insert in to the list
        _actionsList.append(originalActionVM);

        // Insert into the map
        _mapActionsFromActionName.insert(originalActionVM->name(),originalActionVM);
    }

    // Set selected action
    setselectedAction(originalActionVM);
}

/**
  * @brief Close action edition
  * @param action editor controller
  */
void ScenarioController::closeActionEditor(ActionEditorController* actionEditorC)
{
    ActionM* actionM = actionEditorC->originalAction() != NULL ? actionEditorC->originalAction() : actionEditorC->editedAction();
    // Delete the popup if necessary
    if(actionM != NULL && _mapActionsEditorControllersFromActionVM.contains(actionM))
    {
        ActionEditorController* actionEditorC = _mapActionsEditorControllersFromActionVM.value(actionM);

        _mapActionsEditorControllersFromActionVM.remove(actionM);
        _openedActionsEditorsControllers.remove(actionEditorC);
    }
}

/**
  * @brief slot on agents in mapping list count change
  */
void ScenarioController::onAgentsInMappingListCountChange()
{
    I2CustomItemListModel<AgentInMappingVM> * agentInMappingList = dynamic_cast<I2CustomItemListModel<AgentInMappingVM> *>(sender());
    if (agentInMappingList != NULL)
    {
        // Reset the agents in mapping list
        _agentsInMappingList.clear();

        // Add the new list of agents
        _agentsInMappingList.append(agentInMappingList->toList());
    }
}

/**
 * @brief Get a new action name
 */
QString ScenarioController::_buildNewActionName()
{
    // Remove the effect
    int index = 1;
    QString tmpName = "Action_"+QString("%1").arg(index, 3,10, QChar('0'));

    while(_mapActionsFromActionName.contains(tmpName))
    {
        index++;
        tmpName = "Action_"+QString("%1").arg(index, 3, 10, QChar('0'));
    }

    return tmpName;
}

/**
 * @brief Set an action into the palette at index
 * @param index where to insert the action
 * @param action to insert
 */
void ScenarioController::setActionInPalette(int index, ActionM* actionM)
{
    // Set action in palette
    if(index < _actionsInPaletteList.count())
    {
        _actionsInPaletteList.at(index)->setactionModel(actionM);
    }
}

/**
 * @brief Import a scenario a file (actions, palette, timeline actions )
 */
void ScenarioController::importScenarioFromFile()
{
    // "File Dialog" to get the files (paths) to open
    QString scenarioFilePath = QFileDialog::getOpenFileName(NULL,
                                                                "Importer un fichier scénario",
                                                                _scenariosDirectoryPath,
                                                                "JSON (*.json)");

    // Import the scenario from JSON file
    _importScenarioFromFile(scenarioFilePath);
}

/**
 * @brief Import the scenario from JSON file
 * @param scenarioFilePath
 */
void ScenarioController::_importScenarioFromFile(QString scenarioFilePath)
{
    if (!scenarioFilePath.isEmpty() && (_jsonHelper != NULL))
    {
        qInfo() << "Import the scenario from JSON file" << scenarioFilePath;

        QFile jsonFile(scenarioFilePath);
        if (jsonFile.exists())
        {
            if (jsonFile.open(QIODevice::ReadOnly))
            {
                QByteArray byteArrayOfJson = jsonFile.readAll();
                jsonFile.close();

                // Initialize agents list from JSON file
                QPair< QPair< QList<ActionM*>, QList<ActionInPaletteVM*> > , QList<ActionVM*> > scenarioToImport = _jsonHelper->initActionsList(byteArrayOfJson, _agentsInMappingList.toList());

                // Append the list of actions
                if(scenarioToImport.first.first.count() > 0)
                {
                    _actionsList.append(scenarioToImport.first.first);
                }

                // Set the list of actions in palette
                if(scenarioToImport.first.second.count() > 0)
                {
                    foreach (ActionInPaletteVM* actionInPalette, scenarioToImport.first.second)
                    {
                        if(actionInPalette->actionModel() != NULL)
                        {
                            setActionInPalette(actionInPalette->indexInPanel(),actionInPalette->actionModel());
                        }

                        delete actionInPalette;
                        actionInPalette = NULL;
                    }
                }

                // Append the list of actions in timeline
                if(scenarioToImport.second.count() > 0)
                {
                    _actionsInTimeLine.append(scenarioToImport.second);
                }
            }
            else {
                qCritical() << "Can not open file" << scenarioFilePath;
            }
        }
        else {
            qWarning() << "There is no file" << scenarioFilePath;
        }
    }

}

/**
 * @brief Export a scenario to the default file (actions, palette, timeline actions)
 */
void ScenarioController::exportScenarioToSelectedFile()
{
    // "File Dialog" to get the file (path) to save
    QString scenarioFilePath = QFileDialog::getSaveFileName(NULL,
                                                              "Sauvegarder dans un fichier JSON e scenario",
                                                              _scenariosDirectoryPath,
                                                              "JSON (*.json)");

    if(!scenarioFilePath.isEmpty()) {
        // Export the scenario to JSON file
        _exportScenarioToFile(scenarioFilePath);
    }
}

/**
 * @brief Export the scenario to JSON file
 * @param scenarioFilePath
 */
void ScenarioController::_exportScenarioToFile(QString scenarioFilePath)
{
    if (!scenarioFilePath.isEmpty() && (_jsonHelper != NULL))
    {
        qInfo() << "Export the scenario to JSON file" << scenarioFilePath;

        // Export the scenario
        QByteArray byteArrayOfJson = _jsonHelper->exportScenario(_actionsList.toList(),_actionsInPaletteList.toList(),_actionsInTimeLine.toList());

        QFile jsonFile(scenarioFilePath);
        if (jsonFile.open(QIODevice::WriteOnly))
        {
            jsonFile.write(byteArrayOfJson);
            jsonFile.close();
        }
        else {
            qCritical() << "Can not open file" << scenarioFilePath;
        }
    }

}

