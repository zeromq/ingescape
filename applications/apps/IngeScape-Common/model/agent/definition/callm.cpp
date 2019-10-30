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
             , const QList<QPair<QString, AgentIOPValueTypes::Value>>& arguments
             , CallM* reply
             , QObject* parent)
    : QObject(parent)
    , _name(name)
    , _description(description)
    , _reply(nullptr)
{
    // Force ownership of our object, it will prevent Qml from stealing it
    QQmlEngine::setObjectOwnership(this, QQmlEngine::CppOwnership);

    setreply(reply);

    _arguments = new QQmlPropertyMap();
    for (const QPair<QString, AgentIOPValueTypes::Value>& argumentPair : arguments)
    {
        _argumentNames.append(argumentPair.first);
        _argumentsPairList.append(argumentPair);
        _arguments->setProperty(argumentPair.first.toStdString().c_str(), argumentPair.second);
    }
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

    if (_arguments != nullptr)
    {
        QQmlPropertyMap* temp = _arguments;
        setarguments(nullptr);
        delete temp;
    }

    _argumentsPairList.clear();
    _argumentNames.clear();
}

/**
 * @brief Return the arguments as a QPair list
 * @return
 */
QList<QPair<QString, AgentIOPValueTypes::Value> > CallM::argumentsHash() const
{
    return _argumentsPairList;
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
            && left.argumentsHash() == right.argumentsHash()
            && ( (left.reply() == right.reply())
                 || ( (left.reply() != nullptr) && (right.reply() != nullptr) && (*(left.reply()) == *(right.reply())) ) );
}
