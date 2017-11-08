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
ActionEditorController::ActionEditorController(ActionM *originalAction, I2CustomItemListModel<AgentInMappingVM> * listAgentsInMapping, QObject *parent) : QObject(parent),
    _originalAction(originalAction),
    _editedAction(NULL),
    _listAgentsInMapping(listAgentsInMapping)
{
    if(_originalAction != NULL)
    {
        _editedAction = new ActionM(_originalAction->name(), this);
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
    ActionConditionM * condition = new ActionConditionM(this);

    if(_listAgentsInMapping != NULL && _listAgentsInMapping->count() > 0)
    {
        AgentInMappingVM * agentInMapping = _listAgentsInMapping->at(0);
        if(agentInMapping->models()->count() > 0)
        {
            condition->setagentModel(agentInMapping->models()->at(0));
        }
    }

    _editedAction->conditionsList()->append(condition);
}

/**
 * @brief Remove the condition
 */
void ActionEditorController::removeCondition(ActionConditionM* condition)
{
    // Remove the condition
    if(_editedAction->conditionsList()->contains(condition))
    {
        _editedAction->conditionsList()->remove(condition);
    }
}

/**
 * @brief Create a new effect
 */
void ActionEditorController::createNewEffect()
{
    ActionEffectM * effect = new ActionEffectM(this);

    if(_listAgentsInMapping != NULL && _listAgentsInMapping->count() > 0)
    {
        AgentInMappingVM * agentInMapping = _listAgentsInMapping->at(0);
        if(agentInMapping->models()->count() > 0)
        {
            effect->setagentModel(agentInMapping->models()->at(0));
        }
    }

    _editedAction->effectsList()->append(effect);
}

/**
 * @brief Remove the effect
 */
void ActionEditorController::removeEffect(ActionEffectM* effect)
{
    // Remove the effect
    if(_editedAction->effectsList()->contains(effect))
    {
        _editedAction->effectsList()->remove(effect);
    }
}


