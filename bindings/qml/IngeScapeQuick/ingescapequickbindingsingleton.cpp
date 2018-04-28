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


#include "ingescapequickbindingsingleton.h"


//
// Define our singleton instance
//
Q_GLOBAL_STATIC(IngeScapeQuickBindingSingleton, _singletonInstance)



//-------------------------------------------------------------------
//
//
//  IngeScapeQuickBindingSingleton
//
//
//-------------------------------------------------------------------


/**
 * @brief Default constructor
 * @param parent
 */
IngeScapeQuickBindingSingleton::IngeScapeQuickBindingSingleton(QObject *parent) : QObject(parent),
    _AllProperties("/*! IngeScapeBinding ALL PROPERTIES !*/"),
    _AllSignalHandlers("/*! IngeScapeBinding ALL SIGNAL HANDLERS !*/"),
    _None("/*! IngeScapeBinding NOTHING !*/")
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
QObject* IngeScapeQuickBindingSingleton::qmlSingleton(QQmlEngine* engine, QJSEngine* scriptEngine)
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
IngeScapeQuickBindingSingleton* IngeScapeQuickBindingSingleton::instance()
{
    return _singletonInstance();
}



/**
 * @brief Check if a given value is a keyword of our singleton
 * @param value
 * @return
 */
bool IngeScapeQuickBindingSingleton::isKeyword(QString value)
{
    return (
            (value == _AllProperties)
            ||
            (value == _AllSignalHandlers)
            ||
            (value == _None)
            );
}


/**
 * @brief Get the keyword associated to a given value
 * @param value
 * @return
 */
QString IngeScapeQuickBindingSingleton::getKeyword(QString value)
{
    QString result;

    if (value == _AllProperties)
    {
        result = "AllProperties";
    }
    else if (value == _AllSignalHandlers)
    {
        result = "AllSignalHandlers";
    }
    else if (value == _None)
    {
        result = "None";
    }

    return result;
}
