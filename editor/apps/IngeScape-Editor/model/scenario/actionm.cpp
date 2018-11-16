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
 * @brief Enum "ValidationDurationTypes" to string
 * @param value
 * @return
 */
QString ValidationDurationTypes::enumToString(int value)
{
    switch (value)
    {
    case ValidationDurationTypes::CUSTOM:
        return tr("during");

    case ValidationDurationTypes::IMMEDIATE:
        return tr("at start only");

    case ValidationDurationTypes::FOREVER:
        return tr("forever");

    default:
        return "";
    }
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
ActionM::ActionM(int uid, QString name, QObject *parent) : QObject(parent),
    _uid(uid),
    _name(name),
    _validityDurationType(ValidationDurationTypes::IMMEDIATE),
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
    if (actionModel != nullptr)
    {
        setuid(actionModel->uid());
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

        for (ActionEffectVM* effectVM : actionModel->effectsList()->toList())
        {
            ActionEffectVM* copiedEffectVM = new ActionEffectVM();
            copiedEffectVM->seteffectType(effectVM->effectType());

            switch (effectVM->effectType())
            {
            case ActionEffectTypes::AGENT: {
                EffectOnAgentM* effectOnAgent = qobject_cast<EffectOnAgentM*>(effectVM->modelM());
                if (effectOnAgent != nullptr)
                {
                    EffectOnAgentM* copy = new EffectOnAgentM();
                    copy->copyFrom(effectOnAgent);

                    copiedEffectVM->setmodelM(copy);
                }
                break;
            }
            case ActionEffectTypes::VALUE: {
                IOPValueEffectM* iopEffect = qobject_cast<IOPValueEffectM*>(effectVM->modelM());
                if (iopEffect != nullptr)
                {
                    IOPValueEffectM* copy = new IOPValueEffectM();
                    copy->copyFrom(iopEffect);

                    copiedEffectVM->setmodelM(copy);
                }
                break;
            }
            case ActionEffectTypes::MAPPING: {
                MappingEffectM* mappingEffect = qobject_cast<MappingEffectM*>(effectVM->modelM());
                if(mappingEffect != nullptr)
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
        for (ActionConditionVM* reference : actionModel->conditionsList()->toList())
        {
            if ((reference != nullptr) && (reference->modelM() != nullptr))
            {
                ActionConditionVM* copyVM = new ActionConditionVM();
                copyVM->setconditionType(reference->conditionType());

                switch (copyVM->conditionType())
                {
                case ActionConditionTypes::VALUE:
                {
                    IOPValueConditionM* iopValueCondition = qobject_cast<IOPValueConditionM*>(reference->modelM());
                    if (iopValueCondition != nullptr)
                    {
                        IOPValueConditionM* copyM = new IOPValueConditionM();
                        copyM->copyFrom(iopValueCondition);

                        copyVM->setmodelM(copyM);
                    }
                }
                    break;

                case ActionConditionTypes::AGENT:
                {
                    ConditionOnAgentM* conditionOnAgent = qobject_cast<ConditionOnAgentM*>(reference->modelM());
                    if (conditionOnAgent != nullptr)
                    {
                        ConditionOnAgentM* copyM = new ConditionOnAgentM();
                        copyM->copyFrom(conditionOnAgent);

                        copyVM->setmodelM(copyM);
                    }
                }
                    break;

                default:
                    break;
                }

                addConditionToList(copyVM);
            }
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
 * @brief Setter for property "Shall Revert"
 * @param value
 */
void ActionM::setshallRevert(bool value)
{
    if (_shallRevert != value)
    {
        _shallRevert = value;

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

        Q_EMIT shallRevertChanged(value);
    }
}


/**
 * @brief Initialize connections for conditions
 */
void ActionM::initializeConditionsConnections()
{
    if (!_isConnected)
    {
        for (ActionConditionVM* conditionVM : _conditionsList.toList())
        {
            if ((conditionVM != nullptr) && (conditionVM->modelM() != nullptr))
            {
                // Connect to changes on flag "is valid"
                connect(conditionVM->modelM(), &ActionConditionM::isValidChanged, this, &ActionM::_onIsValidConditionChanged);

                // Intialize the connection
                conditionVM->modelM()->initializeConnections();
            }
        }

        // Force to update the flag "is Valid" of our action (parameter is not used)
        _onIsValidConditionChanged(false);

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
        for (ActionConditionVM* conditionVM : _conditionsList.toList())
        {
            if ((conditionVM != nullptr) && (conditionVM->modelM() != nullptr))
            {
                // DIS-connect from changes on flag "is valid"
                disconnect(conditionVM->modelM(), &ActionConditionM::isValidChanged, this, &ActionM::_onIsValidConditionChanged);

                conditionVM->modelM()->resetConnections();
            }
        }

        setisConnected(false);
    }
}


/**
 * @brief Add effect to the list
 * @param effectVM
 */
void ActionM::addEffectToList(ActionEffectVM* effectVM)
{
    if ((effectVM != nullptr) && (effectVM->modelM() != nullptr)) {
        connect(effectVM->modelM(), &ActionEffectM::askForDestruction, this, &ActionM::_onEffectDestructionAsked);
    }

    _effectsList.append(effectVM);
}


/**
 * @brief Add condition to the list
 * @param conditionVM
 */
void ActionM::addConditionToList(ActionConditionVM* conditionVM)
{
    if ((conditionVM != nullptr) && (conditionVM->modelM() != nullptr)) {
        connect(conditionVM->modelM(), &ActionConditionM::askForDestruction, this, &ActionM::_onConditionDestructionAsked);
    }

    _conditionsList.append(conditionVM);
}


/**
 * @brief Slot called when the flag "is Valid" of a condition changed
 * @param isValid
 */
void ActionM::_onIsValidConditionChanged(bool isValid)
{
    Q_UNUSED(isValid)

    bool globalIsValid = true;

    for (ActionConditionVM* condition : _conditionsList.toList())
    {
        if ((condition != nullptr) && (condition->modelM() != nullptr))
        {
            // Logic AND
            //globalIsValid = condition->modelM()->isValid() && globalIsValid;

            // Leave the loop if one of the conditions is not valid
            if (!condition->modelM()->isValid())
            {
                globalIsValid = false;
                break;
            }
        }
    }

    // Set the global flag "is Valid" of our action
    setisValid(globalIsValid);
}


/**
 * @brief Triggered when an agent model associated to an effect has been destroyed from the mapping
 *        The effect does not need to exist anymore, we can delete it
 */
void ActionM::_onEffectDestructionAsked()
{
    ActionEffectM* actionEffect = qobject_cast<ActionEffectM*>(sender());
    if (actionEffect != nullptr)
    {
        disconnect(actionEffect, &ActionEffectM::askForDestruction, this, &ActionM::_onEffectDestructionAsked);

        for (ActionEffectVM* effectVM : _effectsList.toList())
        {
            if ((effectVM != nullptr) && (effectVM->modelM() == actionEffect))
            {
                _effectsList.remove(effectVM);

                delete effectVM;
                effectVM = nullptr;
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
    if (actionCondition != nullptr)
    {
        disconnect(actionCondition, &ActionConditionM::askForDestruction, this, &ActionM::_onConditionDestructionAsked);

        for (ActionConditionVM* conditionVM : _conditionsList.toList())
        {
            if ((conditionVM != nullptr) && (conditionVM->modelM() == actionCondition))
            {
                _conditionsList.remove(conditionVM);

                delete conditionVM;
                conditionVM = nullptr;
            }
        }
    }
}

