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

#include "masticquickoutputbinding.h"

#include <QDebug>

#include "masticquickbindingsingleton.h"
#include "MasticQuick.h"



/**
 * @brief Default constructor
 * @param parent
 */
MasticQuickOutputBinding::MasticQuickOutputBinding(QObject *parent) : QObject(parent),
    _target(NULL),
    _when(true),
    _isCompleted(false),
    _isUsedAsQQmlPropertyValueSource(false)
{
}



/**
 * @brief Destructor
 */
MasticQuickOutputBinding::~MasticQuickOutputBinding()
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
void MasticQuickOutputBinding::settarget(QObject *value)
{
    if (_target != value)
    {
        // Unsubscribe to our previous target if needed
        if (_target != NULL)
        {
            disconnect(_target, &QObject::destroyed, this, &MasticQuickOutputBinding::_ontargetDestroyed);
        }

        // Save our new value
        _target = value;

        // Subscribe to our new target if needed
        if (_target != NULL)
        {
            connect(_target, &QObject::destroyed, this, &MasticQuickOutputBinding::_ontargetDestroyed);
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
void MasticQuickOutputBinding::setproperties(QString value)
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
 * @brief Set the prefix of Mastic outputs
 * @param value
 */
void MasticQuickOutputBinding::setoutputsPrefix(QString value)
{
    if (_outputsPrefix != value)
    {
        // Save our new value
        _outputsPrefix = value;

        // Update internal data
        _update();

        // Notify change
        Q_EMIT outputsPrefixChanged(value);
    }
}



/**
 * @brief Set the suffix of Mastic outputs
 * @param value
 */
void MasticQuickOutputBinding::setoutputsSuffix(QString value)
{
    if (_outputsSuffix != value)
    {
        // Save our new value
        _outputsSuffix = value;

        // Update internal data
        _update();

        // Notify change
        Q_EMIT outputsSuffixChanged(value);
    }
}



/**
 * @brief Flag indicating if our binding is active or not
 * @param value
 */
void MasticQuickOutputBinding::setwhen(bool value)
{
    if (_when != value)
    {
        // Save our new value
        _when = value;

        // Connect or disconnect properties
        _connectOrDisconnectProperties();

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
void MasticQuickOutputBinding::_ontargetDestroyed(QObject *sender)
{
    Q_UNUSED(sender)

    // Reset our target
    _target = NULL;

    // Update internal data
    _update();

    // Notify that our target is destroyed
    Q_EMIT targetChanged(NULL);
}



/**
 * @brief Called when a QML property changes
 * @param name
 * @param value
 */
void MasticQuickOutputBinding::_onQmlPropertyChanged(QString name, QVariant value)
{
    // Check if our binding is active
    if (_when)
    {
        // Check if we are interested by this input
        if (_qmlPropertiesByMasticInputName.contains(name))
        {
            QQmlProperty property = _qmlPropertiesByMasticInputName.value(name);
            if (!property.write(value))
            {
                qmlWarning(this) << "failed to update property '" << property.name()
                                 << "' on " << MasticQuickBindingSingleton::prettyObjectTypeName(_target)
                                 << " binded to Mastic input '" << name << "' with value=" << value;
            }
        }
    }
    // Else: our binding is not active
}




//-------------------------------------------------------------------
//
// Protected methods
//
//-------------------------------------------------------------------



/**
 * @brief QQmlPropertyValueSource API: This method will be called by the QML engine when assigning a value source
 *        with the following syntax    MasticQuickOutputBinding on property { }
 *
 * @param property
 */
void MasticQuickOutputBinding::setTarget(const QQmlProperty &property)
{
    // Our component is used as a property value source
    _isUsedAsQQmlPropertyValueSource = true;

    // Check if we have a property
    if (property.isProperty())
    {
        // Check if this property is writable
        if (property.isWritable())
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
void MasticQuickOutputBinding::classBegin()
{
}


/**
 * @brief QQmlParserStatus API: Invoked after the root component that caused this instantiation has completed construction.
 *        At this point all static values and binding values have been assigned to the class.
 */
void MasticQuickOutputBinding::componentComplete()
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
void MasticQuickOutputBinding::_clear()
{


    // Clear our hashtables
    _qmlPropertiesByName.clear();
    _qmlPropertiesByMasticInputName.clear();
}



/**
 * @brief Update internal data
 */
void MasticQuickOutputBinding::_update()
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
                            // Check if this property is writable
                            if (property.isWritable())
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
                                qmlWarning(this) << "property '" << propertyName << "' on "
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


        // Check if we have at least one valid property
        if (_qmlPropertiesByName.count() > 0)
        {
            MasticQuick* masticQuick = MasticQuick::instance();
            if (masticQuick != NULL)
            {
                // Try to create a Mastic input for each property
                foreach(const QString propertyName, _qmlPropertiesByName.keys())
                {
                    // Get our property
                    QQmlProperty property = _qmlPropertiesByName.value(propertyName);

                    // Name of our Mastic output
                    QString masticOutputName = _outputsPrefix + propertyName + _outputsSuffix;

                    // Get MasticIOP type
                    MasticIopType::Value masticIopType = MasticQuickBindingSingleton::getMasticIOPTypeForProperty(property);

                    // Try to build a Mastic input
                    bool succeeded = false;
                    switch(masticIopType)
                    {
                        case MasticIopType::INVALID:
                            // Should not happen because we should have filter invalid properties
                            break;

                        case MasticIopType::INTEGER:
                            {
                                // Get our initial value
                                QVariant qmlValue = property.read();
                                bool ok = false;
                                int cValue = qmlValue.toInt(&ok);
                                if (!ok)
                                {
                                    cValue = 0;
                                    qmlWarning(this) << "invalid value " << qmlValue
                                                     << " to create a Mastic input with type INTEGER";
                                }

                                // Try to create a Mastic input
                                succeeded = masticQuick->createInputInt(masticOutputName, cValue);
                            }
                            break;

                        case MasticIopType::DOUBLE:
                            {
                                // Get our initial value
                                QVariant qmlValue = property.read();
                                bool ok = false;
                                double cValue = qmlValue.toDouble(&ok);
                                if (!ok)
                                {
                                    cValue = 0.0;
                                    qmlWarning(this) << "invalid value " << qmlValue
                                                     << " to create a Mastic input with type DOUBLE";
                                }

                                // Try to create a Mastic input
                                succeeded = masticQuick->createInputDouble(masticOutputName, cValue);
                            }
                            break;

                        case MasticIopType::STRING:
                            {
                                // Get our initial value
                                QVariant qmlValue = property.read();

                                // Try to create a Mastic input
                                succeeded = masticQuick->createInputString(masticOutputName, qmlValue.toString());
                            }
                            break;

                        case MasticIopType::BOOLEAN:
                            {
                                // Get our initial value
                                QVariant qmlValue = property.read();

                                // Try to create a Mastic input
                                succeeded = masticQuick->createInputBool(masticOutputName, qmlValue.toBool());
                            }
                            break;

                        case MasticIopType::IMPULSION:
                            // Should not happen because QML properties can not have the type impulsion
                            break;

                        case MasticIopType::DATA:
                            {
                                succeeded = masticQuick->createInputData(masticOutputName, NULL);
                            }
                            break;

                        default:
                            break;
                    }


                    // Check if we have succeeded
                    if (succeeded)
                    {
                        _qmlPropertiesByMasticInputName.insert(masticOutputName, property);
                    }
                    else
                    {
                        qmlWarning(this) << "failed to create Mastic input '" << masticOutputName
                                         << "' with type=" << MasticIopType::staticEnumToString(masticIopType);
                    }
                }


                // Check if we need to subscribe to MasticQuick
                if ((_qmlPropertiesByMasticInputName.count() > 0) && _when)
                {
                   // connect(masticQuick, &MasticQuick::observeInput, this, &MasticQuickOutputBinding::_onMasticObserveInput, Qt::UniqueConnection);
                }
            }
        }
        // Else: no valid property => nothing to do
    }
    // Else: our component is not completed yet, we do nothing to avoid useless computations
}



/**
 * @brief Manage connect/disconnect of our properties
 */
void MasticQuickOutputBinding::_connectOrDisconnectProperties()
{
    if (_isCompleted)
    {

    }
}
