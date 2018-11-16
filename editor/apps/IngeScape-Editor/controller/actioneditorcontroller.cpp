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
 * @param allAgentsGroupedByName
 * @param parent
 */
ActionEditorController::ActionEditorController(QString actionName,
                                               ActionM* originalAction,
                                               QList<AgentsGroupedByNameVM*> allAgentsGroupedByName,
                                               QObject *parent) : QObject(parent),
    _originalAction(originalAction),
    _editedAction(NULL),
    _originalViewModel(NULL),
    _editedViewModel(NULL),
    _allAgentsGroupedByName(allAgentsGroupedByName)
{
    // Force ownership of our object, it will prevent Qml from stealing it
    QQmlEngine::setObjectOwnership(this, QQmlEngine::CppOwnership);

    // Get an UID for our new model of action
    int uid = IngeScapeEditorUtils::getUIDforNewActionM();

    _editedAction = new ActionM(uid, actionName);

    if (_originalAction != nullptr)
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
    _allAgentsGroupedByName.clear();

    setoriginalAction(nullptr);

    if (_editedAction != nullptr)
    {
        ActionM* tmp = _editedAction;
        seteditedAction(nullptr);
        delete tmp;
    }

    setoriginalViewModel(nullptr);

    if (_editedViewModel != nullptr)
    {
        ActionVM* tmp = _editedViewModel;
        seteditedViewModel(nullptr);
        delete tmp;
    }
}


/**
 * @brief Valide the edition/creation
 */
void ActionEditorController::validateModification()
{
    // Save action model changes
    if (_originalAction == nullptr)
    {
        setoriginalAction(_editedAction);
        seteditedAction(nullptr);
    }
    else {
        _originalAction->copyFrom(_editedAction);
    }

    // Save action view model changes if it's an action editor from a view model
    if ((_editedViewModel != nullptr) && (_originalViewModel != nullptr))
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

    // Set a condition model (by default: condition on value)
    conditionVM->setmodelM(new IOPValueConditionM());
    //conditionVM->setmodelM(new ConditionOnAgentM());

    // List of agents is NOT empty
    if (!_allAgentsGroupedByName.isEmpty()) {
        conditionVM->modelM()->setagent(_allAgentsGroupedByName.at(0));
    }

    _editedAction->addConditionToList(conditionVM);

    // Set condition VM type (by default: condition on value)
    conditionVM->setconditionType(ActionConditionTypes::VALUE);
    //conditionVM->setconditionType(ActionConditionTypes::AGENT);
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
    if (!_allAgentsGroupedByName.isEmpty())
    {
        effectVM->modelM()->setagent(_allAgentsGroupedByName.at(0));

        if (_allAgentsGroupedByName.count() > 1) {
            effectVM->setsecondAgentForMapping(_allAgentsGroupedByName.at(1));
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


