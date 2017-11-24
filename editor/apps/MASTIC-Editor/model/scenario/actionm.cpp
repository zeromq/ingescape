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

            switch (effectVM->effectType())
            {
            case ActionEffectTypes::AGENT: {
                ActionEffectM* copiedEffect = new ActionEffectM();
                copiedEffect->copyFrom(effectVM->modelM());

                copiedEffectVM->setmodelM(copiedEffect);
                break;
            }
            case ActionEffectTypes::VALUE: {
                IOPValueEffectM* iopEffect = dynamic_cast<IOPValueEffectM*>(effectVM->modelM());
                if (iopEffect != NULL)
                {
                    IOPValueEffectM* copiedIopEffect = new IOPValueEffectM();
                    copiedIopEffect->copyFrom(iopEffect);

                    copiedEffectVM->setmodelM(copiedIopEffect);
                }
                break;
            }
            case ActionEffectTypes::MAPPING: {
                MappingEffectM* mappingEffect = dynamic_cast<MappingEffectM*>(effectVM->modelM());
                if(mappingEffect != NULL)
                {
                    MappingEffectM* copiedMappingEffect = new MappingEffectM();
                    copiedMappingEffect->copyFrom(mappingEffect);

                    copiedEffectVM->setmodelM(copiedMappingEffect);
                }
                break;
            }
            default:
                break;
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
            }
            else {
                ActionConditionM * copiedIopCondition = new ActionConditionM();
                copiedIopCondition->copyFrom(conditionVM->condition());
                copiedConditionVM->setcondition(copiedIopCondition);
            }
            _conditionsList.append(copiedConditionVM);
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

        emit revertAfterTimeStringChanged(value);
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

        emit validityDurationStringChanged(value);
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
        if(_shallRevert == false)
        {
            setshallRevertWhenValidityIsOver(false);
            setshallRevertAfterTime(false);
            setrevertAfterTimeString("0.0");
        }
        else {
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
    if(_isConnected == false)
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
    if(_isConnected == true)
    {
        foreach (ActionConditionVM* conditionVM, _conditionsList.toList())
        {
            if(conditionVM->condition() != NULL)
            {
                disconnect(conditionVM->condition(),&ActionConditionM::isValidChanged,this,&ActionM::_onConditionValidationChange);

                conditionVM->condition()->resetConnections();
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


