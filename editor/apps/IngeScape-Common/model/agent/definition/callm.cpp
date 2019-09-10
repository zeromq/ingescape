/*
 *	IngeScape Common
 *
 *  Copyright © 2017-2019 Ingenuity i/o. All rights reserved.
 *
 *	See license terms for the rights and conditions
 *	defined by copyright holders.
 *
 *
 *	Contributors:
 *      Mahtieu Soum      <soum@ingenuity.io>
 *
 */

#include "callm.h"

/**
 * @brief Constructor
 * @param name
 * @param description
 * @param arguments
 * @param reply
 * @param parent
 */
CallM::CallM(const QString& name
             , const QString& description
             , const QHash<QString, AgentIOPValueTypes::Value>& arguments
             , CallM* reply
             , QObject* parent)
    : QObject(parent)
    , _name(name)
    , _description(description)
    , _reply(nullptr)
    , _arguments(arguments)
{
    // Force ownership of our object, it will prevent Qml from stealing it
    QQmlEngine::setObjectOwnership(this, QQmlEngine::CppOwnership);

    setreply(reply);
}

/**
 * @brief Destructor
 */
CallM::~CallM()
{
    if(_reply != nullptr)
    {
        CallM* temp = _reply;
        setreply(nullptr);
        delete temp;
    }

    _arguments.clear();
}

/**
 * @brief Equality operator to compare 2 calls
 * @param left
 * @param right
 * @return
 */
bool operator==(const CallM& left, const CallM& right)
{
    return left.name() == right.name()
            && left.description() == right.description()
            && left.arguments() == right.arguments()
            && ( (left.reply() == right.reply())
                 || ( (left.reply() != nullptr) && (right.reply() != nullptr) && (*(left.reply()) == *(right.reply())) ) );
}
