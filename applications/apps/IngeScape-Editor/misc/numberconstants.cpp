#include "numberconstants.h"


/**
 * @brief Default constructor inehrited from QObject
 * @param parent
 */
NumberConstants::NumberConstants(QObject* parent)
    : QObject(parent)
{
}

/**
 * @brief Method used to provide a singleton to QML
 * @param engine
 * @param scriptEngine
 * @return
 */
QObject* NumberConstants::qmlSingleton(QQmlEngine* /*engine*/, QJSEngine* /*scriptEngine*/)
{
    return new NumberConstants();
}
