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
        string = "Custom";
        break;

    case ValidationDurationType::IMMEDIATE:
        string = "Immediate";
        break;

    case ValidationDurationType::FOREVER:
        string = "Forever";
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
    _validityDurationString("0.0"),
    _shallRevert(false),
    _shallRevertWhenValidityIsOver(false),
    _shallRevertAfterTime(false),
    _revertAfterTime(-1),
    _revertAfterTimeString("0.0"),
    _shallRearm(false),
    _isValid(false)
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
        setvalidityDurationString(actionModel->validityDurationString());
        setshallRevert(actionModel->shallRevert());
        setshallRevertWhenValidityIsOver(actionModel->shallRevertWhenValidityIsOver());
        setshallRevertAfterTime(actionModel->shallRevertAfterTime());
        setrevertAfterTime(actionModel->revertAfterTime());
        setrevertAfterTimeString(actionModel->revertAfterTimeString());
        setshallRearm(actionModel->shallRearm());
        setisValid(actionModel->isValid());


        _effectsList.deleteAllItems();
        foreach (ActionEffectVM* effectVM, actionModel->effectsList()->toList())
        {
            ActionEffectVM* copiedEffectVM = new ActionEffectVM();
            copiedEffectVM->seteffectType(effectVM->effectType());

            IOPValueEffectM* iopEffect = dynamic_cast<IOPValueEffectM*>(effectVM->effect());
            if(iopEffect != NULL)
            {
                IOPValueEffectM * copiedIopEffect = new IOPValueEffectM();
                copiedIopEffect->copyFrom(iopEffect);
                copiedEffectVM->seteffect(copiedIopEffect);
            } else {
                MappingEffectM* mappingEffect = dynamic_cast<MappingEffectM*>(effectVM->effect());
                if(mappingEffect != NULL)
                {
                    MappingEffectM * copiedMappingEffect = new MappingEffectM();
                    copiedMappingEffect->copyFrom(mappingEffect);
                    copiedEffectVM->seteffect(copiedMappingEffect);
                } else {
                    ActionEffectM * copiedIopEffect = new ActionEffectM();
                    copiedIopEffect->copyFrom(effectVM->effect());
                    copiedEffectVM->seteffect(copiedIopEffect);
                }
            }
            _effectsList.append(copiedEffectVM);
        }


        _conditionsList.deleteAllItems();
        foreach (ActionConditionVM* conditionVM, actionModel->conditionsList()->toList())
        {
            ActionConditionVM* copiedConditionVM = new ActionConditionVM();
            copiedConditionVM->setconditionType(conditionVM->conditionType());

            IOPValueConditionM* iopCondition = dynamic_cast<IOPValueConditionM*>(conditionVM->condition());
            if(iopCondition != NULL)
            {
                IOPValueConditionM * copiedIopCondition = new IOPValueConditionM();
                copiedIopCondition->copyFrom(iopCondition);
                copiedConditionVM->setcondition(copiedIopCondition);
            } else {
                ActionConditionM * copiedIopCondition = new ActionConditionM();
                copiedIopCondition->copyFrom(conditionVM->condition());
                copiedConditionVM->setcondition(copiedIopCondition);
            }
            _conditionsList.append(copiedConditionVM);
        }
    }
}

/**
 * @brief Set the revertAfterTime string
 * @param revertAfterTime value
 */
void ActionM::setrevertAfterTimeString(QString revertAfterTime)
{
    if(_revertAfterTimeString != revertAfterTime)
    {
        _revertAfterTimeString = revertAfterTime;

        // Update the start date time
        if(revertAfterTime.isEmpty() == false)
        {
            QStringList splittedTime = revertAfterTime.split('.');
            if(splittedTime.count() == 2)
            {
                setrevertAfterTime(QString(splittedTime.at(0)).toInt()*1000 + QString(splittedTime.at(1)).toInt());
            }
        } else {
            setrevertAfterTime(-1);
        }

        emit revertAfterTimeStringChanged(revertAfterTime);
    }
}

/**
 * @brief Set the validityDuration string
 * @param validityDuration value
 */
void ActionM::setvalidityDurationString(QString validityDuration)
{
    if(_validityDurationString != validityDuration)
    {
        _validityDurationString = validityDuration;

        // Update the start date time
        if(_validityDurationString.isEmpty() == false)
        {
            QStringList splittedTime = _validityDurationString.split('.');
            if(splittedTime.count() == 2)
            {
                setvalidityDuration(QString(splittedTime.at(0)).toInt()*1000 + QString(splittedTime.at(1)).toInt());
            }
        } else {
            setvalidityDuration(-1);
        }

        emit validityDurationStringChanged(validityDuration);
    }
}

/**
 * @brief Custom setter on the shall revert flag
 * @param shall revert flag
 */
void ActionM::setshallRevert(bool shallRevert)
{
    if(_shallRevert != shallRevert)
    {
        _shallRevert = shallRevert;

        // Resert selection if the shall revert is unchecked
        if(_shallRevert == false)
        {
            setshallRevertAfterTime(false);
            setshallRevertWhenValidityIsOver(false);
            setrevertAfterTimeString("0.0");
        } else {
            setshallRevertWhenValidityIsOver(true);
        }

        emit shallRevertChanged(shallRevert);
    }
}

/**
 * @brief Initialize connections for conditions
 */
void ActionM::initializeConditionsConnections()
{
    foreach (ActionConditionVM* conditionVM, _conditionsList.toList())
    {
        if(conditionVM->condition() != NULL)
        {
            // Connect the valid change
            connect(conditionVM->condition(),&ActionConditionM::isValidChanged,this,&ActionM::_onConditionValidationChange);

            // Intialize the connection
            conditionVM->condition()->initializeConnections();
        }
    }
}

/**
 * @brief Reset connections for conditions
 */
void ActionM::resetConditionsConnections()
{
    foreach (ActionConditionVM* conditionVM, _conditionsList.toList())
    {
        if(conditionVM->condition() != NULL)
        {
            disconnect(conditionVM->condition(),&ActionConditionM::isValidChanged,this,&ActionM::_onConditionValidationChange);

            conditionVM->condition()->resetConnections();
        }
    }
}

/**
 * @brief Slot on the condition validation change
 */
void ActionM::_onConditionValidationChange(bool isValid)
{
    Q_UNUSED(isValid)

    bool actionValidation = true;

    foreach (ActionConditionVM* conditionVM, _conditionsList.toList())
    {
        if(conditionVM->condition() != NULL)
        {
            bool valid = conditionVM->condition()->isValid();
            actionValidation = valid && actionValidation;
        }

        // We leave if all conditions are not valids
        if(actionValidation == false)
        {
            break;
        }
    }

    // Set the general valid status of the action
    setisValid(actionValidation);
}


