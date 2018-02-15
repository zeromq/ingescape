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


#include "masticquickbindingsingleton.h"


//
// Define our singleton instance
//
Q_GLOBAL_STATIC(MasticQuickBindingSingleton, _singletonInstance)



//
// List of supported types
//
// List of supported types for MasticIopType.INTEGER
QList<QMetaType::Type> MasticQuickBindingSingleton::_supportedTypesForMasticIopTypeInteger = QList<QMetaType::Type>()
        << QMetaType::Int
        << QMetaType::UInt
        << QMetaType::Long
        << QMetaType::LongLong
        << QMetaType::Short
        << QMetaType::ULong
        << QMetaType::ULongLong
        << QMetaType::UShort
         ;

// List of supported types for MasticIopType.DOUBLE
QList<QMetaType::Type> MasticQuickBindingSingleton::_supportedTypesForMasticIopTypeDouble = QList<QMetaType::Type>()
        << QMetaType::Double
        << QMetaType::Float
         ;



//-------------------------------------------------------------------
//
//
//  MasticQuickBinding
//
//
//-------------------------------------------------------------------


/**
 * @brief Default constructor
 * @param parent
 */
MasticQuickBindingSingleton::MasticQuickBindingSingleton(QObject *parent) : QObject(parent),
    _AllProperties("/* ALL PROPERTIES */")
{
    // Force C++ ownership, otherwise our singleton will be owned by the QML engine
    QQmlEngine::setObjectOwnership(this, QQmlEngine::CppOwnership);
}



/**
 * @brief Method used to provide a singleton to QML
 * @param engine
 * @param scriptEngine
 * @return
 */
 QObject* MasticQuickBindingSingleton::qmlSingleton(QQmlEngine* engine, QJSEngine* scriptEngine)
 {
     Q_UNUSED(engine)
     Q_UNUSED(scriptEngine);

     // NOTE: our singleton is owned by C++ via Q_GLOBAL_STATIC
     return _singletonInstance();
 }



 /**
  * @brief Get our singleton instance
  * @return
  */
 MasticQuickBindingSingleton* MasticQuickBindingSingleton::instance()
 {
     return _singletonInstance();
 }



 /**
  * @brief Get the pretty type name of a given object
  * @param object
  * @return
  */
 QString MasticQuickBindingSingleton::prettyObjectTypeName(QObject* object)
 {
    QString result;

    if (object != NULL)
    {
        if (object->metaObject() != NULL)
        {
            result = object->metaObject()->className();
        }
        else if (!object->objectName().isEmpty())
        {
            result = QString("objectName=%1").arg(object->objectName());
        }
        else
        {
            result = "UNKNOWN_CLASS";
        }
    }

    return result;
 }



 /**
  * @brief Check if a given property is supported by Mastic
  * @param property
  * @return
  */
bool MasticQuickBindingSingleton::checkIfPropertyIsSupported(const QQmlProperty &property)
{
    bool result = false;

    // Check if we have a valid property
    if (property.type() == QQmlProperty::Property)
    {
        // Check its type category
        if (property.propertyTypeCategory() == QQmlProperty::Normal)
        {
            // Read value to check if we can convert its type to a supported type
            QVariant value = property.read();
            if (
                value.canConvert<int>()
                ||
                value.canConvert<QString>()
                ||
                value.canConvert<double>()
                ||
                value.canConvert<bool>()
                )
            {
                result = true;
            }
            // Else: not yet implemented or not supported
        }
        // Else: not yet implemented or not supported (QQmlProperty::Object, QQmlProperty::List)
    }
    // Else: invalid property or signal => nothing to do

    return result;
}



/**
 * @brief Get the pretty name of a given property
 * @param property
 * @return
 */
QString MasticQuickBindingSingleton::prettyPropertyTypeName(const QQmlProperty &property)
{
    QString result;

    // Check if we have a QVariant i.e. a generic container type
    if (property.propertyType() == QMetaType::QVariant)
    {
        // Read our value to get our real type
        QVariant value = property.read();
        if (value.type() == QVariant::Invalid)
        {
            result = QString("QVariant::Invalid");
        }
        else
        {
            result = QString("QVariant::%1").arg(value.typeName());
        }
    }

    else
    {
        result = property.propertyTypeName();
    }

    return result;
}



 /**
  * @brief Get the MasticIopType of a given property
  * @param property
  * @return
  *
  * @remarks we assume that checkIfPropertyIsSupported has been called before using this method
  */
MasticIopType::Value MasticQuickBindingSingleton::getMasticIOPTypeForProperty(const QQmlProperty &property)
{
    MasticIopType::Value result = MasticIopType::INVALID;

    // Ensure that we have a property
    if (property.type() == QQmlProperty::Property)
    {
        QMetaType::Type propertyType = static_cast<QMetaType::Type>(property.propertyType());

        if (propertyType == QMetaType::Bool)
        {
            result = MasticIopType::BOOLEAN;
        }
        else if (_supportedTypesForMasticIopTypeDouble.contains(propertyType))
        {
            result = MasticIopType::DOUBLE;
        }
        else if (_supportedTypesForMasticIopTypeInteger.contains(propertyType))
        {
            result = MasticIopType::INTEGER;
        }
        else
        {
            // Read value to check if we can convert its type to QString
            QVariant value = property.read();
            if (value.canConvert<QString>())
            {
                result = MasticIopType::STRING;
            }
        }
    }
    // Else: It is not a valid property

    return result;
}
