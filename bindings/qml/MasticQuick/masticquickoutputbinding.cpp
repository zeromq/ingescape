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
    : MasticQuickAbstractIOPBinding(false, parent)
{
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

        // Update our component
        update();

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

        // Update our component
        update();

        // Notify change
        Q_EMIT outputsSuffixChanged(value);
    }
}



//-------------------------------------------------------------------
//
// Protected slots
//
//-------------------------------------------------------------------



/**
 * @brief Called when a QML property changes
 * @param name
 * @param value
 */
void MasticQuickOutputBinding::_onQmlPropertyChanged(QString name, QVariant value)
{
    Q_UNUSED(name)
    Q_UNUSED(value)

    // Check if our binding is active
    if (_when)
    {

    }
    // Else: our binding is not active
}




//-------------------------------------------------------------------
//
// Protected methods
//
//-------------------------------------------------------------------


/**
 * @brief Connect to MasticQuick
 */
void MasticQuickOutputBinding::_connectToMasticQuick()
{
}


/**
 * @brief Disconnect to MasticQuick
 */
void MasticQuickOutputBinding::_disconnectToMasticQuick()
{
}



/**
 * @brief Clear internal data
 */
void MasticQuickOutputBinding::_clearInternalData()
{
    // Clear our additional data

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
                                                 << " to create a Mastic output with type INTEGER";
                            }

                            // Try to create a Mastic output
                            succeeded = masticQuick->createOutputInt(masticOutputName, cValue);
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
                                                 << " to create a Mastic output with type DOUBLE";
                            }

                            // Try to create a Mastic output
                            succeeded = masticQuick->createOutputDouble(masticOutputName, cValue);
                        }
                        break;

                    case MasticIopType::STRING:
                        {
                            // Get our initial value
                            QVariant qmlValue = property.read();

                            // Try to create a Mastic output
                            succeeded = masticQuick->createOutputString(masticOutputName, qmlValue.toString());
                        }
                        break;

                    case MasticIopType::BOOLEAN:
                        {
                            // Get our initial value
                            QVariant qmlValue = property.read();

                            // Try to create a Mastic output
                            succeeded = masticQuick->createOutputBool(masticOutputName, qmlValue.toBool());
                        }
                        break;

                    case MasticIopType::IMPULSION:
                        // Should not happen because QML properties can not have the type impulsion
                        break;

                    case MasticIopType::DATA:
                        {
                            succeeded = masticQuick->createOutputData(masticOutputName, NULL);
                        }
                        break;

                    default:
                        break;
                }


                // Check if we have succeeded
                if (succeeded)
                {
                   // _qmlPropertiesByMasticInputName.insert(masticInputName, property);
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

