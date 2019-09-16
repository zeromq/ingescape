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

#ifndef CALLM_H
#define CALLM_H

#include <QObject>
#include <QtQml>

#include <I2PropertyHelpers.h>
extern "C" {
    #include <ingescape_advanced.h>
}

#include <model/enums.h>

/**
 * @brief Model class representing an ingescape call.
 */
class CallM : public QObject
{
    Q_OBJECT

    I2_QML_PROPERTY_READONLY(QString, name)
    I2_QML_PROPERTY_READONLY(QString, description)
    I2_QML_PROPERTY_READONLY(QQmlPropertyMap*, arguments)
    I2_QML_PROPERTY_READONLY(QStringList, argumentNames)
    I2_QML_PROPERTY_READONLY_DELETE_PROOF(CallM*, reply)

public:
    /**
     * @brief Constructor
     * @param name
     * @param description
     * @param arguments
     * @param reply
     * @param parent
     */
    explicit CallM(const QString& name, const QString& description, const QHash<QString, AgentIOPValueTypes::Value>& arguments, CallM* reply, QObject *parent = nullptr);

    /**
     * @brief Destructor
     */
    virtual ~CallM();

    /**
     * @brief Return the arguments as a QHash
     * @return
     */
    QHash<QString, AgentIOPValueTypes::Value> argumentsHash() const;
};

/**
 * @brief Equality operator to compare 2 calls
 * @param left
 * @param right
 * @return
 */
bool operator==(const CallM& left, const CallM& right);

QML_DECLARE_TYPE(CallM)

#endif // CALLM_H
