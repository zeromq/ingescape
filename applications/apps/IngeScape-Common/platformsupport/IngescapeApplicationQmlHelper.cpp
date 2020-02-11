/*
 *	IngeScape Common
 *
 *  Copyright © 2019 Ingenuity i/o. All rights reserved.
 *
 *	See license terms for the rights and conditions
 *	defined by copyright holders.
 *
 *
 *	Contributors:
 *      Alexandre Lemort   <lemort@ingenuity.io>
 *
 */


#include "IngescapeApplicationQmlHelper.h"

#include "IngescapeApplication.h"


/**
 * @brief Constructor
 * @param parent
 */
IngescapeApplicationQmlHelper::IngescapeApplicationQmlHelper(QObject *parent)
    : QObject(parent),
      _currentWindow(nullptr)
{
    connect(this, &IngescapeApplicationQmlHelper::currentWindowChanged, this, &IngescapeApplicationQmlHelper::_onCurrentWindowChanged);
}


/**
 * @brief Destructor
 */
IngescapeApplicationQmlHelper::~IngescapeApplicationQmlHelper()
{
    disconnect(this, &IngescapeApplicationQmlHelper::currentWindowChanged, this, &IngescapeApplicationQmlHelper::_onCurrentWindowChanged);
    setcurrentWindow(nullptr);
}


/**
 * @brief Method used to provide a singleton to QML
 * @param qmlEngine
 * @param scriptEngine
 * @return
 */
QObject* IngescapeApplicationQmlHelper::qmlSingleton(QQmlEngine* qmlEngine, QJSEngine* scriptEngine)
{
    Q_UNUSED(qmlEngine)
    Q_UNUSED(scriptEngine);

    return new IngescapeApplicationQmlHelper();
}


/**
 * @brief Called when our current window has changed
 */
void IngescapeApplicationQmlHelper::_onCurrentWindowChanged()
{
    if (IngescapeApplication::instance() != nullptr)
    {
        IngescapeApplication::instance()->setcurrentWindow(_currentWindow);
    }
}
