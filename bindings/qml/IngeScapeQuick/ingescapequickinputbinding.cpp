/*
 *  IngeScape - QML binding
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

#include "ingescapequickinputbinding.h"

#include <QDebug>

#include "ingescapequickbindingsingleton.h"
#include "IngeScapeQuick.h"



/**
 * @brief Default constructor
 * @param parent
 */
IngeScapeQuickInputBinding::IngeScapeQuickInputBinding(QObject *parent)
    : IngeScapeQuickAbstractIOPBinding(parent, true)
{
}



/**
 * @brief Destructor
 */
IngeScapeQuickInputBinding::~IngeScapeQuickInputBinding()
{
    // Clear
    clear();
}



//-------------------------------------------------------------------
//
// Custom setters
//
//-------------------------------------------------------------------


/**
 * @brief Set the prefix of IngeScape inputs
 * @param value
 */
void IngeScapeQuickInputBinding::setinputsPrefix(QString value)
{
    if (_inputsPrefix != value)
    {
        // Save our new value
        _inputsPrefix = value;

        // Check if we can use this value
        if (!_isUsedAsQQmlPropertyValueSource)
        {
            // Update our component
            update();
        }
        else
        {
            qmlWarning(this) << "'inputsPrefix' can not be set when our item is used as a property value source (invalid value: '"
                              << _inputsPrefix << "' )";
        }

        // Notify change
        Q_EMIT inputsPrefixChanged(value);
    }
}



/**
 * @brief Set the suffix of IngeScape inputs
 * @param value
 */
void IngeScapeQuickInputBinding::setinputsSuffix(QString value)
{
    if (_inputsSuffix != value)
    {
        // Save our new value
        _inputsSuffix = value;

        // Check if we can use this value
        if (!_isUsedAsQQmlPropertyValueSource)
        {
            // Update our component
            update();
        }
        else
        {
            qmlWarning(this) << "'inputsSuffix' can not be set when our item is used as a property value source (invalid value: '"
                              << _inputsSuffix << "' )";
        }

        // Notify change
        Q_EMIT inputsSuffixChanged(value);
    }
}



/**
 * @brief Set the name of our IngeScape input
 * @remarks only when our component is used as a property value source OR if a single property is referenced by 'properties'
 *
 * @param value
 */
void IngeScapeQuickInputBinding::setinputName(QString value)
{
    if (_inputName != value)
    {
        // Save our new value
        _inputName = value;

        // Check if we can use this value
        if (_isUsedAsQQmlPropertyValueSource || !_isCompleted)
        {
            // Update our component
            update();
        }
        else
        {
            qmlWarning(this) << "'inputName' can not be set when our item is not used as a property value source (invalid value: '"
                              << _inputName << "' )";
        }

        // Notify change
        Q_EMIT inputNameChanged(value);
    }
}



//-------------------------------------------------------------------
//
// Protected slots
//
//-------------------------------------------------------------------


/**
 * @brief Called when a IngeScape input changes
 * @param name
 * @param value
 */
void IngeScapeQuickInputBinding::_onIngeScapeObserveInput(QString name, QVariant value)
{
    // Check if our binding is active
    if (_when)
    {
        // Check if we are interested by this input
        if (_qmlPropertiesByIngeScapeInputName.contains(name))
        {
            QQmlProperty property = _qmlPropertiesByIngeScapeInputName.value(name);
            if (!property.write(value))
            {
                qmlWarning(this) << "failed to update property '" << property.name()
                                 << "' on " << prettyObjectTypeName(_target)
                                 << " binded to IngeScape input '" << name << "' with value=" << value;
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
 * @brief QQmlParserStatus API: Invoked after the root component that caused this instantiation has completed construction.
 *        At this point all static values and binding values have been assigned to the class.
 */
void IngeScapeQuickInputBinding::componentComplete()
{
    // Check if everything is ok
    if (_isUsedAsQQmlPropertyValueSource)
    {
        // Check if "inputsPrefix" is empty
        if (!_inputsPrefix.isEmpty())
        {
            qmlWarning(this) << "'inputsPrefix' can not be set when our item is used as a property value source (invalid value: '"
                             << _inputsPrefix << "' )";
        }

        // Check if "inputsSuffix" is empty
        if (!_inputsSuffix.isEmpty())
        {
            qmlWarning(this) << "'inputsSuffix' can not be set when our item is used as a property value source (invalid value: '"
                             << _inputsSuffix << "' )";
        }
    }
    else
    {
        // Check if "inputName" is empty
        if (!_inputName.isEmpty())
        {
            qmlWarning(this) << "'inputName' can not be set when our item is not used as a property value source (invalid value: '"
                             << _inputName << "' )";
        }
    }

    // Call method of our parent class
    IngeScapeQuickAbstractIOPBinding::componentComplete();
}




/**
 * @brief Connect to IngeScapeQuick
 */
void IngeScapeQuickInputBinding::_connectToIngeScapeQuick()
{
    // Check if we have at least one valid IngeScape input
    IngeScapeQuick* ingescapeQuick = IngeScapeQuick::instance();
    if ((ingescapeQuick != NULL) && (_qmlPropertiesByIngeScapeInputName.count() > 0))
    {
        connect(ingescapeQuick, &IngeScapeQuick::observeInput, this, &IngeScapeQuickInputBinding::_onIngeScapeObserveInput, Qt::UniqueConnection);
    }
}



/**
 * @brief Disconnect to IngeScapeQuick
 */
void IngeScapeQuickInputBinding::_disconnectToIngeScapeQuick()
{
    // Check if we have at least one valid IngeScape input
    IngeScapeQuick* ingescapeQuick = IngeScapeQuick::instance();
     if ((ingescapeQuick != NULL) && (_qmlPropertiesByIngeScapeInputName.count() > 0))
    {
        disconnect(ingescapeQuick, &IngeScapeQuick::observeInput, this, &IngeScapeQuickInputBinding::_onIngeScapeObserveInput);
    }
}



/**
 * @brief Clear internal data
 */
void IngeScapeQuickInputBinding::_clearInternalData()
{
    //
    // Clear our additional data
    //

    // Check if we need to remove inputs
    if (_removeOnUpdatesAndDestruction)
    {
        IngeScapeQuick* ingescapeQuick = IngeScapeQuick::instance();
        if (ingescapeQuick != NULL)
        {
            for (QString inputName : _qmlPropertiesByIngeScapeInputName.keys())
            {
                if (!inputName.isEmpty())
                {
                    if (!ingescapeQuick->removeInput( inputName ))
                    {
                        qmlWarning(this) << "failed to remove input " << inputName;
                    }
                }
                // Else: should not happen. Otherwise, it means that we have stored an invalid data
            }
        }
    }

    // Clear our hashtable
    _qmlPropertiesByIngeScapeInputName.clear();
}



/**
 * @brief Update internal data
 */
void IngeScapeQuickInputBinding::_updateInternalData()
{
    // Check if we have at least one valid property
    if (_qmlPropertiesByName.count() > 0)
    {
        IngeScapeQuick* ingescapeQuick = IngeScapeQuick::instance();
        if (ingescapeQuick != NULL)
        {
            // Trim prefix, suffix, inputName
            QString prefix = _inputsPrefix.trimmed();
            QString suffix = _inputsSuffix.trimmed();
            QString inputName = _inputName.trimmed();

            // Sort properties
            QList<QString> properties = _qmlPropertiesByName.keys();
            std::sort(properties.begin(), properties.end());

            // Try to create a IngeScape input for each property
            for (QString propertyName : properties)
            {
                // Get our property
                QQmlProperty property = _qmlPropertiesByName.value(propertyName);

                // Name of our IngeScape input
                QString ingescapeInputName;
                if (_isUsedAsQQmlPropertyValueSource)
                {
                    ingescapeInputName = (inputName.isEmpty() ? propertyName : inputName);
                }
                else
                {
                    ingescapeInputName = prefix + propertyName + suffix;
                }

                // Get IngeScapeIOP type
                IngeScapeIopType::Value ingescapeIopType = getIngeScapeIOPTypeForProperty(property);

                // Try to build a IngeScape input
                bool succeeded = false;
                switch (ingescapeIopType)
                {
                    case IngeScapeIopType::INVALID:
                        // Should not happen because we should have filter invalid properties
                        break;

                    case IngeScapeIopType::INTEGER:
                        {
                            // Get our initial value
                            QVariant qmlValue = property.read();
                            bool ok = false;
                            int cValue = qmlValue.toInt(&ok);
                            if (!ok)
                            {
                                cValue = 0;
                                qmlWarning(this) << "invalid value " << qmlValue
                                                 << " to create a IngeScape input with type INTEGER";
                            }

                            // Try to create a IngeScape input
                            QString warning;
                            succeeded = ingescapeQuick->createInputInt(ingescapeInputName, cValue, &warning);
                            if (succeeded && !warning.isEmpty())
                            {
                                qmlWarning(this) << warning;
                            }
                            else if (!succeeded)
                            {
                               qmlWarning(this) << "error";
                            }
                        }
                        break;

                    case IngeScapeIopType::DOUBLE:
                        {
                            // Get our initial value
                            QVariant qmlValue = property.read();
                            bool ok = false;
                            double cValue = qmlValue.toDouble(&ok);
                            if (!ok)
                            {
                                cValue = 0.0;
                                qmlWarning(this) << "invalid value " << qmlValue
                                                 << " to create a IngeScape input with type DOUBLE";
                            }

                            // Try to create a IngeScape input
                            QString warning;
                            succeeded = ingescapeQuick->createInputDouble(ingescapeInputName, cValue, &warning);
                            if (succeeded && !warning.isEmpty())
                            {
                                qmlWarning(this) << warning;
                            }
                            else if (!succeeded)
                            {
                               qmlWarning(this) << "error";
                            }
                        }
                        break;

                    case IngeScapeIopType::STRING:
                        {
                            // Get our initial value
                            QVariant qmlValue = property.read();

                            // Try to create a IngeScape input
                            QString warning;
                            succeeded = ingescapeQuick->createInputString(ingescapeInputName, qmlValue.toString(), &warning);
                            if (succeeded && !warning.isEmpty())
                            {
                                qmlWarning(this) << warning;
                            }
                        }
                        break;

                    case IngeScapeIopType::BOOLEAN:
                        {
                            // Get our initial value
                            QVariant qmlValue = property.read();

                            // Try to create a IngeScape input
                            QString warning;
                            succeeded = ingescapeQuick->createInputBool(ingescapeInputName, qmlValue.toBool(), &warning);
                            if (succeeded && !warning.isEmpty())
                            {
                                qmlWarning(this) << warning;
                            }
                        }
                        break;

                    case IngeScapeIopType::IMPULSION:
                        // Should not happen because QML properties can not have the type impulsion
                        break;

                    case IngeScapeIopType::DATA:
                        {
                            // Try to create a IngeScape input
                            QString warning;
                            succeeded = ingescapeQuick->createInputData(ingescapeInputName, NULL, &warning);
                            if (succeeded && !warning.isEmpty())
                            {
                                qmlWarning(this) << warning;
                            }
                        }
                        break;

                    default:
                        break;
                }


                // Check if we have succeeded
                if (succeeded)
                {
                    _qmlPropertiesByIngeScapeInputName.insert(ingescapeInputName, property);
                }
                else
                {
                    qmlWarning(this) << "failed to create IngeScape input '" << ingescapeInputName
                                     << "' with type=" << IngeScapeIopType::staticEnumToString(ingescapeIopType);
                }
            }
        }
    }
    // Else: no valid property => nothing to do
}



