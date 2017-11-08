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

/**
 * @brief Validation duration type for an action
 * @param value
 * @return
 */
QString ValidationDurationType::enumToString(int value)
{
    QString string = "Validation duration";

    switch (value) {
    case ValidationDurationType::CUSTOM:
        string = "custom";
        break;

    case ValidationDurationType::IMMEDIATE:
        string = "immediate";
        break;

    case ValidationDurationType::INFINITE:
        string = "infinite";
        break;

    default:
        break;
    }

    return string;
}


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
    _validityDurationType(ValidationDurationType::IMMEDIATE),
    _validityDuration(-1),
    _shallRevert(false),
    _shallRevertWhenValidityIsOver(false),
    _shallRevertAfterTime(false),
    _revertAfterTimeInSec(-1),
    _revertAfterTime("00:00:00"),
    _shallRearm(false),
    _actionsPanelIndex(-1)
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
        setvalidityDurationType(actionModel->validityDurationType());
        setvalidityDuration(actionModel->validityDuration());
        setshallRevert(actionModel->shallRevert());
        setshallRevertWhenValidityIsOver(actionModel->shallRevertWhenValidityIsOver());
        setshallRevertAfterTime(actionModel->shallRevertAfterTime());
        setrevertAfterTimeInSec(actionModel->revertAfterTimeInSec());
        setrevertAfterTime(actionModel->revertAfterTime());
        setshallRearm(actionModel->shallRearm());
        setactionsPanelIndex(actionModel->actionsPanelIndex());

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

/**
 * @brief Set the revertAfterTime flag
 * @param revertAfterTime value
 */
void ActionM::setrevertAfterTime(QString revertAfterTime)
{
    if(_revertAfterTime != revertAfterTime)
    {
        _revertAfterTime = revertAfterTime;

        // Update the start date time
        if(revertAfterTime.isEmpty() == false)
        {
            QStringList splittedTime = revertAfterTime.split(':');
            if(splittedTime.count() == 3)
            {
                setrevertAfterTimeInSec(QString(splittedTime.at(0)).toInt()*3600 + QString(splittedTime.at(1)).toInt()*60 + + QString(splittedTime.at(2)).toInt());
            }
        } else {
            setrevertAfterTimeInSec(-1);
        }

        emit revertAfterTimeChanged(revertAfterTime);
    }
}



