/*
 *	ActionEditorController
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

#include "actioneditorcontroller.h"


#include <QDebug>



//--------------------------------------------------------------
//
// ActionEditorController
//
//--------------------------------------------------------------


/**
 * @brief Default constructor
 * @param parent
 */
ActionEditorController::ActionEditorController(QString actionName, ActionM *originalAction, I2CustomItemSortFilterListModel<AgentInMappingVM> *listAgentsInMapping, QObject *parent) : QObject(parent),
    _originalAction(originalAction),
    _editedAction(NULL),
    _viewModel(NULL),
    _listAgentsInMapping(listAgentsInMapping)
{
    _editedAction = new ActionM(actionName, this);

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
}

/**
 * @brief Valide the edition/creation
 */
void ActionEditorController::validateModification()
{
    if(_originalAction == NULL)
    {
        setoriginalAction(_editedAction);
        seteditedAction(NULL);
    }
    else {
        _originalAction->copyFrom(_editedAction);
    }
}

/**
 * @brief Create a new condition
 */
void ActionEditorController::createNewCondition()
{
    ActionConditionVM * conditionVM = new ActionConditionVM(this);

    // Set a condition model
    conditionVM->setcondition(new ActionConditionM());

    if(_listAgentsInMapping != NULL && _listAgentsInMapping->count() > 0)
    {
        conditionVM->condition()->setagentModel(_listAgentsInMapping->at(0));
    }

    _editedAction->conditionsList()->append(conditionVM);
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
    ActionEffectVM * effectVM = new ActionEffectVM(this);

    // Set an effect model
    effectVM->seteffect(new ActionEffectM());

    if(_listAgentsInMapping != NULL && _listAgentsInMapping->count() > 0)
    {
        effectVM->effect()->setagentModel(_listAgentsInMapping->at(0));

        if(_listAgentsInMapping->count() > 1)
        {
            effectVM->setsecondAgentInMapping(_listAgentsInMapping->at(1));
        }
    }

    _editedAction->effectsList()->append(effectVM);
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


