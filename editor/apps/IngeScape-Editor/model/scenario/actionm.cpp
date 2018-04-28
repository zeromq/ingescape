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
        string = "during";
        break;

    case ValidationDurationType::IMMEDIATE:
        string = "at start only";
        break;

    case ValidationDurationType::FOREVER:
        string = "forever";
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
    _rearmAfterTime(200),
    _rearmAfterTimeString("0.200"),
    _isValid(false),
    _isConnected(false)
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
    if (actionModel != NULL)
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
        setrearmAfterTime(actionModel->rearmAfterTime());
        setrearmAfterTimeString(actionModel->rearmAfterTimeString());

        _effectsList.deleteAllItems();

        foreach (ActionEffectVM* effectVM, actionModel->effectsList()->toList())
        {
            ActionEffectVM* copiedEffectVM = new ActionEffectVM();
            copiedEffectVM->seteffectType(effectVM->effectType());

            switch (effectVM->effectType())
            {
            case ActionEffectTypes::AGENT: {
                EffectOnAgentM* effectOnAgent = qobject_cast<EffectOnAgentM*>(effectVM->modelM());
                if (effectOnAgent != NULL)
                {
                    EffectOnAgentM* copy = new EffectOnAgentM();
                    copy->copyFrom(effectOnAgent);

                    copiedEffectVM->setmodelM(copy);
                }
                break;
            }
            case ActionEffectTypes::VALUE: {
                IOPValueEffectM* iopEffect = qobject_cast<IOPValueEffectM*>(effectVM->modelM());
                if (iopEffect != NULL)
                {
                    IOPValueEffectM* copy = new IOPValueEffectM();
                    copy->copyFrom(iopEffect);

                    copiedEffectVM->setmodelM(copy);
                }
                break;
            }
            case ActionEffectTypes::MAPPING: {
                MappingEffectM* mappingEffect = qobject_cast<MappingEffectM*>(effectVM->modelM());
                if(mappingEffect != NULL)
                {
                    MappingEffectM* copy = new MappingEffectM();
                    copy->copyFrom(mappingEffect);

                    copiedEffectVM->setmodelM(copy);
                }
                break;
            }
            default:
                break;
            }

            addEffectToList(copiedEffectVM);
        }


        _conditionsList.deleteAllItems();
        foreach (ActionConditionVM* conditionVM, actionModel->conditionsList()->toList())
        {
            ActionConditionVM* copiedConditionVM = new ActionConditionVM();
            copiedConditionVM->setconditionType(conditionVM->conditionType());

            IOPValueConditionM* iopCondition = qobject_cast<IOPValueConditionM*>(conditionVM->modelM());
            if(iopCondition != NULL)
            {
                IOPValueConditionM * copiedIopCondition = new IOPValueConditionM();
                copiedIopCondition->copyFrom(iopCondition);
                copiedConditionVM->setmodelM(copiedIopCondition);
            }
            else {
                ActionConditionM * copiedIopCondition = new ActionConditionM();
                copiedIopCondition->copyFrom(conditionVM->modelM());
                copiedConditionVM->setmodelM(copiedIopCondition);
            }
            addConditionToList(copiedConditionVM);
        }
    }
}


/**
 * @brief Setter for property "Revert After Time String"
 * @param value
 */
void ActionM::setrevertAfterTimeString(QString value)
{
    if (_revertAfterTimeString != value)
    {
        _revertAfterTimeString = value;

        if (!_revertAfterTimeString.isEmpty())
        {
            bool successSeconds = false;
            bool successMilliSeconds = false;

            QStringList splittedTime = _revertAfterTimeString.split('.');
            if (splittedTime.count() == 2)
            {
                int seconds = splittedTime.at(0).toInt(&successSeconds);
                int milliSeconds = splittedTime.at(1).leftJustified(3, '0').toInt(&successMilliSeconds);

                if (successSeconds && successMilliSeconds) {
                    setrevertAfterTime(seconds * 1000 + milliSeconds);
                    //qDebug() << "Revert After Time =" << _revertAfterTime;
                }
                else {
                    setrevertAfterTime(-1);
                    qCritical() << "Wrong 'Revert After Time':" << _revertAfterTimeString << "for action" << _name;
                }
            }
            else {
                int seconds = _revertAfterTimeString.toInt(&successSeconds);
                if (successSeconds) {
                    setrevertAfterTime(seconds * 1000);
                    //qDebug() << "Revert After Time =" << _revertAfterTime;
                }
                else {
                    setrevertAfterTime(-1);
                    qCritical() << "Wrong 'Revert After Time':" << _revertAfterTimeString << "for action" << _name;
                }
            }
        }
        else {
            setrevertAfterTime(-1);
            qCritical() << "Wrong 'Revert After Time':" << _revertAfterTimeString << "for action" << _name;
        }

        Q_EMIT revertAfterTimeStringChanged(value);
    }
}

/**
 * @brief Setter for property "Rearm After Time String"
 * @param value
 */
void ActionM::setrearmAfterTimeString(QString value)
{
    if (_rearmAfterTimeString != value)
    {
        _rearmAfterTimeString = value;

        if (!_rearmAfterTimeString.isEmpty())
        {
            bool successSeconds = false;
            bool successMilliSeconds = false;

            QStringList splittedTime = _rearmAfterTimeString.split('.');
            if (splittedTime.count() == 2)
            {
                int seconds = splittedTime.at(0).toInt(&successSeconds);
                int milliSeconds = splittedTime.at(1).leftJustified(3, '0').toInt(&successMilliSeconds);

                if (successSeconds && successMilliSeconds) {
                    setrearmAfterTime(seconds * 1000 + milliSeconds);
                    //qDebug() << "Rearm After Time =" << _revertAfterTime;
                }
                else {
                    setrearmAfterTime(-1);
                    qCritical() << "Wrong 'Rearm After Time':" << _rearmAfterTimeString << "for action" << _name;
                }
            }
            else {
                int seconds = _rearmAfterTimeString.toInt(&successSeconds);
                if (successSeconds) {
                    setrearmAfterTime(seconds * 1000);
                    //qDebug() << "Rearm After Time =" << _revertAfterTime;
                }
                else {
                    setrearmAfterTime(-1);
                    qCritical() << "Wrong 'Rearm After Time':" << _rearmAfterTimeString << "for action" << _name;
                }
            }
        }
        else {
            setrearmAfterTime(-1);
            qCritical() << "Wrong 'Rearm After Time':" << _rearmAfterTimeString << "for action" << _name;
        }

        Q_EMIT rearmAfterTimeStringChanged(value);
    }
}

/**
 * @brief Setter for property "Validity Duration String"
 * @param value
 */
void ActionM::setvalidityDurationString(QString value)
{
    if(_validityDurationString != value)
    {
        _validityDurationString = value;

        if (!_validityDurationString.isEmpty())
        {
            bool successSeconds = false;
            bool successMilliSeconds = false;

            QStringList splittedTime = _validityDurationString.split('.');
            if (splittedTime.count() == 2)
            {
                int seconds = splittedTime.at(0).toInt(&successSeconds);
                int milliSeconds = splittedTime.at(1).leftJustified(3, '0').toInt(&successMilliSeconds);

                if (successSeconds && successMilliSeconds) {
                    setvalidityDuration(seconds * 1000 + milliSeconds);
                    //qDebug() << "Validity Duration =" << _validityDuration;
                }
                else {
                    setvalidityDuration(-1);
                    qCritical() << "Wrong 'Validity Duration':" << _validityDurationString << "for action" << _name;
                }
            }
            else {
                int seconds = _validityDurationString.toInt(&successSeconds);
                if (successSeconds) {
                    setvalidityDuration(seconds * 1000);
                    //qDebug() << "Validity Duration =" << _validityDuration;
                }
                else {
                    setvalidityDuration(-1);
                    qCritical() << "Wrong 'Validity Duration':" << _validityDurationString << "for action" << _name;
                }
            }
        }
        else {
            setvalidityDuration(-1);
            qCritical() << "Wrong 'Validity Duration':" << _validityDurationString << "for action" << _name;
        }

        Q_EMIT validityDurationStringChanged(value);
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

        // Reset properties if the shall revert is unchecked
        if (!_shallRevert)
        {
            setshallRevertWhenValidityIsOver(false);
            setshallRevertAfterTime(false);
            setrevertAfterTimeString("0.0");
        }
        else {
            setshallRevertWhenValidityIsOver(true);
        }

        Q_EMIT shallRevertChanged(shallRevert);
    }
}


/**
 * @brief Initialize connections for conditions
 */
void ActionM::initializeConditionsConnections()
{
    if (!_isConnected)
    {
        // Initialize conditions connections
        foreach (ActionConditionVM* conditionVM, _conditionsList.toList())
        {
            if(conditionVM->modelM() != NULL)
            {
                // Connect to changes on flag "is valid"
                connect(conditionVM->modelM(), &ActionConditionM::isValidChanged, this, &ActionM::_onConditionValidationChange);

                // Intialize the connection
                conditionVM->modelM()->initializeConnections();
            }
        }

        // Evaluate the action validation flag
        _onConditionValidationChange(true);

        setisConnected(true);
    }
}


/**
 * @brief Reset connections for conditions
 */
void ActionM::resetConditionsConnections()
{
    if (_isConnected)
    {
        // Initialize conditions connections
        foreach (ActionConditionVM* conditionVM, _conditionsList.toList())
        {
            if(conditionVM->modelM() != NULL)
            {
                disconnect(conditionVM->modelM(), &ActionConditionM::isValidChanged, this, &ActionM::_onConditionValidationChange);

                conditionVM->modelM()->resetConnections();
            }
        }

        setisConnected(false);
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
        if (conditionVM->modelM() != NULL)
        {
            bool valid = conditionVM->modelM()->isValid();
            actionValidation = valid && actionValidation;
        }

        // We leave if all conditions are not valids
        if (!actionValidation)
        {
            break;
        }
    }

    // Set the general valid status of the action
    setisValid(actionValidation);
}


/**
 * @brief Add effect to the list
 */
void ActionM::addEffectToList(ActionEffectVM* effectVM)
{
    if(effectVM != NULL && effectVM->modelM())
    {
        connect(effectVM->modelM(), &ActionEffectM::askForDestruction, this, &ActionM::_onEffectDestructionAsked);
    }

    _effectsList.append(effectVM);
}

/**
 * @brief Add condition to the list
 */
void ActionM::addConditionToList(ActionConditionVM* conditionVM)
{
    if(conditionVM != NULL && conditionVM->modelM())
    {
        connect(conditionVM->modelM(), &ActionConditionM::askForDestruction, this, &ActionM::_onConditionDestructionAsked);
    }

    _conditionsList.append(conditionVM);
}

/**
 * @brief Triggered when an agent model associated to an effect has been destroyed from the mapping
 *        The effect does not need to exist anymore, we can delete it
 */
void ActionM::_onEffectDestructionAsked()
{
    ActionEffectM* actionEffect = qobject_cast<ActionEffectM*>(sender());
    if (actionEffect != NULL)
    {
        disconnect(actionEffect, &ActionEffectM::askForDestruction, this, &ActionM::_onEffectDestructionAsked);

        foreach (ActionEffectVM * effectVM, _effectsList.toList())
        {
            if(effectVM != NULL && effectVM->modelM() == actionEffect)
            {
                _effectsList.remove(effectVM);
                delete effectVM;
                effectVM = NULL;
            }
        }
    }
}

/**
 * @brief Triggered when an agent model associated to a condition has been destroyed from the mapping
 *        The condition does not need to exist anymore, we can delete it
 */
void ActionM::_onConditionDestructionAsked()
{
    ActionConditionM* actionCondition = qobject_cast<ActionConditionM*>(sender());
    if (actionCondition != NULL)
    {
        disconnect(actionCondition, &ActionConditionM::askForDestruction, this, &ActionM::_onConditionDestructionAsked);

        foreach (ActionConditionVM * conditionVM, _conditionsList.toList())
        {
            if(conditionVM != NULL && conditionVM->modelM() == actionCondition)
            {
                _conditionsList.remove(conditionVM);
                delete conditionVM;
                conditionVM = NULL;
            }
        }
    }
}

