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
#include <misc/ingescapeeditorutils.h>

/**
 * @brief Constructor
 * @param actionName
 * @param originalAction
 * @param listAgentsInMapping
 * @param parent
 */
ActionEditorController::ActionEditorController(QString actionName,
                                               ActionM *originalAction,
                                               I2CustomItemSortFilterListModel<AgentInMappingVM> *listAgentsInMapping,
                                               QObject *parent) : QObject(parent),
    _originalAction(originalAction),
    _editedAction(NULL),
    _originalViewModel(NULL),
    _editedViewModel(NULL),
    _listAgentsInMapping(listAgentsInMapping)
{
    // Force ownership of our object, it will prevent Qml from stealing it
    QQmlEngine::setObjectOwnership(this, QQmlEngine::CppOwnership);

    // Get an UID for our new model of action
    int uid = IngeScapeEditorUtils::getUIDforNewActionM();

    _editedAction = new ActionM(uid, actionName);

    if (_originalAction != NULL)
    {
        _editedAction->copyFrom(_originalAction);

        // Cancel the increment because the edited action uses now (after the call to "copyFrom") the uid of the original action
        // Free the UID of the action model
        IngeScapeEditorUtils::freeUIDofActionM(uid);
    }
}


/**
 * @brief Destructor
 */
ActionEditorController::~ActionEditorController()
{
    //_listAgentsInMapping->clear();

    setoriginalAction(NULL);

    if (_editedAction != NULL)
    {
        ActionM* tmp = _editedAction;
        seteditedAction(NULL);
        delete tmp;
        tmp = NULL;
    }

    setoriginalViewModel(NULL);

    if (_editedViewModel != NULL)
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
    if (_originalAction == NULL)
    {
        setoriginalAction(_editedAction);
        seteditedAction(NULL);
    }
    else {
        _originalAction->copyFrom(_editedAction);
    }

    // Save action view model changes if it's an action editor from a view model
    if ((_editedViewModel != NULL) && (_originalViewModel != NULL))
    {
        _originalViewModel->setcolor(_editedViewModel->color());
        _originalViewModel->setstartTimeString(_editedViewModel->startTimeString());
    }

    // If the model is connected we reinitialize the conditions
    if (_originalAction->isConnected())
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

    // List of agents is NOT empty
    if ((_listAgentsInMapping != NULL) && !_listAgentsInMapping->isEmpty()) {
        conditionVM->modelM()->setagent(_listAgentsInMapping->at(0));
    }

    _editedAction->addConditionToList(conditionVM);

    // Set condition VM type
    conditionVM->setconditionType(ActionConditionTypes::VALUE);
}


/**
 * @brief Remove the conditionVM
 */
void ActionEditorController::removeCondition(ActionConditionVM* conditionVM)
{
    // Remove the condition
    if (_editedAction->conditionsList()->contains(conditionVM)) {
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

    // List of agents is NOT empty
    if ((_listAgentsInMapping != NULL) && !_listAgentsInMapping->isEmpty())
    {
        effectVM->modelM()->setagent(_listAgentsInMapping->at(0));

        if (_listAgentsInMapping->count() > 1) {
            effectVM->setsecondAgentInMapping(_listAgentsInMapping->at(1));
        }
    }

    _editedAction->addEffectToList(effectVM);

    // Set effect VM type
    effectVM->seteffectType(ActionEffectTypes::VALUE);
}


/**
 * @brief Remove the effect
 */
void ActionEditorController::removeEffect(ActionEffectVM* effectVM)
{
    if(_editedAction->effectsList()->contains(effectVM)) {
        _editedAction->effectsList()->remove(effectVM);
    }
}


