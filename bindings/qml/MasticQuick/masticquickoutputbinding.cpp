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
        _onQmlPropertySignalMetaMethod = myMetaObject->method(myMetaObject->indexOfMethod("_onQmlPropertySignal()"));
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



/**
 * @brief Set our list of signal handlers
 * @param value
 */
void MasticQuickOutputBinding::setsignalHandlers(QString value)
{
    if (_signalHandlers != value)
    {
        // Save our new value
        _signalHandlers = value;

        // Check if we can use this value
        if (!_isUsedAsQQmlPropertyValueSource)
        {
            // Update our component
            update();
        }
        else
        {
            qmlWarning(this) << "'signalHandlers' can not be set when our item is used as a property value source (invalid value: '"
                              << _signalHandlers << "' )";
        }

        // Notify change
        Q_EMIT signalHandlersChanged(value);
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
void MasticQuickOutputBinding::_onQmlPropertySignal()
{
    // Check if our binding is active
    if (_when)
    {
        // Get MasticQuick
        MasticQuick* masticQuick = MasticQuick::instance();
        if (masticQuick != NULL)
        {
            // Get signal index
            int signalIndex = senderSignalIndex();

            // Check if this signal index is our our hashtable
            if (_qmlPropertiesByNotifySignalIndex.contains(signalIndex))
            {
                // Get our QML property
                QQmlProperty qmlProperty = _qmlPropertiesByNotifySignalIndex.value(signalIndex);

                // Get its Mastic output
                if (_masticOutputsByQmlProperty.contains(qmlProperty))
                {
                    // Get info about our Mastic output
                    QPair<QString, MasticIopType::Value> masticOutputInfo = _masticOutputsByQmlProperty.value(qmlProperty);
                    QString masticOutputName = masticOutputInfo.first;
                    MasticIopType::Value masticIopType = masticOutputInfo.second;

                    // Get value of our property if needed
                    QVariant qmlValue;
                    if (masticIopType != MasticIopType::IMPULSION)
                    {
                        qmlValue = qmlProperty.read();
                    }

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
                                    succeeded = masticQuick->writeOutputAsInt(masticOutputName, value);
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
                                    succeeded = masticQuick->writeOutputAsDouble(masticOutputName, value);
                                }
                            }
                            break;

                        case MasticIopType::STRING:
                            {
                                 // Update our Mastic output
                                 succeeded = masticQuick->writeOutputAsString(masticOutputName, qmlValue.toString());
                            }
                            break;

                        case MasticIopType::BOOLEAN:
                            {
                                 // Update our Mastic output
                                 succeeded = masticQuick->writeOutputAsBool(masticOutputName, qmlValue.toBool());
                            }
                            break;

                        case MasticIopType::IMPULSION:
                            {
                                // Update our Mastic output
                                succeeded = masticQuick->writeOutputAsImpulsion(masticOutputName);
                            }
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
                        if (qmlProperty.isProperty())
                        {
                            qmlWarning(this) << "failed to update Mastic output '" << masticOutputName
                                             << "binded to property '" << qmlProperty.name()
                                             << "' on " << prettyObjectTypeName(_target)
                                             << " with value=" << qmlValue;
                        }
                        else if (qmlProperty.isSignalProperty())
                        {
                            qmlWarning(this) << "failed to update Mastic output '" << masticOutputName
                                             << "binded to signal handler '" << qmlProperty.name()
                                             << "' of " << prettyObjectTypeName(_target);
                        }
                    }
                }
            }

        }
        // Else: should not happen. Otherwise, it means that QML has destroyed MasticQuick
        //       Thus, it means that our application should have quit
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

        // Check if "signalHandlers" is empty
        if (!_signalHandlers.isEmpty())
        {
            qmlWarning(this) << "'signalHandlers' can not be set when our item is used as a property value source (invalid value: '"
                             << _signalHandlers << "' )";
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
        for (QQmlProperty qmlProperty : _masticOutputsByQmlProperty.keys())
        {
            // Check if we have a property
            if (qmlProperty.isProperty())
            {
                // Check if our property has a notify signal
                if (qmlProperty.hasNotifySignal())
                {
                    //
                    // NB: we can not use QQmlProperty::connectNotifySignal because
                    //     there is no public way to call disconnect
                    //

                    // Get its object
                    QObject* propertyObject = qmlProperty.object();
                    if (propertyObject != NULL)
                    {
                        // Get its meta object
                        const QMetaObject* metaObject = propertyObject->metaObject();
                        if (metaObject != NULL)
                        {
                            // Get index of our property
                            int propertyIndex = qmlProperty.index();

                            // Get its meta property
                            QMetaProperty metaProperty = metaObject->property(propertyIndex);
                            if (metaProperty.hasNotifySignal())
                            {
                                // Get its notify signal index
                                int notifySignalIndex = metaProperty.notifySignalIndex();

                                // Save it
                                _qmlPropertiesByNotifySignalIndex.insert(notifySignalIndex, qmlProperty);

                                // Subscribe to this signal
                                QMetaMethod notifySignal = metaObject->method(notifySignalIndex);

                                // NB: Qt::UniqueConnection to ensure that we only subscribe once to each property
                                connect (propertyObject, notifySignal, this, _onQmlPropertySignalMetaMethod, Qt::UniqueConnection);
                            }
                            // Else: should not happen because our QQmlProperty has a notify signal
                        }
                        // Else: should not happen
                    }
                    // Else: should not happen, otherwise our property is invalid
                }
                // Else: our property does not have a notify signal. Thus, we can not subscribe to its changes
            }
            // Check if we have a signal
            else if (qmlProperty.isSignalProperty())
            {
                // Get its object
                QObject* propertyObject = qmlProperty.object();
                if (propertyObject != NULL)
                {
                    // Get our signal index
                    int notifySignalIndex = qmlProperty.index();

                    // Save it
                    _qmlPropertiesByNotifySignalIndex.insert(notifySignalIndex, qmlProperty);

                    // Subscribe to this signal
                    QMetaMethod notifySignal = qmlProperty.method();

                    // NB: Qt::UniqueConnection to ensure that we only subscribe once to each signal
                    connect (propertyObject, notifySignal, this, _onQmlPropertySignalMetaMethod, Qt::UniqueConnection);
                }
                // Else: should not happen, otherwise our property is invalid
            }
        }
        // End for (QQmlProperty qmlProperty : _masticOutputsByQmlProperty.keys())
    }
}



/**
 * @brief Disconnect to MasticQuick
 */
void MasticQuickOutputBinding::_disconnectToMasticQuick()
{
    // Check if we have at least one connection to a QML property
    if (_qmlPropertiesByNotifySignalIndex.count() > 0)
    {
        for (int notifySignalIndex : _qmlPropertiesByNotifySignalIndex.keys())
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
                    disconnect (propertyObject, notifySignal, this, _onQmlPropertySignalMetaMethod);
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
    //
    // Clear our additional data
    //

    // Check if we need to remove outputs
    if (_removeOnUpdatesAndDestruction)
    {
        MasticQuick* masticQuick = MasticQuick::instance();
        if (masticQuick != NULL)
        {
            for ( QPair<QString, MasticIopType::Value> masticOutputInfo : _masticOutputsByQmlProperty.values())
            {
                QString outputName = masticOutputInfo.first;

                if (!masticQuick->removeOutput( outputName ))
                {
                    qmlWarning(this) << "failed to remove output " << outputName;
                }
            }
        }
    }

    // Clear our hashtables
    _qmlPropertiesByNotifySignalIndex.clear();
    _masticOutputsByQmlProperty.clear();

}



/**
 * @brief Update internal data
 */
void MasticQuickOutputBinding::_updateInternalData()
{
    // Get our MasticQuick instance
    MasticQuick* masticQuick = MasticQuick::instance();
    if (masticQuick != NULL)
    {
        // Trim prefix and suffix
        QString prefix = _outputsPrefix.trimmed();
        QString suffix = _outputsSuffix.trimmed();




        //
        // Check if we have at least one valid QML property
        //
        if (_qmlPropertiesByName.count() > 0)
        {
            // Trim outputName
            QString outputName = _outputName.trimmed();

            // Sort properties
            QList<QString> properties = _qmlPropertiesByName.keys();
            std::sort(properties.begin(), properties.end());


            // Try to create a Mastic input for each property
            for (QString propertyName : properties)
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
        // Else: no valid QML property => nothing to do


        //
        // Check if we have a list of QML signal handlers
        //
        QString signalHandlers = _signalHandlers.trimmed();
        if ((_target != NULL) && !signalHandlers.isEmpty())
        {
            //
            // Build our list of signal handlers
            //
            QStringList listOfSignalHandlerNames;

            // Check if its a keyword of MasticQuickBindingSingleton
            MasticQuickBindingSingleton* masticQuickBindingSingleton = MasticQuickBindingSingleton::instance();
            if ((masticQuickBindingSingleton != NULL) && masticQuickBindingSingleton->isKeyword(signalHandlers))
            {
                if (signalHandlers == masticQuickBindingSingleton->AllSignalHandlers())
                {
                    // Get meta object of our target
                    const QMetaObject* metaObject = _target->metaObject();
                    if (metaObject != NULL)
                    {
                        // Get the number of methods
                        int numberOfMethods = metaObject->methodCount();
                        for (int index = 0; index < numberOfMethods; index++)
                        {
                            // Check if this method is a valid signal
                            QMetaMethod metaMethod = metaObject->method(index);
                            QString methodName = metaMethod.name();
                            if (
                                (metaMethod.methodType() == QMetaMethod::Signal)
                                &&
                                !methodName.isEmpty()
                                &&
                                !_signalsExcludedFromIntrospection.contains(methodName)
                               )
                            {
                                // Build name of our signal handler
                                methodName[0] = methodName[0].toUpper();
                                QString signalHandlerName = QString("on%1").arg(methodName);

                                // Add it to our list if needed
                                if (!listOfSignalHandlerNames.contains(signalHandlerName))
                                {
                                    listOfSignalHandlerNames.append(signalHandlerName);
                                }
                            }
                        }
                    }
                    // Else: should not happen because a QObject always has a meta object
                }
                else if (signalHandlers == masticQuickBindingSingleton->None())
                {
                    // No signal handlers => nothing to do
                }
                else
                {
                    qmlWarning(this) << "invalid value 'MasticBinding." << masticQuickBindingSingleton->getKeyword(signalHandlers)
                                      << "' for 'signalHandlers'";
                }
            }
            else
            {
                //
                // Specific set of signal handlers
                //
                listOfSignalHandlerNames = signalHandlers.split(QLatin1Char(','));
            }


            //
            // Check if we have at least one signal handler
            //
            int numberOfSignalHandlerNames = listOfSignalHandlerNames.count();
            if (numberOfSignalHandlerNames > 0)
            {
                // Sort our list
                std::sort(listOfSignalHandlerNames.begin(), listOfSignalHandlerNames.end());


                // Try to create a Mastic output of type IMPULSION for each signal handler
                for (int index = 0; index < numberOfSignalHandlerNames; index++)
                {
                    // Get name of our current signal handler
                    QString signalHandlerName = listOfSignalHandlerNames.at(index).trimmed();

                    // Check if we have a valid signal name
                    if (
                        // The "onXXX" syntax requires at least 3 characters
                        (signalHandlerName.length() < 3)
                        ||
                        // A QML signal always starts with "on"
                        !signalHandlerName.startsWith(QLatin1String("on"))
                        ||
                        // The third letter of a QML signal is always uppercase
                        !signalHandlerName.at(2).isUpper()
                        )
                    {
                        qmlWarning(this) << "invalid value in 'signalHandler' - '" << signalHandlerName << "' is not a signal handler"
                                         << " - signal handlers are named on<Signal> where <Signal> is the name of a signal with the first letter capitalized.";
                    }
                    else
                    {
                        //
                        // We have a valid signal name
                        //

                        // Create a QML property
                        QQmlProperty qmlProperty = QQmlProperty(_target, signalHandlerName);
                        if (qmlProperty.isValid() && qmlProperty.isSignalProperty())
                        {
                            // Name of our Mastic output
                            QString masticOutputName = prefix + signalHandlerName + suffix;


                            // Try to create a Mastic output
                            QString warning;
                            bool succeeded = masticQuick->createOutputImpulsion(masticOutputName, &warning);
                            if (succeeded)
                            {
                                // Print a QML warning if needed
                                if (!warning.isEmpty())
                                {
                                    qmlWarning(this) << warning;
                                }

                                // Save it
                                _masticOutputsByQmlProperty.insert(qmlProperty, QPair<QString, MasticIopType::Value>(masticOutputName, MasticIopType::IMPULSION));
                            }
                            else
                            {
                                qmlWarning(this) << "failed to create Mastic output '" << masticOutputName
                                                 << "' with type IMPULSION";
                            }
                        }
                        else
                        {
                            qmlWarning(this) << "invalid value in 'signalHandler' - '" << signalHandlerName << "' is not a signal handler";
                        }
                    }
                }
            }
            // Else: empty list of signal handlers => nothing to do
        }
        // Else: empty 'signalHandlers' property => nothing to do
    }
    // Else: MasticQuick does not exist => should not happen
}

