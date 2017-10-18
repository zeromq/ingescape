/*
 *	ActionM
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

#include "actionm.h"


#include <QDebug>



//--------------------------------------------------------------
//
// ActionM
//
//--------------------------------------------------------------


/**
 * @brief Default constructor
 * @param parent
 */
ActionM::ActionM(QString name, QObject *parent) : QObject(parent),
    _name(name),
    _startTime(-1),
    _validityDuration(-1),
    _shallRevert(false),
    _revertWhenValidityIsOver(false),
    _revertAtTime(-1),
    _shallRearm(false)
{
    // Force ownership of our object, it will prevent Qml from stealing it
    QQmlEngine::setObjectOwnership(this, QQmlEngine::CppOwnership);

}


/**
 * @brief Destructor
 */
ActionM::~ActionM()
{
    // Delete the list of conditions
    _conditionsList.deleteAllItems();

    // Delete the list of effects
    _effectsList.deleteAllItems();
}

/**
 * @brief Copy from another action model
 * @param action model to copy
 */
void ActionM::copyFrom(ActionM* actionModel)
{
    if(actionModel != NULL)
    {
        setname(actionModel->name());
        setstartTime(actionModel->startTime());
        setvalidityDuration(actionModel->validityDuration());
        setshallRevert(actionModel->shallRevert());
        setrevertWhenValidityIsOver(actionModel->revertWhenValidityIsOver());
        setrevertAtTime(actionModel->revertAtTime());
        setshallRearm(actionModel->shallRearm());

        _effectsList.deleteAllItems();
        foreach (ActionEffectM* effect, actionModel->effectsList()->toList())
        {
            ActionEffectM* copiedEffect = new ActionEffectM();
            copiedEffect->copyFrom(effect);
            _conditionsList.append(effect);
        }

        _conditionsList.deleteAllItems();
        foreach (ActionConditionM* condition, actionModel->conditionsList()->toList())
        {
            ActionConditionM* copiedCondition = new ActionConditionM();
            copiedCondition->copyFrom(condition);
            _conditionsList.append(copiedCondition);
        }
    }
}


