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

#include "masticquickbindingsingleton.h"
#include "MasticQuick.h"



/**
 * @brief Default constructor
 * @param parent
 */
MasticQuickInputBinding::MasticQuickInputBinding(QObject *parent)
    : MasticQuickAbstractIOPBinding(parent, true)
{
}



/**
 * @brief Destructor
 */
MasticQuickInputBinding::~MasticQuickInputBinding()
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
 * @brief Set the prefix of Mastic inputs
 * @param value
 */
void MasticQuickInputBinding::setinputsPrefix(QString value)
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
 * @brief Set the suffix of Mastic inputs
 * @param value
 */
void MasticQuickInputBinding::setinputsSuffix(QString value)
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
 * @brief Set the name of our Mastic input
 * @remarks only when our component is used as a property value source OR if a single property is referenced by 'properties'
 *
 * @param value
 */
void MasticQuickInputBinding::setinputName(QString value)
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
 * @brief Called when a Mastic input changes
 * @param name
 * @param value
 */
void MasticQuickInputBinding::_onMasticObserveInput(QString name, QVariant value)
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
                                 << "' on " << prettyObjectTypeName(_target)
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
 * @brief QQmlParserStatus API: Invoked after the root component that caused this instantiation has completed construction.
 *        At this point all static values and binding values have been assigned to the class.
 */
void MasticQuickInputBinding::componentComplete()
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
    MasticQuickAbstractIOPBinding::componentComplete();
}




/**
 * @brief Connect to MasticQuick
 */
void MasticQuickInputBinding::_connectToMasticQuick()
{
    // Check if we have at least one valid Mastic input
    MasticQuick* masticQuick = MasticQuick::instance();
    if ((masticQuick != NULL) && (_qmlPropertiesByMasticInputName.count() > 0))
    {
        connect(masticQuick, &MasticQuick::observeInput, this, &MasticQuickInputBinding::_onMasticObserveInput, Qt::UniqueConnection);
    }
}



/**
 * @brief Disconnect to MasticQuick
 */
void MasticQuickInputBinding::_disconnectToMasticQuick()
{
    // Check if we have at least one valid Mastic input
    MasticQuick* masticQuick = MasticQuick::instance();
     if ((masticQuick != NULL) && (_qmlPropertiesByMasticInputName.count() > 0))
    {
        disconnect(masticQuick, &MasticQuick::observeInput, this, &MasticQuickInputBinding::_onMasticObserveInput);
    }
}



/**
 * @brief Clear internal data
 */
void MasticQuickInputBinding::_clearInternalData()
{
    // Clear our additional data
    _qmlPropertiesByMasticInputName.clear();
}



/**
 * @brief Update internal data
 */
void MasticQuickInputBinding::_updateInternalData()
{
    // Check if we have at least one valid property
    if (_qmlPropertiesByName.count() > 0)
    {
        MasticQuick* masticQuick = MasticQuick::instance();
        if (masticQuick != NULL)
        {
            // Trim prefix, suffix, inputName
            QString prefix = _inputsPrefix.trimmed();
            QString suffix = _inputsSuffix.trimmed();
            QString inputName = _inputName.trimmed();

            // Sort properties
            QList<QString> properties = _qmlPropertiesByName.keys();
            std::sort(properties.begin(), properties.end());

            // Try to create a Mastic input for each property
            foreach(const QString propertyName, properties)
            {
                // Get our property
                QQmlProperty property = _qmlPropertiesByName.value(propertyName);

                // Name of our Mastic input
                QString masticInputName;
                if (_isUsedAsQQmlPropertyValueSource)
                {
                    masticInputName = (inputName.isEmpty() ? propertyName : inputName);
                }
                else
                {
                    masticInputName = prefix + propertyName + suffix;
                }

                // Get MasticIOP type
                MasticIopType::Value masticIopType = getMasticIOPTypeForProperty(property);

                // Try to build a Mastic input
                bool succeeded = false;
                switch (masticIopType)
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
                            QString warning;
                            succeeded = masticQuick->createInputInt(masticInputName, cValue, &warning);
                            if (succeeded && !warning.isEmpty())
                            {
                                qmlWarning(this) << warning;
                            }
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
                            QString warning;
                            succeeded = masticQuick->createInputDouble(masticInputName, cValue, &warning);
                            if (succeeded && !warning.isEmpty())
                            {
                                qmlWarning(this) << warning;
                            }
                        }
                        break;

                    case MasticIopType::STRING:
                        {
                            // Get our initial value
                            QVariant qmlValue = property.read();

                            // Try to create a Mastic input
                            QString warning;
                            succeeded = masticQuick->createInputString(masticInputName, qmlValue.toString(), &warning);
                            if (succeeded && !warning.isEmpty())
                            {
                                qmlWarning(this) << warning;
                            }
                        }
                        break;

                    case MasticIopType::BOOLEAN:
                        {
                            // Get our initial value
                            QVariant qmlValue = property.read();

                            // Try to create a Mastic input
                            QString warning;
                            succeeded = masticQuick->createInputBool(masticInputName, qmlValue.toBool(), &warning);
                            if (succeeded && !warning.isEmpty())
                            {
                                qmlWarning(this) << warning;
                            }
                        }
                        break;

                    case MasticIopType::IMPULSION:
                        // Should not happen because QML properties can not have the type impulsion
                        break;

                    case MasticIopType::DATA:
                        {
                            // Try to create a Mastic input
                            QString warning;
                            succeeded = masticQuick->createInputData(masticInputName, NULL, &warning);
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
                    _qmlPropertiesByMasticInputName.insert(masticInputName, property);
                }
                else
                {
                    qmlWarning(this) << "failed to create Mastic input '" << masticInputName
                                     << "' with type=" << MasticIopType::staticEnumToString(masticIopType);
                }
            }
        }
    }
    // Else: no valid property => nothing to do
}



