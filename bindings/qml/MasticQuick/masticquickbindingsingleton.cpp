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



//-------------------------------------------------------------------
//
//
//  MasticQuickBindingSingleton
//
//
//-------------------------------------------------------------------


/**
 * @brief Default constructor
 * @param parent
 */
MasticQuickBindingSingleton::MasticQuickBindingSingleton(QObject *parent) : QObject(parent),
    _AllProperties("/*! ALL PROPERTIES !*/"),
    _NoProperty("/*! NO PROPERTY !*/")
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

