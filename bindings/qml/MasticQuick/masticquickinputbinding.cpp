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

#include "masticquickbinding.h"


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
    // Reset our isCompleted flag to be able to use setters
    _isCompleted = false;

    // Unsubscribe to our target
    if (_target != NULL)
    {
        settarget(NULL);
    }

    // Clear internal data
    _clear();
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
        // Unsubscribe to our previous target if needed
        if (_target != NULL)
        {
            disconnect(_target, &QObject::destroyed, this, &MasticQuickInputBinding::_ontargetDestroyed);
        }

        // Save our new value
        _target = value;

        // Subscribe to our new target if needed
        if (_target != NULL)
        {
            connect(_target, &QObject::destroyed, this, &MasticQuickInputBinding::_ontargetDestroyed);
        }

        // Check if we can use this value
        if (!_isUsedAsQQmlPropertyValueSource)
        {
            // Update internal data
            _update();
        }
        else
        {
            qmlWarning(this) << "target can not be set when our item is used as a property value source";
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
        // Save our new value
        _properties = value;

        // Check if we can use this value
        if (!_isUsedAsQQmlPropertyValueSource)
        {
            // Update internal data
            _update();
        }
        else
        {
            qmlWarning(this) << "properties can not be set when our item is used as a property value source (invalid value: "
                              << _properties << " )";
        }

        // Notify change
        Q_EMIT propertiesChanged(value);
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
        // Save our new value
        _inputsPrefix = value;

        // Update internal data
        _update();

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

    // Reset our target
    _target = NULL;

    // Update internal data
    _update();

    // Notify that our target is destroyed
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
            // Save this property
            _propertyValueSourceTarget = property;
        }
        else
        {
            // Reset value
            _propertyValueSourceTarget = QQmlProperty();

            qmlWarning(this) << "can only be associated to a writable property. " << property.name() << " is not a writable";
        }
    }
    else
    {
        // Reset value
        _propertyValueSourceTarget = QQmlProperty();

        qmlWarning(this) << "can only be associated to a property. " << property.name() << " is not a property";
    }

    // Update internal data
    _update();
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
    // Check if everything is ok
    if (_isUsedAsQQmlPropertyValueSource)
    {
        // Check if properties is empty
        if (!_properties.isEmpty())
        {
            qmlWarning(this) << "properties can not be set when our item is used as a property value source (invalid value: "
                             << _properties << " )";
        }

        // Check if target is NULL
        if (_target != NULL)
        {
            qmlWarning(this) << "target can ot be set when our item is used as a property value source";
        }
    }

    // Our component is completed
    _isCompleted = true;


    // Update internal data
    _update();
}



/**
 * @brief Clear internal data
 */
void MasticQuickInputBinding::_clear()
{
    // clear our hashtable of properties
    _qmlPropertiesByName.clear();
}



/**
 * @brief Update internal data
 */
void MasticQuickInputBinding::_update()
{
    // Ensure that our component is completed
    if (_isCompleted)
    {
        // Clean-up previous data
        _clear();


        // Check if our item is used as as property value source or a standard QML item
        if (_isUsedAsQQmlPropertyValueSource)
        {
            //
            // Property value source
            //

            // Add our QML property to our hashtable if needed
            if (_propertyValueSourceTarget.isValid())
            {
                _qmlPropertiesByName.insert( _propertyValueSourceTarget.name(), _propertyValueSourceTarget);
            }
        }
        else
        {
            //
            // Standard QML item
            //

            // Check if we have a target and a list of properties
            QString properties = _properties.trimmed();
            if ((_target != NULL) && !properties.isEmpty())
            {
                // Check if we have a specific set of properties OR all properties
                if ((MasticQuickBinding::instance() != NULL) && (properties == MasticQuickBinding::instance()->AllProperties()))
                {
                    qmlWarning(this) << "'properties: MasticBinding.AllProperties' is not yet implemented";
                }
                else
                {
                    //
                    // Specific set of properties
                    //

                    // Parse our list of properties
                    QStringList listOfPropertyNames = _properties.split(QLatin1Char(','));
                    int numberOfPropertyNames = listOfPropertyNames.count();
                    for (int index = 0; index < numberOfPropertyNames; index++)
                    {
                        // Clean-up the name of the current property
                        QString propertyName = listOfPropertyNames.at(index).trimmed();

                        // Create a property
                        QQmlProperty property = QQmlProperty(_target, propertyName);

                        // Check if this property exists
                        if (property.isValid() && property.isProperty())
                        {
                            // Check if this property is writable
                            if (property.isWritable())
                            {
                                // Check if the type of our property is supported
                                if (MasticQuickBinding::checkIfPropertyIsSupported(property))
                                {
                                    // Save property
                                    _qmlPropertiesByName.insert(propertyName, property);
                                }
                                else
                                {
                                    qmlWarning(this) << "property '" << propertyName << "' on "
                                                     << MasticQuickBinding::prettyTypeName(_target)
                                                     << " has type '" << property.propertyTypeName()
                                                     << "' that is not supported by MasticQuick";

                                }
                            }
                            else
                            {
                                qmlWarning(this) << "property '" << propertyName << "' on "
                                                 << MasticQuickBinding::prettyTypeName(_target) << " is read-only";
                            }
                        }
                        else
                        {
                            qmlWarning(this) << "property '" << propertyName << "' does not exist on "
                                             << MasticQuickBinding::prettyTypeName(_target);
                        }
                    }
                }
            }
            // Else: no target or no properties => nothing to do
        }


        // Check if we have at least one valid property
        if (_qmlPropertiesByName.count() > 0)
        {
            foreach(const QString &propertyName, _qmlPropertiesByName.keys())
            {
                Q_UNUSED(propertyName)
            }
        }
        // Else: no valid property => nothing to do
    }
    // Else: our component is not completed yet, we do nothing to avoid useless computations
}
