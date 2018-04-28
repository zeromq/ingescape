/*
 *	IngeScape Editor
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

#include "actioneditorcontroller.h"

#include <QDebug>

/**
 * @brief Default constructor
 * @param parent
 */
ActionEditorController::ActionEditorController(QString actionName, ActionM *originalAction, I2CustomItemSortFilterListModel<AgentInMappingVM> *listAgentsInMapping, QObject *parent) : QObject(parent),
    _originalAction(originalAction),
    _editedAction(NULL),
    _originalViewModel(NULL),
    _editedViewModel(NULL),
    _listAgentsInMapping(listAgentsInMapping)
{
    _editedAction = new ActionM(actionName);

    if(_originalAction != NULL)
    {
        _editedAction->copyFrom(_originalAction);
    }

    // Force ownership of our object, it will prevent Qml from stealing it
    QQmlEngine::setObjectOwnership(this, QQmlEngine::CppOwnership);
}


/**
 * @brief Destructor
 */
ActionEditorController::~ActionEditorController()
{
    setoriginalAction(NULL);
    if(_editedAction != NULL)
    {
        ActionM* tmp = _editedAction;
        seteditedAction(NULL);
        delete tmp;
        tmp = NULL;
    }

    setoriginalViewModel(NULL);
    if(_editedViewModel != NULL)
    {
        ActionVM* tmp = _editedViewModel;
        seteditedViewModel(NULL);
        delete tmp;
        tmp = NULL;
    }
}

/**
 * @brief Valide the edition/creation
 */
void ActionEditorController::validateModification()
{
    // Save action model changes
    if(_originalAction == NULL)
    {
        setoriginalAction(_editedAction);
        seteditedAction(NULL);
    }
    else {
        _originalAction->copyFrom(_editedAction);
    }

    // Save action view model changes if it's an action editor from a view model
    if(_editedViewModel != NULL && _originalViewModel != NULL)
    {
        _originalViewModel->setcolor(_editedViewModel->color());
        _originalViewModel->setstartTimeString(_editedViewModel->startTimeString());
    }

    // If the model is connected we reinitialize the conditions
    if(_originalAction->isConnected())
    {
        // Reset connections
        _originalAction->resetConditionsConnections();

        // Init connections
        _originalAction->initializeConditionsConnections();
    }
}


/**
 * @brief Create a new condition
 */
void ActionEditorController::createNewCondition()
{
    ActionConditionVM * conditionVM = new ActionConditionVM();

    // Set a condition model
    conditionVM->setmodelM(new IOPValueConditionM());

    if(_listAgentsInMapping != NULL && _listAgentsInMapping->count() > 0)
    {
        conditionVM->modelM()->setagent(_listAgentsInMapping->at(0));
    }

    _editedAction->addConditionToList(conditionVM);

    // Set condition VM type
    conditionVM->setconditionType(ActionConditionType::VALUE);
}

/**
 * @brief Remove the conditionVM
 */
void ActionEditorController::removeCondition(ActionConditionVM* conditionVM)
{
    // Remove the condition
    if(_editedAction->conditionsList()->contains(conditionVM))
    {
        _editedAction->conditionsList()->remove(conditionVM);
    }
}


/**
 * @brief Create a new effectVM
 */
void ActionEditorController::createNewEffect()
{
    ActionEffectVM * effectVM = new ActionEffectVM();

    // Set an effect model
    effectVM->setmodelM(new IOPValueEffectM());

    if(_listAgentsInMapping != NULL && _listAgentsInMapping->count() > 0)
    {
        effectVM->modelM()->setagent(_listAgentsInMapping->at(0));

        if(_listAgentsInMapping->count() > 1)
        {
            effectVM->setsecondAgentInMapping(_listAgentsInMapping->at(1));
        }
    }

    _editedAction->addEffectToList(effectVM);

    // Set effect VM type
    effectVM->seteffectType(ActionEffectTypes::VALUE);
}


/**
 * @brief Remove the effectVM
 */
void ActionEditorController::removeEffect(ActionEffectVM* effectVM)
{
    // Remove the effect
    if(_editedAction->effectsList()->contains(effectVM))
    {
        _editedAction->effectsList()->remove(effectVM);
    }
}


