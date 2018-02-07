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

#include "MasticQuick.h"

#include <QtQml>
#include <QQmlEngine>
#include <QDebug>

#include "masticquickcontroller.h"


#define MASTICQUICK_VERSION_MAJOR 1
#define MASTICQUICK_VERSION_MINOR 0


/**
 * @brief Fonction called to register all new types which are provided by this extension plugin
 * @param uri The URI of our extension plugin
 */
void MasticQuick::registerTypes(const char* uri)
{
    // Check our URI
    Q_ASSERT(uri == QLatin1String(DEFAULT_MASTICQUICK_URI));


    //
    // Register enums
    //
    qmlRegisterSingletonType<MasticIopType>(uri, MASTICQUICK_VERSION_MAJOR, MASTICQUICK_VERSION_MINOR, "MasticIopType", &MasticIopType::qmlSingleton);
    qmlRegisterSingletonType<MasticLogLevel>(uri, MASTICQUICK_VERSION_MAJOR, MASTICQUICK_VERSION_MINOR, "MasticLogLevel", &MasticLogLevel::qmlSingleton);



    //
    // Register uncreatable types
    //
    qmlRegisterUncreatableType<MasticQuickQmlPropertyMap>(uri, MASTICQUICK_VERSION_MAJOR, MASTICQUICK_VERSION_MINOR, "MasticQuickQmlPropertyMap", "Internal class");


    //
    // Register our main singleton
    //
    qmlRegisterSingletonType<MasticQuickController>(uri, MASTICQUICK_VERSION_MAJOR, MASTICQUICK_VERSION_MINOR, "Mastic", &MasticQuickController::qmlSingleton);
}
