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
     static QString prettyObjectTypeName(QObject* object);


     /**
      * @brief Check if a given property is supported by Mastic
      * @param property
      * @return
      */
     static bool checkIfPropertyIsSupported(const QQmlProperty &property);


     /**
      * @brief Get the pretty name of a given property
      * @param property
      * @return
      */
     static QString prettyPropertyTypeName(const QQmlProperty &property);


     /**
      * @brief Get the MasticIopType of a given property
      * @param property
      * @return
      *
      * @remarks we assume that checkIfPropertyIsSupported has been called before using this method
      */
     static MasticIopType::Value getMasticIOPTypeForProperty(const QQmlProperty &property);


protected:
     // List of supported types for MasticIopType.INTEGER
     static QList<QMetaType::Type> _supportedTypesForMasticIopTypeInteger;

     // List of supported types for MasticIopType.DOUBLE
     static QList<QMetaType::Type> _supportedTypesForMasticIopTypeDouble;
};

QML_DECLARE_TYPE(MasticQuickBinding)

#endif // _MASTICQUICKBINDING_H_
