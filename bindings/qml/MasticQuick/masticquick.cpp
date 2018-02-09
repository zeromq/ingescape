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

#include "masticquickinputspropertymap.h"
#include "masticquickoutputspropertymap.h"
#include "masticquickparameterspropertymap.h"



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
    // - IOP types
    qmlRegisterSingletonType<MasticIopType>(uri, MASTICQUICK_VERSION_MAJOR, MASTICQUICK_VERSION_MINOR, "MasticIopType", &MasticIopType::qmlSingleton);
    qRegisterMetaType<MasticIopType::Value>();
    // - Log levels
    qmlRegisterSingletonType<MasticLogLevel>(uri, MASTICQUICK_VERSION_MAJOR, MASTICQUICK_VERSION_MINOR, "MasticLogLevel", &MasticLogLevel::qmlSingleton);
    qRegisterMetaType<MasticLogLevel::Value>();


    //
    // Register uncreatable types
    //
    qmlRegisterUncreatableType<MasticQuickInputsPropertyMap>(uri, MASTICQUICK_VERSION_MAJOR, MASTICQUICK_VERSION_MINOR, "MasticQuickInputsPropertyMap", "Internal class");
    qmlRegisterUncreatableType<MasticQuickOutputsPropertyMap>(uri, MASTICQUICK_VERSION_MAJOR, MASTICQUICK_VERSION_MINOR, "MasticQuickOutputsPropertyMap", "Internal class");
    qmlRegisterUncreatableType<MasticQuickParametersPropertyMap>(uri, MASTICQUICK_VERSION_MAJOR, MASTICQUICK_VERSION_MINOR, "MasticQuickParametersPropertyMap", "Internal class");



    //
    // Register our main singleton
    //
    qmlRegisterSingletonType<MasticQuickController>(uri, MASTICQUICK_VERSION_MAJOR, MASTICQUICK_VERSION_MINOR, "Mastic", &MasticQuickController::qmlSingleton);
}
