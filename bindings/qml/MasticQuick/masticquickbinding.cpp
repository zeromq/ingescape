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


#include "masticquickbinding.h"


//
// Define our singleton instance
//
Q_GLOBAL_STATIC(MasticQuickBinding, _singletonInstance)



/**
 * @brief Default constructor
 * @param parent
 */
MasticQuickBinding::MasticQuickBinding(QObject *parent) : QObject(parent),
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
 QObject* MasticQuickBinding::qmlSingleton(QQmlEngine* engine, QJSEngine* scriptEngine)
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
 MasticQuickBinding* MasticQuickBinding::instance()
 {
     return _singletonInstance();
 }



 /**
  * @brief Get the pretty type name of a given object
  * @param object
  * @return
  */
 QString MasticQuickBinding::prettyTypeName(QObject* object)
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
bool MasticQuickBinding::checkIfPropertyIsSupported(const QQmlProperty &property)
{
    bool result = false;

    // Check the type category
    if (property.propertyTypeCategory() == QQmlProperty::Normal)
    {
        QString propertyType = property.propertyTypeName();
        qDebug() << "type of " << property.name() << "is" << propertyType;
    }
    // Else: not yet implemented or not supported

    return result;
}


 /**
  * @brief Get the MasticIopType of a given property
  * @param property
  * @return
  */
MasticIopType::Value MasticQuickBinding::getPropertyType(const QQmlProperty &property)
{
    MasticIopType::Value result = MasticIopType::INVALID;

    if (property.type() == QQmlProperty::Property)
    {

    }
    // Else: It is not a valid property

    return result;
}
