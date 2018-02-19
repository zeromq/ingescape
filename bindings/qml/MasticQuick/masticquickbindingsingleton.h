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

    // Constant used to associated all signals to a MasticOutputBinding
    MASTIC_QML_PROPERTY_CONSTANT(QString, AllSignalHandlers)

    // Constant used to reset a binding
    MASTIC_QML_PROPERTY_CONSTANT(QString, None)


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


     /**
      * @brief Check if a given value is a keyword of our singleton
      * @param value
      * @return
      */
     bool isKeyword(QString value);


     /**
      * @brief Get the keyword associated to a given value
      * @param value
      * @return
      */
     QString getKeyword(QString value);

};

QML_DECLARE_TYPE(MasticQuickBindingSingleton)

#endif // _MASTICQUICKBINDINGSINGLETON_H_
