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

#ifndef _MASTICQUICKBINDING_H_
#define _MASTICQUICKBINDING_H_

#include <QObject>
#include <QtQml>

#include "masticquick_helpers.h"
#include "masticquick_enums.h"


/**
 * @brief The MasticQuickBinding class defines a singleton that provides constants
 */
class MasticQuickBinding : public QObject
{
    Q_OBJECT

    // Constant used to associated all properties to a MasticBinding (input or output)
    MASTIC_QML_PROPERTY_CONSTANT(QString, AllProperties)

public:
    /**
     * @brief Default constructor
     * @param parent
     */
    explicit MasticQuickBinding(QObject *parent = nullptr);


    /**
     * @brief Method used to provide a singleton to QML
     * @param engine
     * @param scriptEngine
     * @return
     */
     static QObject* qmlSingleton(QQmlEngine* engine, QJSEngine* scriptEngine);


     /**
      * @brief Get our singleton instance
      * @return
      */
     static MasticQuickBinding* instance();


     /**
      * @brief Get the pretty type name of a given object
      * @param object
      * @return
      */
     static QString prettyTypeName(QObject* object);


     /**
      * @brief Check if a given property is supported by Mastic
      * @param property
      * @return
      */
     static bool checkIfPropertyIsSupported(const QQmlProperty &property);


     /**
      * @brief Get the MasticIopType of a given property
      * @param property
      * @return
      */
     static MasticIopType::Value getPropertyType(const QQmlProperty &property);
};

QML_DECLARE_TYPE(MasticQuickBinding)

#endif // _MASTICQUICKBINDING_H_
