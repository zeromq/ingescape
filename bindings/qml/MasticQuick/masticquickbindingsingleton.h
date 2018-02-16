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

#ifndef _MASTICQUICKBINDINGSINGLETON_H_
#define _MASTICQUICKBINDINGSINGLETON_H_

#include <QObject>
#include <QtQml>

#include "masticquick_helpers.h"
#include "masticquick_enums.h"


/**
 * @brief The MasticQuickBindingSingleton class defines a singleton that provides constants
 */
class MasticQuickBindingSingleton : public QObject
{
    Q_OBJECT

    // Constant used to associated all properties to a MasticBinding (input or output)
    MASTIC_QML_PROPERTY_CONSTANT(QString, AllProperties)

    // Constant used to reset a "properties" property
    MASTIC_QML_PROPERTY_CONSTANT(QString, NoProperty)


public:
    /**
     * @brief Default constructor
     * @param parent
     */
    explicit MasticQuickBindingSingleton(QObject *parent = nullptr);


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
     static MasticQuickBindingSingleton* instance();


};

QML_DECLARE_TYPE(MasticQuickBindingSingleton)

#endif // _MASTICQUICKBINDINGSINGLETON_H_
