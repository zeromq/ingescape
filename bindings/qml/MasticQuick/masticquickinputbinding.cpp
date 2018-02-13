/*
 *  Mastic - QML binding
 *
 *  Copyright (c) 2018 Ingenuity i/o. All rights reserved.
 *
 *  See license terms for the rights and conditions
 *  defined by copyright holders.
 *
 *
 *  Contributors:
 *      Alexandre Lemort <lemort@ingenuity.io>
 *
 */

#include "masticquickinputbinding.h"


#include <QDebug>


/**
 * @brief Default constructor
 * @param parent
 */
MasticQuickInputBinding::MasticQuickInputBinding(QObject *parent) : QObject(parent),
    _target(NULL),
    _isCompleted(false),
    _isUsedAsQQmlPropertyValueSource(false)
{

}



/**
 * @brief Destructor
 */
MasticQuickInputBinding::~MasticQuickInputBinding()
{
    // Unsubscribe to our target
    if (_target != NULL)
    {
        disconnect(_target, &QObject::destroyed, this, &MasticQuickInputBinding::_ontargetDestroyed);
    }

    // clear our hashtable of properties
    _qmlPropertiesByName.clear();
}



//-------------------------------------------------------------------
//
// Custom setters
//
//-------------------------------------------------------------------


/**
 * @brief Set our target
 * @param value
 */
void MasticQuickInputBinding::settarget(QObject *value)
{
    if (_target != value)
    {
        // Unsubscribe to our previous target
        if (_target != NULL)
        {
            disconnect(_target, &QObject::destroyed, this, &MasticQuickInputBinding::_ontargetDestroyed);
        }

        _target = value;

        // Subscribe to our new target
        if (_target != NULL)
        {
            connect(_target, &QObject::destroyed, this, &MasticQuickInputBinding::_ontargetDestroyed);
        }


        // Notify change
        Q_EMIT targetChanged(value);
    }
}



/**
 * @brief Set properties
 * @param value
 */
void MasticQuickInputBinding::setproperties(QString value)
{
    if (_properties != value)
    {
        // Check if we can save this value
        if (!_isUsedAsQQmlPropertyValueSource)
        {
            _properties = value;


            // Notify change
            Q_EMIT propertiesChanged(value);
        }
        else
        {
            qWarning() << "MasticInputBinding warning: properties can not be set when our item is used as a property value source (invalid value:"
                       << _properties << ")";
        }
    }
}



/**
 * @brief Set the prefix of Mastic inputs
 * @param value
 */
void MasticQuickInputBinding::setinputsPrefix(QString value)
{
    if (_inputsPrefix != value)
    {
        _inputsPrefix = value;


        // Notify change
        Q_EMIT inputsPrefixChanged(value);
    }
}




//-------------------------------------------------------------------
//
// Protected slots
//
//-------------------------------------------------------------------


/**
 * @brief Called when the object associated to our target property is destroyed
 * @param sender
 */
void MasticQuickInputBinding::_ontargetDestroyed(QObject *sender)
{
    Q_UNUSED(sender)
    _target = NULL;
    Q_EMIT targetChanged(NULL);
}


//-------------------------------------------------------------------
//
// Protected methods
//
//-------------------------------------------------------------------



/**
 * @brief QQmlPropertyValueSource API: This method will be called by the QML engine when assigning a value source
 *        with the following syntax    MasticQuickInputBinding on property { }
 *
 * @param property
 */
void MasticQuickInputBinding::setTarget(const QQmlProperty &property)
{
    // Our component is used as a property value source
    _isUsedAsQQmlPropertyValueSource = true;

    // Check if we have a property
    if (property.isProperty())
    {
        // Check if this property is writable
        if (property.isWritable())
        {
            qDebug() << "setTarget:" << property.name();
        }
        else
        {
            qWarning() << "MasticInputBinding warning: can only be associated to a writable property." << property.name() << "is not a writable";
        }
    }
    else
    {
        qWarning() << "MasticInputBinding warning: can only be associated to a property." << property.name() << "is not a property";
    }
}


/**
 * @brief QQmlParserStatus API: Invoked after class creation, but before any properties have been set
 */
void MasticQuickInputBinding::classBegin()
{
}


/**
 * @brief QQmlParserStatus API: Invoked after the root component that caused this instantiation has completed construction.
 *        At this point all static values and binding values have been assigned to the class.
 */
void MasticQuickInputBinding::componentComplete()
{
    // Our component is completed
    _isCompleted = true;

    // Check if everything is ok
    if (_isUsedAsQQmlPropertyValueSource)
    {
        // Check if properties is empty
        if (!_properties.isEmpty())
        {
            qWarning() << "MasticInputBinding warning: properties can not be set when our item is used as a property value source (invalid value:"
                       << _properties << ")";

            // Reset
            _properties = "";
            Q_EMIT propertiesChanged(_properties);
        }
    }
}
