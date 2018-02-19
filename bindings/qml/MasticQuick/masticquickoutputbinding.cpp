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
MasticQuickOutputBinding::MasticQuickOutputBinding(QObject *parent)
    : MasticQuickAbstractIOPBinding(parent, false)
{
    // Get index of our callback called when a QML property changed
    const QMetaObject* myMetaObject = metaObject();
    if (myMetaObject != NULL)
    {
        _onQmlPropertyChangedMetaMethod = myMetaObject->method(myMetaObject->indexOfMethod("_onQmlPropertyChanged()"));
    }
    else
    {
        qmlWarning(this) << "failed to get metaObject";
    }
}



/**
 * @brief Destructor
 */
MasticQuickOutputBinding::~MasticQuickOutputBinding()
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
 * @brief Set the prefix of Mastic outputs
 * @param value
 */
void MasticQuickOutputBinding::setoutputsPrefix(QString value)
{
    if (_outputsPrefix != value)
    {
        // Save our new value
        _outputsPrefix = value;

        // Check if we can use this value
        if (!_isUsedAsQQmlPropertyValueSource)
        {
            // Update our component
            update();
        }
        else
        {
            qmlWarning(this) << "'outputsPrefix' can not be set when our item is used as a property value source (invalid value: '"
                              << _outputsPrefix << "' )";
        }

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

        // Check if we can use this value
        if (!_isUsedAsQQmlPropertyValueSource)
        {
            // Update our component
            update();
        }
        else
        {
            qmlWarning(this) << "'outputsSuffix' can not be set when our item is used as a property value source (invalid value: '"
                              << _outputsSuffix << "' )";
        }

        // Notify change
        Q_EMIT outputsSuffixChanged(value);
    }
}



/**
 * @brief Set the name of our Mastic output
 * @remarks only when our component is used as a property value source OR if a single property is referenced by 'properties'
 *
 * @param value
 */
void MasticQuickOutputBinding::setoutputName(QString value)
{
    if (_outputName != value)
    {
        // Save our new value
        _outputName = value;

        // Check if we can use this value
        if (_isUsedAsQQmlPropertyValueSource || !_isCompleted)
        {
            // Update our component
            update();
        }
        else
        {
            qmlWarning(this) << "'outputName' can not be set when our item is not used as a property value source (invalid value: '"
                              << _outputName << "' )";
        }

        // Notify change
        Q_EMIT outputNameChanged(value);
    }
}




//-------------------------------------------------------------------
//
// Protected slots
//
//-------------------------------------------------------------------



/**
 * @brief Called when a QML property changes
 */
void MasticQuickOutputBinding::_onQmlPropertyChanged()
{
    // Check if our binding is active
    if (_when)
    {
        // Get signal index
        int signalIndex = senderSignalIndex();

        // Check if it is our our hashtable
        if (_qmlPropertiesByNotifySignalIndex.contains(signalIndex))
        {
            // Get our QML property
            QQmlProperty property = _qmlPropertiesByNotifySignalIndex.value(signalIndex);

            // Get its Mastic output
            if (_masticOutputsByQmlProperty.contains(property))
            {
                // Get info about our Mastic output
                QPair<QString, MasticIopType::Value> masticOutputInfo = _masticOutputsByQmlProperty.value(property);
                QString masticOutputName = masticOutputInfo.first;
                MasticIopType::Value masticIopType = masticOutputInfo.second;

                // Get value of our property
                QVariant qmlValue = property.read();

                // Try to update our Mastic output
                bool succeeded = false;
                switch (masticIopType)
                {
                    case MasticIopType::INVALID:
                        // Should not happen because we should have filter invalid properties
                        break;

                    case MasticIopType::INTEGER:
                        {
                            // Convert value to int
                            bool ok = false;
                            int value = qmlValue.toInt(&ok);

                            // Update our Mastic output if everything is ok
                            if (ok)
                            {
                                MasticQuick* masticQuick = MasticQuick::instance();
                                if (masticQuick != NULL)
                                {
                                    succeeded = masticQuick->writeOutputAsInt(masticOutputName, value);
                                }
                            }
                        }
                        break;

                    case MasticIopType::DOUBLE:
                        {
                            // Convert value to double
                            bool ok = false;
                            double value = qmlValue.toDouble(&ok);

                            // Update our Mastic output if everything is ok
                            if (ok)
                            {
                                MasticQuick* masticQuick = MasticQuick::instance();
                                if (masticQuick != NULL)
                                {
                                    succeeded = masticQuick->writeOutputAsDouble(masticOutputName, value);
                                }
                            }
                        }
                        break;

                    case MasticIopType::STRING:
                        {
                             // Update our Mastic output
                             MasticQuick* masticQuick = MasticQuick::instance();
                             if (masticQuick != NULL)
                             {
                                succeeded = masticQuick->writeOutputAsString(masticOutputName, qmlValue.toString());
                             }
                        }
                        break;

                    case MasticIopType::BOOLEAN:
                        {
                             // Update our Mastic output
                             MasticQuick* masticQuick = MasticQuick::instance();
                             if (masticQuick != NULL)
                             {
                                succeeded = masticQuick->writeOutputAsBool(masticOutputName, qmlValue.toBool());
                             }
                        }
                        break;

                    case MasticIopType::IMPULSION:
                        // Should not happen because QML properties can not have the type impulsion
                        break;

                    case MasticIopType::DATA:
                        {
                            qmlWarning(this) << "can not update a Mastic output with type DATA (not yet implemented)";
                        }
                        break;

                    default:
                        break;
                }

                // Warning message if needed
                if (!succeeded)
                {
                    qmlWarning(this) << "failed to update Mastic output '" << masticOutputName
                                     << "binded to property '" << property.name()
                                     << "' on " << prettyObjectTypeName(_target)
                                     << " with value=" << qmlValue;
                }
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
void MasticQuickOutputBinding::componentComplete()
{
    // Check if everything is ok
    if (_isUsedAsQQmlPropertyValueSource)
    {
        // Check if "oututsPrefix" is empty
        if (!_outputsPrefix.isEmpty())
        {
            qmlWarning(this) << "'outputsPrefix' can not be set when our item is used as a property value source (invalid value: '"
                             << _outputsPrefix << "' )";
        }

        // Check if "outputsSuffix" is empty
        if (!_outputsSuffix.isEmpty())
        {
            qmlWarning(this) << "'outputsSuffix' can not be set when our item is used as a property value source (invalid value: '"
                             << _outputsSuffix << "' )";
        }
    }
    else
    {
        // Check if "outputName" is empty
        if (!_outputName.isEmpty())
        {
            qmlWarning(this) << "'outputName' can not be set when our item is not used as a property value source (invalid value: '"
                             << _outputName << "' )";
        }
    }

    // Call method of our parent class
    MasticQuickAbstractIOPBinding::componentComplete();
}



/**
 * @brief Connect to MasticQuick
 */
void MasticQuickOutputBinding::_connectToMasticQuick()
{
    // Check if we have at least one valid Mastic output
    if (_masticOutputsByQmlProperty.count() > 0)
    {
        for (QQmlProperty property : _masticOutputsByQmlProperty.keys())
        {
            // Check if our property has a notify signal
            if (property.hasNotifySignal())
            {
                //
                // NB: we can not use QQmlProperty::connectNotifySignal because
                //     there is no public way to call disconnect
                //

                // Get its object
                QObject* propertyObject = property.object();
                if (propertyObject != NULL)
                {
                    // Get its meta object
                    const QMetaObject* metaObject = propertyObject->metaObject();
                    if (metaObject != NULL)
                    {
                        // Get index of our property
                        int propertyIndex = property.index();

                        // Get its meta property
                        QMetaProperty metaProperty = metaObject->property(propertyIndex);
                        if (metaProperty.hasNotifySignal())
                        {
                            // Get its notify signal index
                            int notifySignalIndex = metaProperty.notifySignalIndex();

                            // Save it
                            _qmlPropertiesByNotifySignalIndex.insert(notifySignalIndex, property);

                            // Subscribe to this signal
                            QMetaMethod notifySignal = metaObject->method(notifySignalIndex);

                            // NB: Qt::UniqueConnection to ensure that we only subscribe once to each property
                            connect (propertyObject, notifySignal, this, _onQmlPropertyChangedMetaMethod, Qt::UniqueConnection);
                        }
                        // Else: should not happen because our QQmlProperty has a notify signal
                    }
                    // Else: should not happen
                }
                // Else: should not happen, otherwise our property is invalid
            }
            // Else: nothing to do
        }
    }
}



/**
 * @brief Disconnect to MasticQuick
 */
void MasticQuickOutputBinding::_disconnectToMasticQuick()
{
    // Check if we have at least one coonection to a QML property
    if (_qmlPropertiesByNotifySignalIndex.count() > 0)
    {
        for(int notifySignalIndex : _qmlPropertiesByNotifySignalIndex.keys())
        {
            // Get our porperty
            QQmlProperty property = _qmlPropertiesByNotifySignalIndex.value(notifySignalIndex);

            // Get its object
            QObject* propertyObject = property.object();
            if (propertyObject != NULL)
            {
                // Get its meta object
                const QMetaObject* metaObject = propertyObject->metaObject();
                if (metaObject != NULL)
                {
                    // Unsubscribe to this signal
                    QMetaMethod notifySignal = metaObject->method(notifySignalIndex);
                    disconnect (propertyObject, notifySignal, this, _onQmlPropertyChangedMetaMethod);
                }
                // Else: should not happen
            }
            // Else: should not happen, otherwise our property is invalid
        }
    }
}



/**
 * @brief Clear internal data
 */
void MasticQuickOutputBinding::_clearInternalData()
{
    // Clear our additional data
    _qmlPropertiesByNotifySignalIndex.clear();
    _masticOutputsByQmlProperty.clear();

}



/**
 * @brief Update internal data
 */
void MasticQuickOutputBinding::_updateInternalData()
{
    // Check if we have at least one valid property
    if (_qmlPropertiesByName.count() > 0)
    {
        MasticQuick* masticQuick = MasticQuick::instance();
        if (masticQuick != NULL)
        {
            // Trim prefix, suffix, outputName
            QString prefix = _outputsPrefix.trimmed();
            QString suffix = _outputsSuffix.trimmed();
            QString outputName = _outputName.trimmed();

            // Try to create a Mastic input for each property
            foreach(const QString propertyName, _qmlPropertiesByName.keys())
            {
                // Get our property
                QQmlProperty property = _qmlPropertiesByName.value(propertyName);

                // Name of our Mastic output
                QString masticOutputName;
                if (_isUsedAsQQmlPropertyValueSource)
                {
                    masticOutputName = (outputName.isEmpty() ? propertyName : outputName);
                }
                else
                {
                    masticOutputName = prefix + propertyName + suffix;
                }

                // Get MasticIOP type
                MasticIopType::Value masticIopType = getMasticIOPTypeForProperty(property);

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
                                                 << " to create a Mastic output with type INTEGER."
                                                 << " Mastic output '" << masticOutputName << "' will be initialized with 0";
                            }

                            // Try to create a Mastic output
                            QString warning;
                            succeeded = masticQuick->createOutputInt(masticOutputName, cValue, &warning);
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
                                                 << " to create a Mastic output with type DOUBLE."
                                                 << " Mastic output '" << masticOutputName << "' will be initialized with 0.0";
                            }

                            // Try to create a Mastic output
                            QString warning;
                            succeeded = masticQuick->createOutputDouble(masticOutputName, cValue, &warning);
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

                            // Try to create a Mastic output
                            QString warning;
                            succeeded = masticQuick->createOutputString(masticOutputName, qmlValue.toString(), &warning);
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

                            // Try to create a Mastic output
                            QString warning;
                            succeeded = masticQuick->createOutputBool(masticOutputName, qmlValue.toBool(), &warning);
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
                            // Try to create a Mastic output
                            QString warning;
                            succeeded = masticQuick->createOutputData(masticOutputName, NULL, &warning);
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
                    _masticOutputsByQmlProperty.insert(property, QPair<QString, MasticIopType::Value>(masticOutputName, masticIopType));
                }
                else
                {
                    qmlWarning(this) << "failed to create Mastic output '" << masticOutputName
                                     << "' with type=" << MasticIopType::staticEnumToString(masticIopType);
                }
            }
        }
    }
    // Else: no valid property => nothing to do
}

