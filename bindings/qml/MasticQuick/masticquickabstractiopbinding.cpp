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

#include "masticquickabstractiopbinding.h"

#include <QDebug>

#include "masticquickbindingsingleton.h"
#include "MasticQuick.h"




/**
 * @brief Default constructor
 * @param parent
 */
MasticQuickAbstractIOPBinding::MasticQuickAbstractIOPBinding(QObject *parent)
    : QObject(parent),
    _target(NULL),
    _when(true),
    _isCompleted(false),
    _isUsedAsQQmlPropertyValueSource(false),
    _qmlPropertiesMustBeWritable(false)
{
}


/**
 * @brief Destructor
 */
MasticQuickAbstractIOPBinding::~MasticQuickAbstractIOPBinding()
{
    // Reset our isCompleted flag to be able to use setters
    _isCompleted = false;

    // Unsubscribe to our target
    if (_target != NULL)
    {
        settarget(NULL);
    }

    // Clear
    clear();
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
void MasticQuickAbstractIOPBinding::settarget(QObject *value)
{
    if (_target != value)
    {
        // Unsubscribe to our previous target if needed
        if (_target != NULL)
        {
            disconnect(_target, &QObject::destroyed, this, &MasticQuickAbstractIOPBinding::_ontargetDestroyed);
        }

        // Save our new value
        _target = value;

        // Subscribe to our new target if needed
        if (_target != NULL)
        {
            connect(_target, &QObject::destroyed, this, &MasticQuickAbstractIOPBinding::_ontargetDestroyed);
        }

        // Check if we can use this value
        if (!_isUsedAsQQmlPropertyValueSource)
        {
            // Update our component
            update();
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
void MasticQuickAbstractIOPBinding::setproperties(QString value)
{
    if (_properties != value)
    {
        // Save our new value
        _properties = value;

        // Check if we can use this value
        if (!_isUsedAsQQmlPropertyValueSource)
        {
            // Update our component
            update();
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
 * @brief Flag indicating if our binding is active or not
 * @param value
 */
void MasticQuickAbstractIOPBinding::setwhen(bool value)
{
    if (_when != value)
    {
        // Save our new value
        _when = value;

        // Connect or disconnect to MasticQuick
        connectOrDisconnectToMasticQuick();

        // Notify change
        Q_EMIT whenChanged(value);
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
void MasticQuickAbstractIOPBinding::_ontargetDestroyed(QObject *sender)
{
    Q_UNUSED(sender)

    // Reset our target
    _target = NULL;

    // Update our component
    update();

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
 *        with the following syntax    MasticXXXXXBinding on property { }
 *
 * @param property
 */
void MasticQuickAbstractIOPBinding::setTarget(const QQmlProperty &property)
{
    // Our component is used as a property value source
    _isUsedAsQQmlPropertyValueSource = true;

    // Check if we have a property
    if (property.isProperty())
    {
        // Check if we need a writable property
        if (!_qmlPropertiesMustBeWritable || property.isWritable())
        {
            // Check if the type of our property is supported
            if (MasticQuickBindingSingleton::checkIfPropertyIsSupported(property))
            {
                // Save this property
                _propertyValueSourceTarget = property;
            }
            else
            {
                // Reset value
                _propertyValueSourceTarget = QQmlProperty();

                qmlWarning(this) << "property '" << property.name() << "' has type '"
                                 << MasticQuickBindingSingleton::prettyPropertyTypeName(property)
                                 << "' that is not supported by MasticQuick";
            }
        }
        else
        {
            // Reset value
            _propertyValueSourceTarget = QQmlProperty();

            qmlWarning(this) << "can only be associated to a writable property - " << property.name() << " is read-only";
        }
    }
    else
    {
        // Reset value
        _propertyValueSourceTarget = QQmlProperty();

        qmlWarning(this) << "can only be associated to a property. " << property.name() << " is not a property";
    }

    // Update our component
    update();
}




/**
 * @brief QQmlParserStatus API: Invoked after class creation, but before any properties have been set
 */
void MasticQuickAbstractIOPBinding::classBegin()
{
}


/**
 * @brief QQmlParserStatus API: Invoked after the root component that caused this instantiation has completed construction.
 *        At this point all static values and binding values have been assigned to the class.
 */
void MasticQuickAbstractIOPBinding::componentComplete()
{
    // Check if everything is ok
    if (_isUsedAsQQmlPropertyValueSource)
    {
        // Check if "properties" is empty
        if (!_properties.isEmpty())
        {
            qmlWarning(this) << "properties can not be set when our item is used as a property value source (invalid value: "
                             << _properties << " )";
        }

        // Check if "target" is NULL
        if (_target != NULL)
        {
            qmlWarning(this) << "target can ot be set when our item is used as a property value source";
        }
    }

    // Our component is completed
    _isCompleted = true;


    // Update internal data
    update();
}



/**
 * @brief Update our component
 *
 * @remarks this function will call _updateInternalData()
 */
void MasticQuickAbstractIOPBinding::update()
{
    // Ensure that our component is completed
    if (_isCompleted)
    {
        // Clean-up previous data
        clear();


        //
        // Check if our item is used as as property value source or a standard QML item
        //
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
                if ((MasticQuickBindingSingleton::instance() != NULL) && (properties == MasticQuickBindingSingleton::instance()->AllProperties()))
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
                            // Check if we need a writable property
                            if (!_qmlPropertiesMustBeWritable || property.isWritable())
                            {
                                // Check if the type of our property is supported
                                if (MasticQuickBindingSingleton::checkIfPropertyIsSupported(property))
                                {
                                    // Save property
                                    _qmlPropertiesByName.insert(propertyName, property);
                                }
                                else
                                {
                                    qmlWarning(this) << "property '" << propertyName << "' on "
                                                     << MasticQuickBindingSingleton::prettyObjectTypeName(_target)
                                                     << " has type '" << MasticQuickBindingSingleton::prettyPropertyTypeName(property)
                                                     << "' that is not supported by MasticQuick";

                                }
                            }
                            else
                            {
                                qmlWarning(this) << "can only be associated to a writable property - property '" << propertyName << "' on "
                                                 << MasticQuickBindingSingleton::prettyObjectTypeName(_target) << " is read-only";
                            }
                        }
                        else
                        {
                            qmlWarning(this) << "property '" << propertyName << "' does not exist on "
                                             << MasticQuickBindingSingleton::prettyObjectTypeName(_target);
                        }
                    }
                }
            }
            // Else: no target or no properties => nothing to do
        }


        //
        // Do something with our list of QML properties
        //
        _updateInternalData();


        //
        // Connect to MasticQuick if needed
        //
        if (_when)
        {
            _connectToMasticQuick();
        }
    }
    // Else: our component is not completed yet, we do nothing to avoid useless computations
}



/**
 * @brief Clear our component
 *
 * @remarks this function will call _clearInternalData();
 */
void MasticQuickAbstractIOPBinding::clear()
{
    // Unsubscribe to MasticQuick
    _disconnectToMasticQuick();

    // Clear our hashtable
    _qmlPropertiesByName.clear();

    // Clear additional data
    _clearInternalData();
}



/**
 * @brief Manage connect/disconnect calls to associate our item to MasticQuick
 */
void MasticQuickAbstractIOPBinding::connectOrDisconnectToMasticQuick()
{
    if (_isCompleted)
    {
        if (_when)
        {
            _connectToMasticQuick();
        }
        else
        {
            _disconnectToMasticQuick();
        }
    }
}



/**
 * @brief Connect to MasticQuick
 */
void MasticQuickAbstractIOPBinding::_connectToMasticQuick()
{
}


/**
 * @brief Disconnect to MasticQuick
 */
void MasticQuickAbstractIOPBinding::_disconnectToMasticQuick()
{
}



/**
 * @brief Clear internal data
 */
void MasticQuickAbstractIOPBinding::_clearInternalData()
{
}



/**
 * @brief Update internal data
 */
void MasticQuickAbstractIOPBinding::_updateInternalData()
{
}





