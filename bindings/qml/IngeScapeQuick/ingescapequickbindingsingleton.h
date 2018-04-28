/*
 *  IngeScape - QML binding
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

#ifndef _INGESCAPEQUICKBINDINGSINGLETON_H_
#define _INGESCAPEQUICKBINDINGSINGLETON_H_

#include <QObject>
#include <QtQml>

#include "ingescapequick_helpers.h"
#include "ingescapequick_enums.h"


/**
 * @brief The IngeScapeQuickBindingSingleton class defines a singleton that provides constants
 */
class IngeScapeQuickBindingSingleton : public QObject
{
    Q_OBJECT

    // Constant used to associated all properties to a IngeScapeBinding (input or output)
    INGESCAPE_QML_PROPERTY_CONSTANT(QString, AllProperties)

    // Constant used to associated all signals to a IngeScapeOutputBinding
    INGESCAPE_QML_PROPERTY_CONSTANT(QString, AllSignalHandlers)

    // Constant used to reset a binding
    INGESCAPE_QML_PROPERTY_CONSTANT(QString, None)


public:
    /**
     * @brief Default constructor
     * @param parent
     */
    explicit IngeScapeQuickBindingSingleton(QObject *parent = nullptr);


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
     static IngeScapeQuickBindingSingleton* instance();


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

QML_DECLARE_TYPE(IngeScapeQuickBindingSingleton)

#endif // _INGESCAPEQUICKBINDINGSINGLETON_H_
